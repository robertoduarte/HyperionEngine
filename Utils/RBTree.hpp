#pragma once

#include <stdlib.h>
#include <stdint.h>

template <typename K, typename V>
class RBTree
{
public:
    V *Insert(const K &key, const V &data)
    {
        if (root == nullptr)
        {
            root = new RBNode(key, data, Color::Black);
            return &root->data;
        }
        else
        {
            RBNode *newNode = new RBNode(key, data, Color::Red);
            AssignParent(root, newNode);
            InsertFixup(&root, newNode);
            return &newNode->data;
        }
    }

    V *Search(const K &key) const
    {
        RBNode *search = SearchNode(root, key);
        return (search) ? &search->data : nullptr;
    }

private:
    enum class Color
    {
        Black,
        Red
    };

    class RBNode
    {
    public:
        Color color;
        RBNode *left = nullptr;
        RBNode *right = nullptr;
        RBNode *parent = nullptr;
        const K key;
        V data;
        RBNode(const K &key, const V &data, const Color &color) : color(color), key(key), data(data){};
    };

    RBNode *root = nullptr;

    void SwapColor(RBNode *nodeA, RBNode *nodeB)
    {
        Color nodeAColor = nodeA->color;
        nodeA->color = nodeB->color;
        nodeB->color = nodeAColor;
    }

    void LeftRotate(RBNode **rootNode, RBNode *node)
    {
        RBNode *rightNode = node->right;

        node->right = rightNode->left;

        if (node->right != nullptr)
            node->right->parent = node;

        rightNode->parent = node->parent;

        if (node->parent == nullptr)
            *rootNode = rightNode;

        else if (node == node->parent->left)
            node->parent->left = rightNode;
        else
            node->parent->right = rightNode;

        rightNode->left = node;

        node->parent = rightNode;
    }

    void RightRotate(RBNode **rootNode, RBNode *node)
    {
        RBNode *leftNode = node->left;

        node->left = leftNode->right;

        if (node->right != nullptr)
            node->right->parent = node;

        leftNode->parent = node->parent;

        if (leftNode->parent == nullptr)
            *rootNode = leftNode;

        else if (node == node->parent->left)
            node->parent->left = leftNode;
        else
            node->parent->right = leftNode;

        leftNode->right = node;

        node->parent = leftNode;
    }

    void InsertFixup(RBNode **root, RBNode *node)
    {
        RBNode *parentPtr = nullptr;
        RBNode *grandParentPtr = nullptr;

        while ((node != *root) && (node->color != Color::Black) &&
               (node->parent->color == Color::Red))
        {

            parentPtr = node->parent;
            grandParentPtr = node->parent->parent;

            /*  Case : A
                Parent is left child of Grand-parent */
            if (parentPtr == grandParentPtr->left)
            {

                RBNode *unclePtr = grandParentPtr->right;

                /* Case : 1
                   The uncle is also red
                   Only Recoloring required */
                if (unclePtr != nullptr && unclePtr->color == Color::Red)
                {
                    grandParentPtr->color = Color::Red;
                    parentPtr->color = Color::Black;
                    unclePtr->color = Color::Black;
                    node = grandParentPtr;
                }

                else
                {
                    /* Case : 2
                       pt is right child of its parent
                       Left-rotation required */
                    if (node == parentPtr->right)
                    {
                        LeftRotate(root, parentPtr);
                        node = parentPtr;
                        parentPtr = node->parent;
                    }

                    /* Case : 3
                       pt is left child of its parent
                       Right-rotation required */
                    RightRotate(root, grandParentPtr);
                    SwapColor(parentPtr, grandParentPtr);
                    node = parentPtr;
                }
            }

            /* Case : B
               Parent is right child of Grand-parent */
            else
            {
                RBNode *unclePtr = grandParentPtr->left;

                /*  Case : 1
                    The uncle is also red
                    Only Recoloring required */
                if ((unclePtr != nullptr) && (unclePtr->color == Color::Red))
                {
                    grandParentPtr->color = Color::Red;
                    parentPtr->color = Color::Black;
                    unclePtr->color = Color::Black;
                    node = grandParentPtr;
                }
                else
                {
                    /* Case : 2
                       pt is left child of its parent
                       Right-rotation required */
                    if (node == parentPtr->left)
                    {
                        RightRotate(root, parentPtr);
                        node = parentPtr;
                        parentPtr = node->parent;
                    }

                    /* Case : 3
                       pt is right child of its parent
                       Left-rotation required */
                    LeftRotate(root, grandParentPtr);
                    SwapColor(parentPtr, grandParentPtr);
                    node = parentPtr;
                }
            }
        }

        (*root)->color = Color::Black;
    }

