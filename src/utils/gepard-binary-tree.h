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

#ifndef GEPARD_BINARY_TREE_H
#define GEPARD_BINARY_TREE_H

#include "gepard-defs.h"
#include "gepard-region.h"
#include <cmath>
#include <iostream>
#include <iterator>
#include <stack>
#include <utility>
#include <vector>

namespace gepard {

template<typename T, const size_t EXPECTED_MIN_NODES = 64>
class BinaryTree {
    struct _Node {
        _Node(const _Node* n) : height(n->height) , data(n->data) {}
        _Node(const T& d) : data(d) {}
        _Node(T&& d) : data(std::forward<T>(d)) {}
        ~_Node() {}

        _Node* left = nullptr;
        _Node* right = nullptr;
        _Node* next = nullptr;
        int8_t height = 0;
        T data;
    };
    typedef _Node* _NodePtr;

    template<size_t requestedSize, size_t memStorageSize> struct _SelectSize {
        static const int val = memStorageSize * (1 + requestedSize / memStorageSize);
    };
    using _Region = Region<_SelectSize<EXPECTED_MIN_NODES * sizeof(_Node), 2048>::val - (int)sizeof(void*)>;

    struct _RetType {
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

    template<typename _Node>
    class _Iterator : public std::iterator<std::forward_iterator_tag, _Node> {
         _Node* _node;

    public:
        _Iterator(const _NodePtr node) : _node(node) {}

        _Iterator operator++(int)
        {
            iterator it = *this;
            operator++();
            return it;
        }
        _Iterator& operator++()
        {
            if (_node) {
                _node = _node->next;
            }
            return *this;
        }
        const T& operator*() const { GD_ASSERT(_node); return _node->data; }
        T& operator*() { GD_ASSERT(_node); return _node->data; }
        T* operator->() { GD_ASSERT(_node); return &(_node->data); }
        bool operator==(const _Iterator& rhs) const { return _node == rhs._node; }
        bool operator!=(const _Iterator& rhs) const { return _node != rhs._node; }
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

    BinaryTree(const size_t size, const _NodePtr root = nullptr, const _NodePtr first = nullptr, _Region&& region = _Region())
        : _size(size)
        , _region(std::move(region))
    {
        _NodePtr r = root;
        _root = r;
        _NodePtr f = first;
        _first = f;
    }

public:
    BinaryTree() = default;
    BinaryTree(const BinaryTree&) = delete;
    BinaryTree(BinaryTree&& bt)
        : _size(bt._size), _root(bt._root), _first(bt._first), _region(std::move(bt._region))
    {
        bt._first = nullptr;
    }
    BinaryTree& operator=(const BinaryTree&) = delete;
    BinaryTree& operator=(BinaryTree&& rhs)
    {
        if (this == &rhs)
            return *this;

        _size = rhs._size;
        _root = rhs._root;
        _first = rhs._first;
        _region = rhs._region;
        rhs._first = nullptr;
        return *this;
    }
    ~BinaryTree()
    {
        while (_first) {
            _NodePtr node = _first;
            node->~_Node();
            _first = _first->next;
        }
    }

    typedef _Iterator<_Node> iterator;

    struct ReturnType {
        ReturnType(iterator&& i, T* p, const bool in) : it(std::move(i)), prev(p), isNew(in) {}
        iterator it;
        T* prev;
        bool isNew;
    };

    iterator begin() { return iterator(_first); }
    iterator end() { return iterator(nullptr); }

    const size_t size() const { return _size; }
    const int8_t rootHeight() const { return height(_root); }

    ReturnType uniqueInsert(const T& data) { return insert<_Greater<T>>(data); }
    ReturnType uniqueInsert(T&& data) { return insert<_Greater<T>>(std::move(data)); }

    ReturnType multiInsert(const T& data) { return insert<_GreaterOrEqual<T>>(data); }
    ReturnType multiInsert(T&& data) { return insert<_GreaterOrEqual<T>>(std::move(data)); }

    ReturnType find(const T& data)
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
        return ReturnType(iterator(node), prev ? &(prev->data) : nullptr, false);
    }

