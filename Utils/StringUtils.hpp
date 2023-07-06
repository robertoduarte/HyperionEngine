#pragma once

#include "std\type_traits.h"

namespace StringUtils
{
    template <typename T>
    size_t Parse(T &dest, const char *srcStr)
    {
        if (!srcStr || srcStr[0] == '\0')
            return 0;
        if constexpr (std::is_integral_v<T>)
        {
            [[maybe_unused]] bool isNegative = false;

            const char *tmpStr = srcStr;

            while (*tmpStr == ' ')
                tmpStr++;

            if constexpr (std::is_signed_v<T>)
                if (*tmpStr == '-')
                {
                    isNegative = true;
                    tmpStr++;
                }

            dest = 0;
            while (*tmpStr >= '0' && *tmpStr <= '9')
            {
                dest = (dest * 10) + (*tmpStr - '0');
                tmpStr++;
            }

            if constexpr (std::is_signed_v<T>)
                if (isNegative)
                    dest = -dest;

            return tmpStr - srcStr;
        }
        else
        {
            if constexpr (!std::is_empty_v<T>)
                return dest.Parse(srcStr);
            else
                return 0;
        }
    }

    bool SkipUntil(const char **str, const char token)
    {
        const char *tmpStr = *str;
        while (*tmpStr != token)
        {
            if (*tmpStr == '\0')
                return false;
            tmpStr++;
        }

        *str = tmpStr;
        return true;
    }
};
