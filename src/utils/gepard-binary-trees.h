/* Copyright (C) 2019, Gepard Graphics
 * Copyright (C) 2019, Szilard Ledan <szledan@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GEPARD_BINARY_TREES_H
#define GEPARD_BINARY_TREES_H

#include "gepard-defs.h"
#include "gepard-region.h"
#include <cmath>
#include <iostream>
#include <iterator>
#include <memory>

namespace gepard {

template<typename _Tp>
class _LinkedBinaryTree;

template<typename _Key, typename _Tp>
class Map {
    struct _Data {
        _Key key;
        _Tp value;
        friend bool operator<(const _Data& lhs, const _Data& rhs)
        {
            return lhs.key < rhs.key;
        }
    } _sentinel;
    using LinkedBinaryTree = _LinkedBinaryTree<_Data>;
public:
    class iterator : public LinkedBinaryTree::iterator {
        typename LinkedBinaryTree::iterator _it;

    public:
        iterator(const typename LinkedBinaryTree::iterator& it) { _it = it; }

        iterator operator++(int)
        {
            iterator it = *this;
            operator++();
            return it;
        }
        iterator& operator++()
        {
            ++_it;
            return *this;
        }
        _Data& operator*() const { return _it->data; }
        _Data* operator->() const { return &(_it->data); }
        bool operator==(const iterator& rhs) const { return _it == rhs; }
        bool operator!=(const iterator& rhs) const { return _it != rhs; }
    };

    iterator begin() { return iterator(_lbt.begin()); }
    iterator end() { return iterator(_lbt.end()); }

    const size_t size() const  { return _lbt.size(); }

    iterator insert(const _Key& key)
    {
        _sentinel.key = key;
        return _lbt.uniqueInsert(_sentinel);
    }

    _Tp& operator[](const _Key& key) { return insert(key)->value; }

private:
    LinkedBinaryTree _lbt;
};

template<typename _Tp>
class MultiSet {
    using LinkedBinaryTree = _LinkedBinaryTree<_Tp>;
public:
    class iterator : public LinkedBinaryTree::iterator {
        typename LinkedBinaryTree::iterator _it;

    public:
        iterator(const typename LinkedBinaryTree::iterator& it) { _it = it; }

        iterator operator++(int)
        {
            iterator it = *this;
            operator++();
            return it;
        }
        iterator& operator++()
        {
            ++_it;
            return *this;
        }
        _Tp& operator*() const { return _it->data; }
        _Tp* operator->() const { return &(_it->data); }
        bool operator==(const iterator& rhs) const { return _it == rhs; }
        bool operator!=(const iterator& rhs) const { return _it != rhs; }
    };

    iterator begin() { return iterator(_lbt.begin()); }
    iterator end() { return iterator(_lbt.end()); }

    const size_t size() const  { return _lbt.size(); }

    iterator insert(_Tp& value)
    {
        return iterator(_lbt.multiInsert(value));
    }

private:
    LinkedBinaryTree _lbt;
};

template<typename _Tp>
class _LinkedBinaryTree {
    struct _Node {
        _Node(const _Tp& d = _Tp()) : data(d) {}

        _Tp data;
        _Node* left = nullptr;
        _Node* right = nullptr;
        _Node* next = nullptr;
        int height = 0;
    };
    using NodePtr = _Node*;

public:
    class iterator : public std::iterator<std::forward_iterator_tag, _Node> {
        NodePtr _node;

    public:
        iterator(const NodePtr n = nullptr) { _node = n; }

        iterator operator++(int)
        {
            iterator it = *this;
            operator++();
            return it;
        }
        iterator& operator++()
        {
            _node = _node->next;
            return *this;
        }
        _Node& operator*() const { return *_node; }
        NodePtr operator->() const { return _node; }
        bool operator==(const iterator& rhs) const { return _node == rhs._node; }
        bool operator!=(const iterator& rhs) const { return _node != rhs._node; }
    };

    iterator begin() { return iterator(_first); }
    iterator end() { return iterator(nullptr); }

    const size_t size() const { return _size; }
    const int rootHeight() const { return height(_root); }

    iterator uniqueInsert(const _Tp& data)
    {
        return iterator(insert<_Greater<_Tp>>(data));
    }

    iterator multiInsert(const _Tp& data)
    {
        return iterator(insert<_GreaterOrEqual<_Tp>>(data));
    }

    void displayTree()
    {
        for (auto& it : *this) {
            std::cout << std::string(2 * it.height, ' ') << it.data.value << "(" << it.height << ")" << std::endl;
        }
    }

    void displayList()
    {
        NodePtr node = _first;
        while (node) {
            std::cout << node->data.value << "(" << node->height << ") " << std::endl;
            node = node->next;
        }
    }

private:
    const int height(const NodePtr node) const
    {
        return node ? node->height : -1;
    }

    const int balanceFactor(const NodePtr node) const
    {
        GD_ASSERT(node);
        return height(node->right) - height(node->left);
    }

    void fixHeight(NodePtr node)
    {
        GD_ASSERT(node);
        node->height = std::max(height(node->left), height(node->right)) + 1;
    }

    NodePtr leftRotate(NodePtr node)
    {
        NodePtr child = node->right;
        node->right = child->left;
        child->left = node;

        node->height = std::max(height(node->left), height(node->right)) + 1;
        child->height = std::max(height(child->left), height(child->right)) + 1;
        return child;
    }

    NodePtr rightRotate(NodePtr node)
    {
        NodePtr child = node->left;
        node->left = child->right;
        child->right = node;

        node->height = std::max(height(node->left), height(node->right)) + 1;
        child->height = std::max(height(child->left), height(child->right)) + 1;
        return child;
    }

    NodePtr balance(NodePtr node)
    {
        const int bFactor = balanceFactor(node);

        if (bFactor < -1) {
            if (balanceFactor(node->left) > 0) {
                node->left = leftRotate(node->left);
            }
            return rightRotate(node);
        }

        if (bFactor > 1) {
            if (balanceFactor(node->right) < 0) {
                node->right = rightRotate(node->right);
            }
            return leftRotate(node);
        }

        return node;
    }

    NodePtr put(NodePtr& node, const _Tp& data)
    {
        GD_ASSERT(!node);
        _size++;
        return node = new (_region.alloc(sizeof(_Node))) _Node(data);
    }

    NodePtr putLeft(NodePtr& parent, const _Tp& data)
    {
        GD_ASSERT(parent);
        NodePtr node = put(parent->left, data);
        node->next = parent;
        if (_first == parent) {
            _first = node;
        } else {
            GD_ASSERT(_prev->next == parent);
            _prev->next = node;
        }
        return node;
    }

    NodePtr putRight(NodePtr& parent, const _Tp& data)
    {
        GD_ASSERT(parent);
        NodePtr node = put(parent->right, data);
        node->next = parent->next;
        parent->next = node;
        return node;
    }

    template<typename T>
    struct _Greater {
        constexpr bool operator()(const T& lhs, const T& rhs) const
        {
            return rhs < lhs;
        }
    };

    template<typename T>
    struct _GreaterOrEqual {
        constexpr bool operator()(const T& lhs, const T& rhs) const
        {
            return !(lhs < rhs);
        }
    };

    template<typename _NotLessCompare>
    NodePtr insert(const _Tp& data)
    {
        _NotLessCompare notLess;
        NodePtr node = nullptr;

        if (!_root) {
            node = _first = _root = put(_root, data);
        } else {
            NodePtr workSide, parent = _prev = _root = balance(_root);
            do {
                const bool isLesser = data < parent->data;

                if (isLesser) {
                    workSide = parent->left;
                } else if (notLess(data, parent->data)) {
                    _prev = parent;
                    workSide = parent->right;
                } else {
                    break;
                }

                if (!workSide) {
                    break;
                }
                workSide = balance(workSide);

                if (isLesser) {
                    parent->left = workSide;
                } else {
                    parent->right = workSide;
                }

                if (workSide->height >= parent->height) {
                    fixHeight(parent);
                }

                parent = workSide;
            } while (parent);
            GD_ASSERT(parent);

            if (data < parent->data) {
                node = putLeft(parent, data);
            } else if (notLess(data, parent->data)) {
                node = putRight(parent, data);
            } else {
                return parent;
            }

            fixHeight(parent);
        }

        GD_ASSERT(node);
        return node;

    }

    size_t _size = 0;
    NodePtr _root = nullptr;
    NodePtr _prev = nullptr;
    NodePtr _first = nullptr;

    Region<((8 * 1024) - (int)sizeof(void*))> _region;
};

} // namespace gepard

#endif // GEPARD_BINARY_TREES_H