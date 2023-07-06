#pragma once

#include <stdlib.h>
#include <scu/bus/a/cs0/dram-cart/dram-cart.h>
#include <mm/tlsf.h>

namespace lwram
{
    namespace internal
    {
        // Dont mess with this directly!
        constexpr size_t lwram_start = 0x00200000;
        constexpr size_t lwram_size = 0x100000;
        inline tlsf_t const pool = tlsf_pool_create((void *)lwram_start, lwram_size);

        inline bool in_range(void *ptr)
        {
            return (ptr >= (void *)lwram_start && ptr <= (char *)lwram_start + lwram_size);
        }
    }

    inline void free(void *ptr)
    {
        tlsf_free(internal::pool, ptr);
    }

    inline void *malloc(size_t size)
    {
        return tlsf_malloc(internal::pool, size);
    }

    inline void *realloc(void *ptr, size_t size)
    {
        return tlsf_realloc(internal::pool, ptr, size);
    }
}

namespace cart_ram
{
    namespace internal
    {
        // Dont mess with this directly!

        // 1mib cart should be devided into two 0.5mib sections: 0x02400000 and 0x22600000,
        // however since the first 0.5mib are mirrored we can set start at 0x22580000
        // making it work as contiguous memory and avoiding to deal with two heaps.
        constexpr size_t cart_1mib_start = 0x22580000;
        constexpr size_t cart_4mib_start = 0x22400000;

        inline tlsf_t const pool = []() -> tlsf_t
        {
            dram_cart_init();
            void *start_address;
            switch (dram_cart_id_get())
            {
            case DRAM_CART_ID_1MIB:
                start_address = (void *)cart_1mib_start;
                break;
            case DRAM_CART_ID_4MIB:
                start_address = (void *)cart_4mib_start;
                break;
            default:
                start_address = nullptr;
            }

            return tlsf_pool_create(start_address, dram_cart_size_get());
        }();

        inline bool in_range(void *ptr)
        {
            return pool
                       ? (ptr >= pool && ptr <= ((char *)pool + dram_cart_size_get()))
                       : false;
        }
    }

    inline void free(void *ptr)
    {
        if (internal::pool)
        {
            tlsf_free(internal::pool, ptr);
        }
    }

    inline void *malloc(size_t size)
    {
        return internal::pool ? tlsf_malloc(internal::pool, size) : nullptr;
    }

    inline void *realloc(void *ptr, size_t size)
    {
        return internal::pool ? tlsf_realloc(internal::pool, ptr, size) : nullptr;
    }
}

inline void auto_detect_free(void *ptr)
{
    if (lwram::internal::in_range(ptr))
    {
        return lwram::free(ptr);
    }

    if (cart_ram::internal::in_range(ptr))
    {
        return cart_ram::free(ptr);
    }

    return free(ptr);
}

inline void *auto_detect_realloc(void *ptr, size_t size)
{

    if (lwram::internal::in_range(ptr))
    {
        return lwram::realloc(ptr, size);
    }

    if (cart_ram::internal::in_range(ptr))
    {
        return cart_ram::realloc(ptr, size);
    }

    return ptr ? realloc(ptr, size) : nullptr;
}

inline void *operator new(size_t size, bool)
{
    return lwram::malloc(size);
}

#define lwram_new new (true)

inline void *operator new(size_t size, bool, bool)
{
    return cart_ram::malloc(size);
}

#define cart_new new (true, true)

inline void operator delete(void *ptr, unsigned int size)
{
    return auto_detect_free(ptr);
}
