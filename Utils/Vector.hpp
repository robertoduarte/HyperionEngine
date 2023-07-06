#pragma once

#include "SatAlloc.hpp"

#include <stddef.h>
#include <string.h>

template <typename T>
class Vector
{
public:
	Vector() : capacity_(0), size_(0), data_(nullptr) {}

	~Vector()
	{
		clear();
	}

	bool push_back(const T &value)
	{
		if (size_ == capacity_)
		{
			size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
			if (!reserve(new_capacity))
			{
				return false;
			}
		}
		memcpy(static_cast<void *>(data_ + size_), static_cast<const void *>(&value), sizeof(T));
		++size_;
		return true;
	}

	template <typename Func>
	bool push_back_init(Func &&initializer)
	{
		if (size_ == capacity_)
		{
			size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
			if (!reserve(new_capacity))
			{
				return false;
			}
		}
		initializer(data_[size_]);
		++size_;
		return true;
	}

	void pop_back()
	{
		if (size_ > 0)
		{
			--size_;
		}
	}

	const T *operator[](size_t index) const
	{
		if (index >= size_)
		{
			return nullptr;
		}
		return &data_[index];
	}

	T *operator[](size_t index)
	{
		return const_cast<T *>(static_cast<const Vector &>(*this)[index]);
	}

	bool reserve(size_t new_capacity)
	{
		if (new_capacity > capacity_)
		{
			T *new_data = static_cast<T *>(realloc(data_, new_capacity * sizeof(T)));
			if (!new_data)
			{
				return false;
			}
			data_ = new_data;
			capacity_ = new_capacity;
		}
		return true;
	}

	void resize(size_t new_size)
	{
		if (new_size > capacity_)
		{
			reserve(new_size);
		}
		size_ = new_size;
	}

	void clear()
	{
		if (data_)
		{
			free(data_);
			data_ = nullptr;
		}
		capacity_ = 0;
		size_ = 0;
	}

	void erase(size_t index)
	{
		if (index >= size_)
		{
			return;
		}

		if (index != size_ - 1)
		{
			// Move the last element to the deleted position
			memcpy(static_cast<void *>(data_ + index), static_cast<const void *>(data_ + size_ - 1), sizeof(T));
		}

		--size_;
	}

	size_t size() const
	{
		return size_;
	}

	size_t capacity() const
	{
		return capacity_;
	}

private:
	size_t capacity_;
	size_t size_;
	T *data_;
};