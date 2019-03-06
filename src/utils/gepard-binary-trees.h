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
#include <utility>

namespace gepard {

template<typename _Tp, const size_t EXPECTED_MIN_NODES = 64>
class _LinkedBinaryTree;

template<typename T>
class Set {
    using LinkedBinaryTree = _LinkedBinaryTree<T>;

public:
    typedef typename LinkedBinaryTree::iterator iterator;

    iterator begin() { return _lbt.begin(); }
    iterator end() { return _lbt.end(); }

    const size_t size() const  { return _lbt.size(); }

    iterator find(const T& value) { return _lbt.find(value); }

    iterator insert(const T& value) { return _lbt.uniqueInsert(value); }
    iterator insert(T&& value) { return _lbt.uniqueInsert(value); }
    void emplace(const T& value) { _lbt.uniqueEmplace(value); }
    void emplace(T&& value) { _lbt.uniqueEmplace(value); }

private:
    LinkedBinaryTree _lbt;
};

template<typename T>
class MultiSet {
    using LinkedBinaryTree = _LinkedBinaryTree<T>;

public:
    typedef typename LinkedBinaryTree::iterator iterator;

    iterator begin() { return _lbt.begin(); }
    iterator end() { return _lbt.end(); }

    const size_t size() const  { return _lbt.size(); }

    iterator find(const T& value) { return _lbt.find(value); }

    iterator insert(const T& value) { return _lbt.multiInsert(value); }
    iterator insert(T&& value) { return _lbt.multiInsert(value); }
    void emplace(const T& value) { _lbt.multiEmplace(value); }
    void emplace(T&& value) { _lbt.multiEmplace(value); }

private:
    LinkedBinaryTree _lbt;
};

template<typename _T, const size_t EXPECTED_MIN_NODES>
class _LinkedBinaryTree {
    struct _Node {
        _Node(const _T& d) : data(d) {}
        _Node(_T&& d) : data(std::forward<_T>(d)) {}

        _Node* left = nullptr;
        _Node* right = nullptr;
        _Node* next = nullptr;
        int8_t height = 0;
        _T data;
    };
    typedef _Node* _NodePtr;

    struct _EmptyRetType {
        inline void setPrev(const _NodePtr) {}
        inline void setType(const bool) {}
        inline void findPrev(const _NodePtr) {}
        inline const _NodePtr getPrev() const { return nullptr; }
        inline const bool getType() const { return false; }
    };

    struct _RetType : public _EmptyRetType {
        _NodePtr prevNode;
        bool type;
        _RetType(const _NodePtr p = nullptr, const bool t = false)
            : prevNode(p), type(t)
        {}

        inline void setPrev(const _NodePtr p) { prevNode = p; }
        inline void setType(const bool t) { type = t; }
        inline void findPrev(const _NodePtr node) { prevNode = findPrevNode(node, prevNode); }
        inline const _NodePtr getPrev() const { return prevNode; }
        inline const bool getType() const { return type; }
    };

    template<typename _RetT>
    class _Iterator : public std::iterator<std::forward_iterator_tag, _RetT> {
         _RetT _retVal;

    public:
        _Iterator(const _RetT& rv) : _retVal(rv) {}

        const _RetT& retVal() const { return _retVal; }

        _Iterator operator++(int)
        {
            iterator it = *this;
            operator++();
            return it;
        }
        _Iterator& operator++()
        {
            ++_retVal;
            return *this;
        }
        _RetT& operator*() { return _retVal; }
        _RetT* operator->() { return &_retVal; }
        bool operator==(const _Iterator& rhs) const { return _retVal == rhs._retVal; }
        bool operator!=(const _Iterator& rhs) const { return !(_retVal == rhs._retVal); }
    };

    template<typename _U>
    struct _Greater {
        constexpr bool operator()(const _U& lhs, const _U& rhs) const
        {
            return rhs < lhs;
        }
    };

    template<typename _U>
    struct _GreaterOrEqual {
        constexpr bool operator()(const _U& lhs, const _U& rhs) const
        {
            return !(lhs < rhs);
        }
    };

public:
    struct ReturnType {
        _T* data;
        _T* prev;
        bool isNew;

        ReturnType(const _NodePtr n = nullptr, const _NodePtr p = nullptr, const bool t = false)
            : data(n ? &(n->data) : nullptr)
            , prev(p ? &(p->data) : nullptr)
            , isNew(t)
            , _node(n)
        {}

        bool operator==(const ReturnType& rhs) const { return _node == rhs._node; }
        ReturnType& operator++()
        {
            isNew = false;
            _node = _node->next;
            if (_node) {
                prev = data;
                data = &(_node->data);
            }
            return *this;
        }
    private:
        _NodePtr _node;
    };
    typedef _Iterator<ReturnType> iterator;

    iterator begin() { return iterator(ReturnType(_first)); }
    iterator end() { return iterator(ReturnType()); }

    const size_t size() const { return _size; }
    const int8_t rootHeight() const { return height(_root); }

    iterator uniqueInsert(const _T& data) { return iterator(insert<_Greater<_T>>(data)); }
    iterator uniqueInsert(_T&& data) { return iterator(insert<_Greater<_T>>(std::move(data))); }
    void uniqueEmplace(const _T& data) { emplace<_Greater<_T>>(data); }
    void uniqueEmplace(_T&& data) { emplace<_Greater<_T>>(std::move(data)); }

