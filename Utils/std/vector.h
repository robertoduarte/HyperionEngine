#pragma once

extern "C" {
#include <stddef.h>
#include <string.h>
}

#include "utils.h"

namespace std
{

    template <typename T>
    concept SupportedIndex = std::is_same_v<T, uint16_t> || std::is_same_v<T, size_t>;

    /**
     * @brief A simple vector implementation.
     * @tparam T The type of elements stored in the vector.
     */
    template <typename T, SupportedIndex Index = size_t>
    class vector
    {
    private:
        Index capacity_; /**< The capacity of the vector. */
        Index size_;     /**< The size of the vector. */
        T* data_;         /**< Pointer to the data array. */

    public:
        /**
         * @brief Iterator class for vector.
         */
        class Iterator
        {
            friend class vector;

        public:
            /**
             * @brief Constructor for Iterator.
             * @param ptr Pointer to the element.
             */
            Iterator(T* ptr) : _ptr(ptr) {}

            /**
             * @brief Overloaded dereference operator.
             * @return Reference to the pointed element.
             */
            T& operator*() const
            {
                return *_ptr;
            }

            /**
             * @brief Overloaded pre-increment operator.
             * @return Reference to the incremented iterator.
             */
            Iterator& operator++()
            {
                ++_ptr;
                return *this;
            }

            /**
             * @brief Overloaded inequality operator.
             * @param other Another iterator for comparison.
             * @return True if iterators are not equal, false otherwise.
             */
            bool operator!=(const Iterator& other) const
            {
                return _ptr != other._ptr;
            }

            /**
             * @brief Overloaded equality operator.
             * @param other Another iterator for comparison.
             * @return True if iterators are equal, false otherwise.
             */
            bool operator==(const Iterator& other) const
            {
                return _ptr == other._ptr;
            }

            /**
             * @brief Overloaded less than operator.
             * @param other Another iterator for comparison.
             * @return True if this iterator is less than the other, false otherwise.
             */
            bool operator<(const Iterator& other) const
            {
                return _ptr < other._ptr;
            }

            /**
             * @brief Overloaded greater than operator.
             * @param other Another iterator for comparison.
             * @return True if this iterator is greater than the other, false otherwise.
             */
            bool operator>(const Iterator& other) const
            {
                return _ptr > other._ptr;
            }

            /**
             * @brief Overloaded less than or equal to operator.
             * @param other Another iterator for comparison.
             * @return True if this iterator is less than or equal to the other, false otherwise.
             */
            bool operator<=(const Iterator& other) const
            {
                return _ptr <= other._ptr;
            }

            /**
             * @brief Overloaded greater than or equal to operator.
             * @param other Another iterator for comparison.
             * @return True if this iterator is greater than or equal to the other, false otherwise.
             */
            bool operator>=(const Iterator& other) const
            {
                return _ptr >= other._ptr;
            }

            /**
             * @brief Overloaded addition assignment operator.
             * @param n Number of positions to move forward.
             * @return Iterator moved forward by n positions.
             */
            Iterator& operator+=(Index n)
            {
                _ptr += n;
                return *this;
            }

            /**
             * @brief Overloaded subtraction assignment operator.
             * @param n Number of positions to move backward.
             * @return Iterator moved back by n positions.
             */
            Iterator& operator-=(Index n)
            {
                _ptr -= n;
                return *this;
            }


            /**
             * @brief Overloaded addition operator.
             * @param n Number of positions to move forward.
             * @return Iterator moved forward by n positions.
             */
            Iterator operator+(Index n) const
            {
                return Iterator(_ptr + n);
            }

            /**
             * @brief Overloaded subtraction operator.
             * @param n Number of positions to move backward.
             * @return Iterator moved back by n positions.
             */
            Iterator operator-(Index n) const
            {
                return Iterator(_ptr - n);
            }

        private:
            T* _ptr; /**< Pointer to the element. */
        };

        /**
         * @brief Default constructor for the vector.
         */
        vector() : capacity_(0), size_(0), data_(nullptr) {}

        /**
         * @brief Destructor for the vector.
         */
        ~vector()
        {
            clear();
        }

        /**
         * @brief Copy constructor for the vector.
         * @param other Another vector to copy.
         */
        vector(const vector& other) : capacity_(other.capacity_), size_(other.size_)
        {
            data_ = new T[capacity_];
            for (Index i = 0; i < size_; ++i)
            {
                data_[i] = other.data_[i];
            }
        }

        /**
         * @brief Move constructor for the vector.
         * @param other Another vector to move.
         */
        vector(vector&& other) noexcept : capacity_(0), size_(0), data_(nullptr)
        {
            swap(*this, other);
        }

