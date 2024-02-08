#pragma once

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "..\Utils\std\vector.h"

namespace Hyperion::ECS
{
    /**
     * @brief Base class for ECS components.
     */
    class Component
    {
    private:
        /**
         * @brief Helper struct for determining if an ID has been used.
         */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-template-friend"
        template <size_t N>
        struct IdReader
        {
            friend auto IsCountedFlag(IdReader<N>);
        };
#pragma GCC diagnostic pop

        /**
         * @brief Helper struct for setting the ID.
         */
        template <size_t N>
        struct IdSetter
        {
            friend auto IsCountedFlag(IdReader<N>) {}
            static constexpr size_t n = N;
        };

        /**
         * @brief Retrieves the next available ID.
         *
         * @tparam Tag Unused template parameter.
         * @tparam NextVal The next available ID.
         * @return constexpr auto The next available ID.
         */
        template <auto Tag, size_t NextVal = 0>
        [[nodiscard]] static consteval auto GetNextID()
        {
            constexpr bool isCountedPastValue =
                requires(IdReader<NextVal> r) { IsCountedFlag(r); };

            if constexpr (isCountedPastValue)
            {
                return GetNextID<Tag, NextVal + 1>();
            }
            else
            {
                IdSetter<NextVal> s;
                return s.n;
            }
        }

        /**
         * @brief Deletes an array of type T.
         *
         * @tparam T The type of the array elements.
         * @param array Pointer to the array to be deleted.
         */
        template<typename T>
        static void DeleteArray(void* array)
        {
            delete[] static_cast<T*>(array);
        }

        /**
         * @brief Moves an element from one array to another.
         *
         * @tparam T The type of the array elements.
         * @param dstArray Pointer to the destination array.
         * @param dstPos The position in the destination array.
         * @param srcArray Pointer to the source array.
         * @param srcPos The position in the source array.
         */
        template<typename T>
        static void MoveElement(void* dstArray, size_t dstPos, void* srcArray, size_t srcPos)
        {
            // Move data from source object to destination object
            static_cast<T*>(dstArray)[dstPos] = std::move(static_cast<T*>(srcArray)[srcPos]);

            // Check if the type T has a default constructor
            if constexpr (std::is_default_constructible_v<T>)
            {
                // Reset the original object to a default state
                new (&static_cast<T*>(srcArray)[srcPos]) T{};
            }
        }

        /**
         * @brief Resizes an array of a specific component type.
         *
         * @tparam T The type of the array elements.
         * @param ptrToArray Pointer to the array to be resized.
         * @param newSize The new size of the array.
         * @param moveCount The number of elements to move from the original array to the resized array.
         * @return true If the array was resized successfully.
         * @return false If resizing failed.
         */
        template <typename T>
        static bool ResizeArray(void** ptrToArray, size_t newSize, size_t moveCount)
        {
            // Allocate memory for the resized array
            T* newArray = new T[newSize];
            if (!newArray)
            {
                return false; // Return false if allocation fails
            }

            T* originalArray = *reinterpret_cast<T**>(ptrToArray);

            // Move elements from the original array to the resized array
            for (size_t i = 0; i < moveCount; ++i)
            {
                newArray[i] = std::move(originalArray[i]);
            }

            // Delete the original array
            delete[] originalArray;

            // Update the pointer to point to the resized array
            *reinterpret_cast<T**>(ptrToArray) = newArray;

            return true; // Return true if resizing is successful
        }
        // Typedefs for function pointers
        using DeleteArrayInterface = void (*)(void* array);
        using MoveElementInterface = void(*)(void* dstArray, size_t dstPos, void* srcArray, size_t srcPos);
        using ResizeArrayInterface = bool(*)(void** ptrToArray, size_t newSize, size_t moveCount);

        // Struct to hold operation function pointers
        struct Operation
        {
            DeleteArrayInterface DeleteArray;       /**< Function pointer to delete an array. */
            MoveElementInterface MoveElement;       /**< Function pointer to move an element from one array to another. */
            ResizeArrayInterface ResizeArray;       /**< Function pointer to resize an array. */
        };

        static inline std::vector<Operation> OperationList;    /**< Vector to hold operation function pointers. */

    public:
        using BinaryId = uint64_t;                          /**< Alias for component ID in binary format. */
        static inline constexpr size_t MaxComponentTypes = sizeof(BinaryId) * CHAR_BIT;   /**< Maximum number of component types. */

        /**
         * @brief Retrieves the ID of a component type.
         *
         * @tparam T The type of the component.
         */
        template <typename T>
        static inline constexpr size_t Id = GetNextID < [] {} > ();

        /**
         * @brief Retrieves the binary ID of a component type.
         *
         * @tparam T The type of the component.
         */
        template <typename T>
        static inline BinaryId IdBinary = []()
        {
            constexpr size_t id = Id<T>;
            constexpr size_t size = id + 1;

            if (OperationList.size() < size)
            {
                OperationList.resize(size);
            }

            OperationList[id] = Operation(&DeleteArray<T>, &MoveElement<T>, &ResizeArray<T>);

            return (BinaryId)1 << id;
        }();

        /**
         * @brief Deletes an array of a specific component type.
         *
         * @param componentId The ID of the component type.
         * @param array Pointer to the array to be deleted.
         */
        static void DeleteArray(size_t componentId, void* array)
        {
            OperationList[componentId].DeleteArray(array);
        }

        /**
         * @brief Moves an element from one array to another.
         *
         * @param componentId The ID of the component type.
         * @param dstArray Pointer to the destination array.
         * @param dstPos The position in the destination array.
         * @param srcArray Pointer to the source array.
         * @param srcPos The position in the source array.
         */
        static void MoveElement(size_t componentId, void* dstArray, size_t dstPos, void* srcArray, size_t srcPos)
        {
            OperationList[componentId].MoveElement(dstArray, dstPos, srcArray, srcPos);
        }

        /**
         * @brief Resizes an array of a specific component type.
         *
         * @param componentId The ID of the component type.
         * @param ptrToArray Pointer to the array to be resized.
         * @param newSize The new size of the array.
         * @param moveCount The number of elements to move from the original array to the resized array.
         * @return true If the array was resized successfully.
         * @return false If resizing failed.
         */
        static bool ResizeArray(size_t componentId, void** ptrToArray, size_t newSize, size_t moveCount)
        {
            return OperationList[componentId].ResizeArray(ptrToArray, newSize, moveCount);
        }

    };
}