    iterator multiInsert(const _T& data) { return iterator(insert<_GreaterOrEqual<_T>>(data)); }
    iterator multiInsert(_T&& data) { return iterator(insert<_GreaterOrEqual<_T>>(std::move(data))); }
    void multiEmplace(const _T& data) { emplace<_GreaterOrEqual<_T>>(data); }
    void multiEmplace(_T&& data) { emplace<_GreaterOrEqual<_T>>(std::move(data)); }

    iterator find(const _T& data)
    {
        _NodePtr prev, node = prev = _root;
        while (node) {
            if (data < node->data) {
                node = node->left;
            } else if (node->data < data) {
                prev = node;
                node = node->right;
            } else {
                prev = findPrevNode(node, prev);
                break;
            }
        }
        return iterator(ReturnType(node, prev));
    }

    //! \todo: remove:just testing
    void displayTree()
    {
        displayTree(_root);
    }

private:
    //! \todo: remove:just testing
    void displayTree(_NodePtr node)
    {
        if (!node)
            return;
        displayTree(node->left);
        std::cout << std::string(2 * node->height, ' ') << node->data << " (" << (int)height(node->left) << "," << (int)height(node->right) << ")" << std::endl;
        displayTree(node->right);
    }

    const int8_t height(const _NodePtr node) const { return node ? node->height : -1; }

    const int8_t balanceFactor(const _NodePtr node) const
    {
        GD_ASSERT(node);
        return height(node->right) - height(node->left);
    }

    void fixHeight(_NodePtr node)
    {
        GD_ASSERT(node);
        node->height = std::max(height(node->left), height(node->right)) + 1;
    }

    _NodePtr leftRotate(_NodePtr node)
    {
        _NodePtr child = node->right;
        node->right = child->left;
        child->left = node;

        node->height = std::max(height(node->left), height(node->right)) + 1;
        child->height = std::max(height(child->left), height(child->right)) + 1;
        return child;
    }

    _NodePtr rightRotate(_NodePtr node)
    {
        _NodePtr child = node->left;
        node->left = child->right;
        child->right = node;

        node->height = std::max(height(node->left), height(node->right)) + 1;
        child->height = std::max(height(child->left), height(child->right)) + 1;
        return child;
    }

    _NodePtr balance(_NodePtr node)
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

    static const _NodePtr findPrevNode(const _NodePtr node, const _NodePtr predictPrev)
    {
        _NodePtr prev = predictPrev;
        if (!prev || prev->next != node) {
            prev = node->left;
            if (prev) {
                while (prev->right) {
                    prev = prev->right;
                }
                GD_ASSERT(prev->next == node);
            }
        }
        return prev;
    }

    template<typename _U>
    _NodePtr put(_NodePtr& node, _U&& data)
    {
        GD_ASSERT(!node);
        _size++;
        return node = new (_region.alloc(sizeof(_Node))) _Node(std::forward<_U>(data));
    }

    template<typename _U>
    _NodePtr putLeft(_NodePtr parent, _NodePtr prev, _U&& data)
    {
        GD_ASSERT(parent);
        _NodePtr node = put<_U>(parent->left, std::forward<_U>(data));
        node->next = parent;
        if (GD_UNLIKELY(_first == parent)) {
            _first = node;
        } else if (prev) {
            GD_ASSERT(prev->next == parent);
            prev->next = node;
        }
        return node;
    }

    template<typename _U>
    _NodePtr putRight(_NodePtr parent, _U&& data)
    {
        GD_ASSERT(parent);
        _NodePtr node = put<_U>(parent->right, std::forward<_U>(data));
        node->next = parent->next;
        parent->next = node;
        return node;
    }

    template<typename _NotLessCompare, typename _U, typename _RetVal = _RetType>
    ReturnType insert(_U&& data)
    {
        _NotLessCompare notLess;
        _NodePtr node = nullptr;
        _RetVal retVal;
        retVal.setType(true);

        if (GD_UNLIKELY(!_root)) {
            node = _first = _root = put<_U>(_root, std::forward<_U>(data));
        } else {
            _NodePtr workSide, parent = _root = balance(_root);
            retVal.setPrev(parent);
            do {
                const bool isLesser = data < parent->data;

                if (isLesser) {
                    workSide = parent->left;
                } else if (notLess(data, parent->data)) {
                    retVal.setPrev(parent);
                    workSide = parent->right;
                } else {
                    retVal.setType(false);
                    retVal.findPrev(parent);
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
                node = putLeft<_U>(parent, retVal.getPrev(), std::forward<_U>(data));
            } else if (notLess(data, parent->data)) {
                node = putRight<_U>(parent, std::forward<_U>(data));
            } else {
                node = parent;
            }

            fixHeight(parent);
        }

        GD_ASSERT(node);
        return ReturnType(node, retVal.getPrev(), retVal.getType() > 0);
    }

    template<typename _NotLessCompare, typename _U>
    void emplace(_U&& data)
    {
        insert<_NotLessCompare, _U, _EmptyRetType>(std::forward<_U>(data));
    }

    size_t _size = 0;
    _NodePtr _root = nullptr;
    _NodePtr _first = nullptr;

    template<size_t requestedSize, size_t memStorageSize> struct _SelectSize {
        static const int val = memStorageSize * (1 + requestedSize / memStorageSize);
    };
    Region<_SelectSize<EXPECTED_MIN_NODES * sizeof(_Node), 2048>::val - (int)sizeof(void*)> _region;
};

} // namespace gepard

#endif // GEPARD_BINARY_TREES_H
