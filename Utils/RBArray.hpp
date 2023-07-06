#pragma once

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "std\type_traits.h"
#include "IdTracker.hpp"

namespace std
{
    template <typename T>
    constexpr std::remove_reference_t<T> &&move(T &&arg) noexcept
    {
        return static_cast<std::remove_reference_t<T> &&>(arg);
    }
};

template <typename T>
concept ComparableType = requires(T a, T b) {
    {
        a != b
    };
    {
        a == b
    };
    {
        a < b
    };
    {
        a > b
    };
};

template <typename T>
    requires ComparableType<T>
struct ConstIdentifiable
{
    using IdType = T;
    const T id;

    ConstIdentifiable() : id() {}
    ConstIdentifiable(T id) : id(id) {}
    ConstIdentifiable(const ConstIdentifiable &) = delete;            // Disable the copy constructor
    ConstIdentifiable &operator=(const ConstIdentifiable &) = delete; // Disable the copy assignment operator

    ConstIdentifiable(ConstIdentifiable &&other) noexcept : id(other.id)
    {
        const_cast<T &>(other.id) = T();
    }

    ConstIdentifiable &operator=(ConstIdentifiable &&other) noexcept
    {
        if (this != &other)
        {
            const_cast<T &>(id) = other.id;
            const_cast<T &>(other.id) = T();
        }
        return *this;
    }
};

template <typename T>
concept DerivedFromConstIdentifiable = std::is_base_of_v<ConstIdentifiable<typename T::IdType>, T>;

template <typename T, size_t maxNodes, typename IndexType = uint32_t>
    requires DerivedFromConstIdentifiable<T>
struct FixedRBArray
{
    static constexpr size_t wordSize = CHAR_BIT * sizeof(IndexType);
    static constexpr size_t indexBitWidth = (wordSize - 1) / 3;
    static constexpr IndexType fullWord = ~((IndexType)0);
    static constexpr IndexType InvalidNode = fullWord >> (wordSize - indexBitWidth);
    static constexpr size_t maxIndex = InvalidNode - 1;

    enum class Color
    {
        Black,
        Red
    };

    struct Node
    {
        IndexType parent : indexBitWidth = InvalidNode;
        IndexType left : indexBitWidth = InvalidNode;
        IndexType right : indexBitWidth = InvalidNode;
        Color color : 1 = Color::Black;
    };

    static_assert(sizeof(Node) == sizeof(IndexType));
    static_assert(maxNodes < maxIndex);

    IndexType rootNode = InvalidNode;
    FixedIdTracker<maxNodes> nodeTracker;
    Node nodeArray[maxNodes];
    T objectArray[maxNodes];

    /* Basic Node Operations */

    bool IsValid(IndexType node) { return node != InvalidNode; }

    const typename T::IdType &IdOf(IndexType node) { return objectArray[node].Id(); }

#define LeftOf(node) nodeArray[node].left
#define RightOf(node) nodeArray[node].right
#define ParentOf(node) nodeArray[node].parent
#define ColorOf(node) nodeArray[node].color

    /* Main Node Operations */

    void SwapColor(IndexType nodeA, IndexType nodeB)
    {
        Color nodeBColor = ColorOf(nodeB);
        ColorOf(nodeB) = ColorOf(nodeA);
        ColorOf(nodeA) = nodeBColor;
    }

    void AssignParent(IndexType node)
    {
        IndexType iterator = rootNode;
        IndexType parent;

        const typename T::IdType &idOfNode = IdOf(node);
        while (IsValid(iterator))
        {
            parent = iterator;
            iterator = (idOfNode > IdOf(iterator)) ? RightOf(iterator) : LeftOf(iterator);
        }

        IndexType &childRef = (idOfNode > IdOf(iterator)) ? RightOf(parent) : LeftOf(parent);
        childRef = node;

        ParentOf(node) = parent;
    }

