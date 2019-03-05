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

namespace gepard {

template<typename _Tp, const size_t EXPECTED_MIN_NODE = 64>
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
    typedef typename LinkedBinaryTree::iterator iterator;

    iterator begin() { return iterator(_lbt.begin()); }
    iterator end() { return iterator(_lbt.end()); }

    const size_t size() const  { return _lbt.size(); }

    iterator find(const _Key& key)
    {
        _sentinel.key = key;
        return _lbt.find(_sentinel);
    }

    iterator insert(const _Key& key)
    {
        _sentinel.key = key;
        return _lbt.uniqueInsert(_sentinel);
    }

    _Tp& operator[](const _Key& key) { return insert(key)->node->data.value; }
    const _Tp& operator[](const _Key& key) const { return find(_sentinel)->node->data.value; }

    void displayList()
    {
        for (auto& it : _lbt) {
            std::cout << it.data.key << " ";
        }
        std::cout << std::endl;
    }

private:
    LinkedBinaryTree _lbt;
};

template<typename _Tp>
class MultiSet {
    using LinkedBinaryTree = _LinkedBinaryTree<_Tp>;

public:
    typedef typename LinkedBinaryTree::iterator iterator;

    iterator begin() { return _lbt.begin(); }
    iterator end() { return _lbt.end(); }

    const size_t size() const  { return _lbt.size(); }

    iterator find(const _Tp& value)
    {
        return _lbt.find(value);
    }

    iterator insert(const _Tp& value)
    {
        return _lbt.multiInsert(value);
    }

private:
    LinkedBinaryTree _lbt;
};

template<typename _Tp, const size_t EXPECTED_MIN_NODE>
class _LinkedBinaryTree {
    struct _Node {
        _Node(const _Tp& d = _Tp()) : data(d) {}

        _Node* left = nullptr;
        _Node* right = nullptr;
        _Node* next = nullptr;
        int8_t height = 0;
        _Tp data;
    };
    using NodePtr = _Node*;

    template<typename _T>
    class _iterator : public std::iterator<std::forward_iterator_tag, _T> {
        _T _data;

    public:
        _iterator(_T d = _T()) : _data(d) {}

        _iterator operator++(int)
        {
            iterator it = *this;
            operator++();
            return it;
        }
        _iterator& operator++()
        {
            _data.prev = _data.node;
            _data.node = _data.node->next;
            return *this;
        }
        _T& operator*() const { return _data; }
        const _T* operator->() const { return &_data; }
        bool operator==(const _iterator& rhs) const { return _data.node == rhs._data.node; }
        bool operator!=(const _iterator& rhs) const { return _data.node != rhs._data.node; }
    };

public:
    struct RetType {
        NodePtr node;
        NodePtr prev;
        enum Type {
            Invlaid = -1,
            Find = 0,
            Insert = 1,
            First = 2,
        } type;
        RetType (const NodePtr n = nullptr, const NodePtr p = nullptr, const Type t = Invlaid) : node(n), prev(p), type(t) {}
    };

    typedef _iterator<RetType> iterator;

    iterator begin() { return iterator(_first); }
    iterator end() { return iterator(nullptr); }

    const size_t size() const { return _size; }
    const int8_t rootHeight() const { return height(_root); }

    iterator uniqueInsert(const _Tp& data)
    {
        return iterator(insert<_Greater<_Tp>>(data));
    }

    iterator multiInsert(const _Tp& data)
    {
        return iterator(insert<_GreaterOrEqual<_Tp>>(data));
    }

    iterator find(const _Tp& data)
    {
        NodePtr prev, node = prev = _root;
        while (node) {
            if (data < node->data) {
                node = node->left;
            } else if (node->data < data) {
                prev = node;
                node = node->right;
            } else {
                break;
            }
        }
        return iterator(RetType(node, prev, RetType::Find));
    }

private:
    const int8_t height(const NodePtr node) const
    {
        return node ? node->height : -1;
    }

    const int8_t balanceFactor(const NodePtr node) const
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
        const int8_t bFactor = balanceFactor(node);

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

    NodePtr putLeft(NodePtr parent, NodePtr prev, const _Tp& data)
    {
        GD_ASSERT(parent);
        NodePtr node = put(parent->left, data);
        node->next = parent;
        if (GD_UNLIKELY(_first == parent)) {
            _first = node;
        } else {
            GD_ASSERT(prev->next == parent);
            prev->next = node;
        }
        return node;
    }

    NodePtr putRight(NodePtr parent, const _Tp& data)
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
    RetType insert(const _Tp& data)
    {
        _NotLessCompare notLess;
        NodePtr node = nullptr, prev = nullptr;
        typename RetType::Type type = RetType::Insert;

        if (GD_UNLIKELY(!_root)) {
            node = _first = _root = put(_root, data);
            type = RetType::First;
        } else {
            NodePtr workSide, parent = prev = _root = balance(_root);
            do {
                const bool isLesser = data < parent->data;

                if (isLesser) {
                    workSide = parent->left;
                } else if (notLess(data, parent->data)) {
                    prev = parent;
                    workSide = parent->right;
                } else {
                    break;
                }

                if (GD_UNLIKELY(!workSide)) {
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
            } while (true);
            GD_ASSERT(parent);

            if (data < parent->data) {
                node = putLeft(parent, prev, data);
                if (GD_UNLIKELY(node == _first)) {
                    type = RetType::First;
                }
            } else if (notLess(data, parent->data)) {
                node = putRight(parent, data);
            } else {
                return RetType(parent, prev, RetType::Find);
            }

            fixHeight(parent);
        }

        GD_ASSERT(node);
        return RetType(node, prev, type);
    }

    size_t _size = 0;
    NodePtr _root = nullptr;
    NodePtr _first = nullptr;

    template<size_t requestedSize, size_t memStorageSize> struct _SelectSize {
        static const int val = memStorageSize * (1 + requestedSize / memStorageSize);
    };
    Region<_SelectSize<EXPECTED_MIN_NODE * sizeof(_Node), 2048>::val - (int)sizeof(void*)> _region;
};

} // namespace gepard

#endif // GEPARD_BINARY_TREES_H
