#pragma once

#include "Mat43.hpp"


#include "..\Utils\std\vector.h"

class MatrixStack
{
private:
    std::vector<Mat43> stack;

public:
    /**
     * @brief Push a matrix onto the stack.
     * @param matrix The matrix to push.
     */
    void Push(const Mat43& matrix)
    {
        stack.push_back(matrix);
    }

    /**
     * @brief Pop the top matrix from the stack.
     */
    void Pop()
    {
        if (!stack.empty())
        {
            stack.pop_back();
        }
    }

    /**
     * @brief Get the top matrix from the stack.
     * @return The top matrix on the stack.
     */
    Mat43 Top()
    {
        if (!stack.empty())
        {
            return stack.back();
        }

        // If the stack is empty, return the identity matrix.
        return Mat43::Identity();
    }

    /**
     * @brief Get the combined matrix from the stack.
     * @return The combined matrix.
     */
    Mat43 GetCombinedMatrix()
    {
        Mat43 combinedMatrix = Mat43::Identity();

        for (const auto& matrix : stack)
        {
            combinedMatrix = combinedMatrix * matrix;
        }

        return combinedMatrix;
    }

    /**
     * @brief Clear the matrix stack.
     */
    void Clear()
    {
        stack.clear();
    }

    void Compact()
    {
        stack.shrink_to_fit();
    }
};
