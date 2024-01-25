#pragma once

#include "utils.h"
#include <string.h>

namespace std
{
    class string
    {
    private:
        char* str = nullptr;

    public:
        // Default constructor
        string() : str(nullptr) {}

        // Constructor with source string
        string(const char* src)
        {
            str = new char[strlen(src) + 1];
            strcpy(str, src);
        }

        template <typename... Args>
        string(const char* format, Args... args)
        {
            size_t size = snprintf(nullptr, 0, format, args...) + 1;
            str = new char[size];
            snprintf(str, size, format, args...);
        }

        template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
        string(const T& integer)
        {
            size_t size = snprintf(nullptr, 0, "%d", integer) + 1;
            str = new char[size];
            snprintf(str, size, "%d", integer);
        }

        // Copy constructor
        string(const string& other)
        {
            str = new char[strlen(other.str) + 1];
            strcpy(str, other.str);
        }

        // Copy assignment operator
        string& operator=(const string& other)
        {
            if (this != &other)
            {
                delete[] str;
                str = new char[strlen(other.str) + 1];
                strcpy(str, other.str);
            }
            return *this;
        }

        // Move constructor
        string(string&& other) noexcept
            : str(other.str)
        {
            other.str = nullptr;
        }

        // Move assignment operator
        string& operator=(string&& other) noexcept
        {
            if (this != &other)
            {
                delete[] str;
                str = other.str;
                other.str = nullptr;
            }
            return *this;
        }

        // Overloaded + operator for string concatenation
        string operator+(const string& other) const
        {
            string result;
            result.str = new char[strlen(str) + strlen(other.str) + 1];
            strcpy(result.str, str);
            strcat(result.str, other.str);
            return result;
        }

        // Destructor
        ~string()
        {
            delete[] str;
        }


        const char* c_str() const
        {
            return str;
        }
    };
}
