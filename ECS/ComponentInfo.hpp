#pragma once

#include "..\Utils\TemplateUtils.hpp"
#include <stdlib.h>
#include "yaul.h"

template <typename T>
concept ComponentType = (!std::is_empty_v<T>);

class Component
{
private:
    static inline size_t counter = 0;
    static inline size_t *componentSizes = nullptr;

public:
    template <ComponentType T>
    static inline const size_t Id = []() -> size_t
    {
        const size_t id = counter++;
        componentSizes = (size_t *)realloc(componentSizes, counter * sizeof(size_t));
        componentSizes[id] = sizeof(T);
        return id;
    }();

    static size_t SizeFromId(size_t componentId)
    {
        return componentId >= counter ? 0 : componentSizes[componentId];
    }

    static size_t Count()
    {
        return counter;
    }
};