    BinaryTree clone()
    {
        if (!_root)
            return BinaryTree();

        GD_ASSERT(_root);
        if (_size < (1 << 3)) {
            BinaryTree bt;
            _NodePtr n = _first;
            do {
                bt.multiInsert(n->data);
            } while ((n = n->next));
            return bt;
        }

        int8_t sp = 2 * rootHeight() + 1;
        _NodePtr root = nullptr;
        _NodePtr first = nullptr;
        size_t size = 0;
        _Region region;

        std::vector<_NodePtr> srcStack(sp);
        std::vector<_NodePtr> dstStack(sp);
        std::vector<_NodePtr> prevStack(sp);
        auto cloneNode = [&](_NodePtr& target, _NodePtr node, _NodePtr prev) -> _NodePtr {
            size++;
            dstStack[sp] = target = new (region.alloc(sizeof(_Node))) _Node(node);
            prevStack[sp] = prev;
            srcStack[sp++] = node;
            return target;
        };

        sp = 0;
        first = cloneNode(root, _root, nullptr);

        while (sp) {
            _NodePtr currOld = srcStack[--sp];
            _NodePtr currNew = dstStack[sp];
            _NodePtr prevNode = prevStack[sp];
            GD_ASSERT(currNew);
            if (currOld->right) {
                cloneNode(currNew->right, currOld->right, currNew);
                dstStack[sp - 1]->next = currNew->next;
                currNew->next = dstStack[sp - 1];
            }
            if (currOld->left) {
                cloneNode(currNew->left, currOld->left, nullptr);
                dstStack[sp - 1]->next = currNew;

                if (prevNode) {
                    prevStack[sp - 1] = prevNode;
                    prevNode->next = dstStack[sp - 1];
                } else if (currNew->left->data <= first->data) {
                    first = currNew->left;
                }
            }
        }

        return BinaryTree(size, root, first, std::move(region));
    }

    //! \todo: remove:just testing
    void displayTree() const
    {
        if (!_root)
            return;
        std::cout << "{" << _root << ":" << _root->data << ", " << _first << ":" << _first->data << "}" << std::endl;
        displayTree(_root);
    }

private:
    //! \todo: remove:just testing
    void displayTree(_NodePtr node) const
    {
        if (!node)
            return;
        displayTree(node->left);
        std::cout << std::string(2 * node->height, ' ') << node->data << " (" << (int)height(node->left) << "," << (int)height(node->right) << ") " << node << "->[" << node->next << "]" << std::endl;
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

    template<typename _NotLessCompare, typename _U>
    ReturnType insert(_U&& data)
    {
        _NotLessCompare notLess;
        _NodePtr node = nullptr;
        _NodePtr prev = nullptr;
        bool isNew = true;

        if (GD_UNLIKELY(!_root)) {
            node = _first = _root = put<_U>(_root, std::forward<_U>(data));
        } else {
            _NodePtr workSide, parent = _root = balance(_root);
            prev = parent;
            do {
                const bool isLesser = data < parent->data;

                if (isLesser) {
                    workSide = parent->left;
                } else if (notLess(data, parent->data)) {
                    prev = parent;
                    workSide = parent->right;
                } else {
                    isNew = false;
                    prev = findPrevNode(parent, prev);
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
                node = putLeft<_U>(parent, prev, std::forward<_U>(data));
            } else if (notLess(data, parent->data)) {
                node = putRight<_U>(parent, std::forward<_U>(data));
            } else {
                node = parent;
            }

            fixHeight(parent);
        }

        GD_ASSERT(node);
        return ReturnType(iterator(node), prev ? &(prev->data) : nullptr, isNew);
    }

    template<typename _NotLessCompare, typename _U>
    void emplace(_U&& data)
    {
        insert<_NotLessCompare, _U, _RetType>(std::forward<_U>(data));
    }

    size_t _size = 0;
    _NodePtr _root = nullptr;
    _NodePtr _first = nullptr;
    _Region _region;
};

} // namespace gepard

#endif // GEPARD_BINARY_TREE_H