    void LeftRotate(IndexType node)
    {
        IndexType rightNode = RightOf(node);
        IndexType parentNode = ParentOf(node);

        RightOf(node) = LeftOf(rightNode);

        if (IsValid(rightNode))
            ParentOf(rightNode) = node;

        ParentOf(rightNode) = parentNode;

        if (!IsValid(parentNode))
            rootNode = rightNode;

        else if (node == LeftOf(parentNode))
            LeftOf(parentNode) = rightNode;
        else
            RightOf(parentNode) = rightNode;

        LeftOf(rightNode) = node;

        ParentOf(node) = rightNode;
    }

    void RightRotate(IndexType node)
    {
        IndexType rightNode = RightOf(node);
        IndexType leftNode = LeftOf(node);
        IndexType parentNode = ParentOf(node);

        LeftOf(node) = LeftOf(leftNode);

        if (IsValid(rightNode))
            ParentOf(rightNode) = node;

        ParentOf(leftNode) = parentNode;

        if (!IsValid(parentNode))
            rootNode = leftNode;

        else if (node == LeftOf(parentNode))
            LeftOf(parentNode) = leftNode;
        else
            RightOf(parentNode) = leftNode;

        RightOf(leftNode) = node;

        ParentOf(node) = leftNode;
    }

    void InsertFixup(IndexType node)
    {
        while ((node != rootNode) &&
               (ColorOf(node) != Color::Black) &&
               (ColorOf(ParentOf(node)) == Color::Red))
        {
            IndexType parent = ParentOf(node);
            IndexType grandParent = ParentOf(parent);
            if (parent == LeftOf(grandParent))
            {
                IndexType rightUncle = RightOf(grandParent);
                if (IsValid(rightUncle) && ColorOf(rightUncle) == Color::Red)
                {
                    ColorOf(grandParent) = Color::Red;
                    ColorOf(parent) = Color::Black;
                    ColorOf(rightUncle) = Color::Black;
                    node = grandParent;
                }

                else
                {
                    if (node == RightOf(parent))
                    {
                        LeftRotate(parent);
                        node = parent;
                        parent = node;
                    }
                    RightRotate(grandParent);
                    SwapColor(parent, grandParent);
                    node = parent;
                }
            }
            else
            {
                IndexType leftUncle = LeftOf(grandParent);
                if (IsValid(leftUncle) && ColorOf(leftUncle) == Color::Red)
                {
                    ColorOf(grandParent) = Color::Red;
                    ColorOf(parent) = Color::Black;
                    ColorOf(leftUncle) = Color::Black;
                    node = grandParent;
                }

                else
                {
                    if (node == RightOf(parent))
                    {
                        RightRotate(parent);
                        node = parent;
                        parent = node;
                    }
                    LeftRotate(grandParent);
                    SwapColor(parent, grandParent);
                    node = parent;
                }
            }
        }

        ColorOf(rootNode) = Color::Black;
    }

    IndexType Insert(T &object)
    {
        IndexType node = InvalidNode;
        if (nodeTracker.AssingId(node))
        {
            objectArray[node] = std::move(object);

            if (rootNode == InvalidNode)
            {
                rootNode = node;
            }
            else
            {
                AssignParent(node);
                InsertFixup(node);
            }
        }
        return node;
    }