    void AssignParent(RBNode *root, RBNode *node)
    {
        RBNode *parent = nullptr;
        RBNode *nodeIterator = root;
        while (nodeIterator != nullptr)
        {
            parent = nodeIterator;

            if (node->key < nodeIterator->key)
                nodeIterator = nodeIterator->left;
            else
                nodeIterator = nodeIterator->right;
        }

        if (node->key > parent->key)
            parent->right = node;
        else
            parent->left = node;

        node->parent = parent;
    }

    void DeleteFixup(RBNode **root, RBNode *node)
    {
        while (node != *root && node->color == Color::Black)
        {
            if (node == node->parent->left)
            {
                RBNode *sibling = node->parent->right;
                if (sibling->color == Color::Red)
                {
                    sibling->color = Color::Black;
                    node->parent->color = Color::Red;
                    LeftRotate(root, node->parent);
                    sibling = node->parent->right;
                }
                if (sibling->left->color == Color::Black && sibling->right->color == Color::Black)
                {
                    sibling->color = Color::Red;
                    node = node->parent;
                    continue;
                }
                else if (sibling->right->color == Color::Black)
                {
                    sibling->left->color = Color::Black;
                    sibling->color = Color::Red;
                    RightRotate(root, sibling);
                    sibling = node->parent->right;
                }
                if (sibling->right->color == Color::Red)
                {
                    sibling->color = node->parent->color;
                    node->parent->color = Color::Black;
                    sibling->right->color = Color::Black;
                    LeftRotate(root, node->parent);
                    node = *root;
                }
            }
            else
            {
                RBNode *sibling = node->parent->left;
                if (sibling->color == Color::Red)
                {
                    sibling->color = Color::Black;
                    node->parent->color = Color::Red;
                    RightRotate(root, node->parent);
                    sibling = node->parent->left;
                }
                if (sibling->right->color == Color::Black && sibling->left->color == Color::Black)
                {
                    sibling->color = Color::Red;
                    node = node->parent;
                    continue;
                }
                else if (sibling->left->color == Color::Black)
                {
                    sibling->right->color = Color::Black;
                    sibling->color = Color::Red;
                    LeftRotate(root, sibling);
                    sibling = node->parent->left;
                }
                if (sibling->left->color == Color::Red)
                {
                    sibling->color = node->parent->color;
                    node->parent->color = Color::Black;
                    sibling->left->color = Color::Black;
                    RightRotate(root, node->parent);
                    node = *root;
                }
            }
        }
        node->color = Color::Black;
    }

    void Transplant(RBNode **root, RBNode *target, RBNode *replacement)
    {
        if (target->parent == nullptr)
        {
            *root = replacement;
        }
        else if (target == target->parent->left)
        {
            target->parent->left = replacement;
        }
        else
            target->parent->right = replacement;
        replacement->parent = target->parent;
    }

    RBNode *TreeMinimum(RBNode *subTreeRoot)
    {
        while (subTreeRoot->left != nullptr)
        {
            subTreeRoot = subTreeRoot->left;
        }
        return subTreeRoot;
    }

    RBNode *SearchNode(RBNode *rootNode, const K &key) const
    {
        if (rootNode == nullptr || rootNode->key == key)
            return rootNode;

        if (rootNode->key < key)
            return SearchNode(rootNode->right, key);
        else
            return SearchNode(rootNode->left, key);
    }

    bool Delete(RBNode **root, K key)
    {
        RBNode *node = SearchNode(*root, key);
        if (node == nullptr)
        {
            return false;
        }

        RBNode *temp;

        K originalColor = node->color;

        if (node->left == nullptr)
        {
            temp = node->right;
            Transplant(root, node, node->right);
        }
        else if (node->right == nullptr)
        {
            temp = node->left;
            Transplant(root, node, node->left);
        }
        else
        {
            RBNode *minimumNode = TreeMinimum(node->right);
            originalColor = minimumNode->color;
            temp = minimumNode->right;
            if (minimumNode->parent == node)
                temp->parent = minimumNode;
            else
            {
                Transplant(root, minimumNode, minimumNode->right);
                minimumNode->right = node->right;
                minimumNode->right->parent = minimumNode;
            }
            Transplant(root, node, minimumNode);
            minimumNode->left = node->left;
            minimumNode->left->parent = minimumNode;
            minimumNode->color = node->color;
        }
        delete node;

        if (originalColor == Color::Black)
            DeleteFixup(root, temp);
        return true;
    }
};