        /**
         * @brief Copy assignment operator for the vector.
         * @param other Another vector to copy.
         * @return Reference to the modified vector.
         */
        vector& operator=(vector other) noexcept
        {
            swap(*this, other);
            return *this;
        }

        /**
         * @brief Adds an element to the end of the vector.
         * @param value The value to be added.
         * @return True if successful, false otherwise.
         */
        bool push_back(T&& value)
        {
            if (size_ == capacity_)
            {
                Index new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
                if (!reserve(new_capacity))
                {
                    return false;
                }
            }
            data_[size_] = std::move(value);
            ++size_;
            return true;
        }

        /**
         * @brief Adds a copy of an element to the end of the vector.
         * @param value The value to be added.
         * @return True if successful, false otherwise.
         */
        bool push_back(const T& value)
        {
            if (size_ == capacity_)
            {
                Index new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
                if (!reserve(new_capacity))
                {
                    return false;
                }
            }
            data_[size_] = value;
            ++size_;
            return true;
        }

        /**
         * @brief Removes the last element from the vector.
         */
        void pop_back()
        {
            if (size_ > 0)
            {
                --size_;
            }
        }

        /**
         * @brief Overloaded subscript operator.
         * @param index The index of the element to access.
         * @return Reference to the element at the specified index.
         */
        T& operator[](Index index)
        {
            return data_[index];
        }

        /**
         * @brief Reserves space for at least a specified number of elements.
         * @param new_capacity The new capacity to reserve.
         * @return True if successful, false otherwise.
         */
        bool reserve(Index new_capacity)
        {
            if (new_capacity > capacity_)
            {
                T* new_data = new T[new_capacity];
                if (!new_data)
                {
                    return false;
                }

                for (Index i = 0; i < size_; ++i)
                {
                    new_data[i] = std::move(data_[i]);
                }

                delete[] data_;
                data_ = new_data;
                capacity_ = new_capacity;
            }
            return true;
        }

        /**
         * @brief Reduces the capacity to fit the current size.
         * @return True if successful, false otherwise.
         */
        bool shrink_to_fit()
        {
            if (size_ < capacity_)
            {
                T* new_data = new T[size_];
                if (!new_data)
                {
                    return false;
                }

                for (Index i = 0; i < size_; ++i)
                {
                    new_data[i] = std::move(data_[i]);  // Use move semantics here
                }

                delete[] data_;
                data_ = new_data;
                capacity_ = size_;
            }
            return true;
        }

        /**
         * @brief Resizes the vector to contain a specified number of elements.
         * @param new_size The new size of the vector.
         */
        bool resize(Index new_size)
        {
            if (new_size > capacity_)
            {
                if (!reserve(new_size))
                    return false;
            }
            size_ = new_size;
            return true;
        }

        /**
         * @brief Clears the vector, removing all elements.
         */
        void clear()
        {
            delete[] data_;
            data_ = nullptr;
            capacity_ = 0;
            size_ = 0;
        }

        /**
         * @brief Removes an element at a specified index.
         * @param index The index of the element to remove.
         */
        void erase(Index index)
        {
            erase(begin() + index);
        }

        /**
         * @brief Removes the element at a specified position.
         * @param position Iterator pointing to the element to remove.
         */
        void erase(Iterator position)
        {
            if (position.ptr < data_ || position.ptr >= data_ + size_)
            {
                return;
            }

            Iterator last = end() - 1;

            if (position != last)
            {
                *position = std::move(*last);
            }

            --size_;
        }

        /**
         * @brief Returns the size of the vector.
         * @return The size of the vector.
         */
        Index size() const
        {
            return size_;
        }

        /**
         * @brief Checks if the vector is empty.
         * @return True if empty, false otherwise.
         */
        bool empty() const
        {
            return size_ == 0;
        }

        /**
         * @brief Returns the current capacity of the vector.
         * @return The capacity of the vector.
         */
        Index capacity() const
        {
            return capacity_;
        }

        /**
         * @brief Returns an iterator pointing to the first element of the vector.
         * @return Iterator pointing to the first element.
         */
        Iterator begin()
        {
            return Iterator(data_);
        }

        /**
         * @brief Returns an iterator pointing to the position one past the last element of the vector.
         * @return Iterator pointing to one past the last element.
         */
        Iterator end()
        {
            return Iterator(data_ + size_);
        }

        /**
         * @brief Returns a reference to the last element of the vector.
         * @return Reference to the last element.
         */
        T& back()
        {
            return data_[size_ - 1];
        }

        /**
         * @brief Swaps the content of two vectors.
         * @param first The first vector to swap.
         * @param second The second vector to swap.
         */
        friend void swap(vector& first, vector& second) noexcept
        {
            using std::swap;
            swap(first.capacity_, second.capacity_);
            swap(first.size_, second.size_);
            swap(first.data_, second.data_);
        }

    };
}