    void DeleteFixup(IndexType node)
    {
        while ((node != rootNode) && (ColorOf(node) == Color::Black))
        {
            IndexType parent = ParentOf(node);
            if (node == LeftOf(parent))
            {
                IndexType sibling = RightOf(parent);
                if (ColorOf(sibling) == Color::Red)
                {
                    ColorOf(sibling) = Color::Black;
                    ColorOf(parent) = Color::Red;
                    LeftRotate(parent);
                    sibling = RightOf(parent);
                }
                if (ColorOf(LeftOf(sibling)) == Color::Black &&
                    ColorOf(RightOf(sibling)) == Color::Black)
                {
                    ColorOf(sibling) = Color::Red;
                    node = parent;
                    continue;
                }
                else if (ColorOf(RightOf(sibling)) == Color::Black)
                {
                    ColorOf(LeftOf(sibling)) = Color::Black;
                    ColorOf(sibling) = Color::Red;
                    RightRotate(sibling);
                    sibling = RightOf(parent);
                }
                if (ColorOf(RightOf(sibling)) == Color::Red)
                {
                    ColorOf(sibling) = ColorOf(parent);
                    ColorOf(parent) = Color::Black;
                    ColorOf(RightOf(sibling)) = Color::Black;
                    LeftRotate(parent);
                    node = rootNode;
                }
            }
            else
            {
                IndexType sibling = LeftOf(parent);
                if (ColorOf(sibling) == Color::Red)
                {
                    ColorOf(parent) = Color::Black;
                    ColorOf(parent) = Color::Red;
                    RightRotate(parent);
                    sibling = LeftOf(parent);
                }
                if (ColorOf(RightOf(sibling)) == Color::Black &&
                    ColorOf(LeftOf(sibling)) == Color::Black)
                {
                    ColorOf(sibling) = Color::Red;
                    node = parent;
                    continue;
                }
                else if (ColorOf(LeftOf(sibling)) == Color::Black)
                {
                    ColorOf(RightOf(sibling)) = Color::Black;
                    ColorOf(sibling) = Color::Red;
                    LeftRotate(sibling);
                    sibling = LeftOf(parent);
                }
                if (ColorOf(LeftOf(sibling)) == Color::Red)
                {
                    ColorOf(sibling) = ColorOf(parent);
                    ColorOf(parent) = Color::Black;
                    ColorOf(LeftOf(sibling)) = Color::Black;
                    RightRotate(parent);
                    node = rootNode;
                }
            }
        }
        ColorOf(node) = Color::Black;
    }

    void Transplant(IndexType target, IndexType source)
    {
        IndexType parentOfTarget = ParentOf(target);
        if (!IsValid(parentOfTarget))
        {
            rootNode = source;
        }
        else if (target == LeftOf(parentOfTarget))
        {
            LeftOf(parentOfTarget) = source;
        }
        else
        {
            RightOf(parentOfTarget) = source;
        }

        ParentOf(source) = parentOfTarget;
    }

    IndexType TreeMinimum(IndexType node) const
    {
        IndexType leftOfNode;
        while (IsValid(leftOfNode = LeftOf(node)))
        {
            node = leftOfNode;
        }
        return node;
    }

    IndexType SearchIndex(const typename T::IdType &id) const
    {
        IndexType iterator = rootNode;
        while (IsValid(iterator))
        {
            const typename T::IdType &idOfIterator = IdOf(iterator);
            if (idOfIterator == id)
                return iterator;

            iterator = (id > idOfIterator) ? RightOf(iterator) : LeftOf(iterator);
        }

        return InvalidNode;
    }

    T *GetNode(IndexType nodeIndex) const
    {
        return nodeTracker.IsUsed(nodeIndex) ? &objectArray[nodeIndex] : nullptr;
    }

    T *Search(const typename T::IdType &id) const
    {
        IndexType nodeIndex = SearchIndex(id);
        return IsValid(nodeIndex) ? &objectArray[nodeIndex] : nullptr;
    }

    void Delete(typename T::IdType id)
    {
        IndexType node = SearchIndex(id);
        if (IsValid(node))
        {
            IndexType leftNode = LeftOf(node);
            IndexType rightNode = RightOf(node);
            IndexType temp;

            Color originalColor = ColorOf(node);

            if (IsValid(leftNode))
            {
                temp = rightNode;
                Transplant(node, rightNode);
            }
            else if (IsValid(rightNode))
            {
                temp = leftNode;
                Transplant(node, leftNode);
            }
            else
            {
                IndexType minimumNode = TreeMinimum(rightNode);
                originalColor = ColorOf(minimumNode);
                temp = RightOf(minimumNode);
                if (ParentOf(minimumNode) == node)
                {
                    ParentOf(temp) = minimumNode;
                }
                else
                {
                    Transplant(minimumNode, RightOf(minimumNode));
                    RightOf(minimumNode) = rightNode;
                    ParentOf(RightOf(minimumNode)) = minimumNode;
                }
                Transplant(node, minimumNode);
                LeftOf(minimumNode) = leftNode;
                ParentOf(LeftOf(minimumNode)) = minimumNode;
                ColorOf(minimumNode) = ColorOf(node);
            }
            objectArray[node] = std::move(T());

            if (originalColor == Color::Black)
            {
                DeleteFixup(temp);
            }
        }
    }
};