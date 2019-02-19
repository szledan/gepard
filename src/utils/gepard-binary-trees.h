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
#include <atomic>
#include <cmath>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <stack>
#include <vector>

namespace gepard {

template<typename _Tp>
struct Node;
template<typename _Node>
class _Iterator;
template<typename _Tp, typename _Node>
class _AVLTree;
template<typename _Tp, typename Node>
class _BinaryTree;
template<typename _Tp>
class _LinkedBinaryTree;

template<typename _Tp> using AVLTree = _AVLTree<_Tp, Node<_Tp>>;
template<typename _Tp> using Set = _BinaryTree<_Tp, Node<_Tp>>;
template<typename _Tp> using CSet = _BinaryTree<_Tp, Node<_Tp>>;

template<typename _Tp>
struct Node {
    using NodePtr = Node*;

    Node(_Tp d = _Tp()) : data(d) {}
    _Tp data;

    NodePtr left = nullptr;
    NodePtr right = nullptr;
    int height = 0;
};

template<typename _NodePtr>
inline const int height(const _NodePtr node)
{
    return node ? node->height : -1;
}

template<typename _NodePtr>
inline const int balanceFactor(const _NodePtr node)
{
    GD_ASSERT(node);
    return height(node->right) - height(node->left);
}

template<typename _NodePtr>
inline void fixHeight(_NodePtr node)
{
    GD_ASSERT(node);
    node->height = std::max(height(node->left), height(node->right)) + 1;
}

template<typename _Node>
class _Iterator : public std::iterator<std::forward_iterator_tag, _Node> {
    using NodePtr = _Node*;
    std::stack<NodePtr> _nodes;

public:
    _Iterator(const NodePtr n)
    {
        NodePtr node = n;
        _nodes.push(nullptr);
        while (node) {
            _nodes.push(node);
            node = node->left;
        };
    }

    _Iterator operator++(int)
    {
        _Iterator it = *this;
        operator++();
        return it;
    }
    _Iterator& operator++()
    {
        if (_nodes.top()) {
            NodePtr node = _nodes.top()->right;
            _nodes.pop();
            while (node) {
                _nodes.push(node);
                node = node->left;
            };
        }
        return *this;
    }
    _Node& operator*() const { return *(_nodes.top()); }
    NodePtr operator->() const { return _nodes.top(); }
    bool operator==(const _Iterator& rhs) const { return _nodes.top() == rhs._nodes.top(); }
    bool operator!=(const _Iterator& rhs) const { return _nodes.top() != rhs._nodes.top(); }
};

//template<typename _iterator>
//void displayTree(const _iterator it)
//{
//    while (it) {
//        std::cout << std::string(2 * node.height, ' ') << node.data << "(" << node.height << ")" << std::endl;
//    }
//}

template<typename _Tp, typename _Node>
class _AVLTree {
    using NodePtr = _Node*;
    NodePtr _root = nullptr;
    size_t _size = 0;
    Region<((8 * 1024) - (int)sizeof(void*))> _region;

public:
    typedef _Iterator<_Node> iterator;
    iterator begin() { return iterator(_root); }
    iterator end() { return iterator(nullptr); }

    const size_t size() const { return _size; }

    void uniqueInsert(const _Tp& data)
    {
        _root = uniqueInsert(_root, data);
    }

    void multiInsert(const _Tp& data)
    {
        _root = multiInsert(_root, data);
    }

private:
    NodePtr rightRotate(NodePtr node)
    {
        NodePtr tmp = node->left;
        node->left = tmp->right;
        tmp->right = node;

        fixHeight(node);
        fixHeight(tmp);

        return tmp;
    }

    NodePtr leftRotate(NodePtr node)
    {
        NodePtr tmp = node->right;
        node->right = tmp->left;
        tmp->left = node;

        fixHeight(node);
        fixHeight(tmp);

        return tmp;
    }

    NodePtr balance(NodePtr node)
    {
        fixHeight(node);
        const int bFactor = balanceFactor(node);

        if (bFactor == 2) {
            if (balanceFactor(node->right) < 0) {
                node->right = rightRotate(node->right);
            }
            return leftRotate(node);
        }

        if (bFactor == -2) {
            if (balanceFactor(node->left) > 0) {
                node->left = leftRotate(node->left);
            }
            return rightRotate(node);
        }

        return node;
    }

    NodePtr put(const _Tp& data)
    {
        _size++;
        return new (_region.alloc(sizeof(_Node))) _Node(data);
    }

    NodePtr uniqueInsert(NodePtr node, const _Tp& data)
    {
        if (!node) {
            return put(data);
        }

        if (data < node->data) {
            node->left = uniqueInsert(node->left, data);
        } else if (node->data < data) {
            node->right = uniqueInsert(node->right, data);
        }

        return balance(node);
    }

    NodePtr multiInsert(NodePtr node, const _Tp& data)
    {
        if (!node) {
            return put(data);
        }

        if (data < node->data) {
            node->left = multiInsert(node->left, data);
        } else {
            node->right = multiInsert(node->right, data);
        }

        return balance(node);
    }
};

template<typename _Tp, typename _Node>
class _BinaryTree {
public:
    typedef _Node* NodePtr;

    typedef _Iterator<_Node> iterator;
    iterator begin() { return iterator(_root); }
    iterator end() { return iterator(nullptr); }

    const size_t size() const { return _size; }

    NodePtr* uniqueFindOrInsert(const _Tp& data)
    {
//        if ((*np) == nullptr) {
////            std::cerr << key << " NULL\n";
//            return _bt.put(*np, { key, _Tp() })->data.value;
//        } else {
////            std::cerr << key << " FULL\n";
//            return (*np)->data.value;
//        }
        return &_root;
    }

    _Tp& uniqueInsert(const _Tp& data)
    {
        NodePtr node = nullptr;

        if (!_root) {
            node = put(_root, data);
        } else {
            NodePtr parent = findParentOrSame(data);
            GD_ASSERT(parent);

            if (data < parent->data) {
                node = put(parent->left, data);
            } else if (parent->data < data) {
                node = put(parent->right, data);
            } else {
                return parent->data;
            }

            parent->height = std::max(height(parent->left), height(parent->right)) + 1;
        }

        GD_ASSERT(node);
        return node->data;
    }

    const _Tp& multiInsert(const _Tp& data)
    {
        NodePtr node = nullptr;

        if (!_root) {
            node = put(_root, data);
        } else {
            NodePtr parent = findParent(data);
            GD_ASSERT(parent);

            if (data < parent->data) {
                node = put(parent->left, data);
            } else {
                node = put(parent->right, data);
            }

            parent->height = std::max(height(parent->left), height(parent->right)) + 1;
        };

        GD_ASSERT(node);
        return node->data;
    }

    NodePtr put(NodePtr& node, const _Tp& data)
    {
        GD_ASSERT(!node);
        _size++;
        return node = new (_region.alloc(sizeof(_Node))) _Node(data);
    }

    void displayTree()
    {
        for (auto& it : *this) {
            std::cout << std::string(2 * it.height, ' ') << it.data.value << "(" << it.height << ")" << std::endl;
        }
    }

private:
    NodePtr findParent(const _Tp& data)
    {
        GD_ASSERT(_root);

        NodePtr parent = _root = balance(_root);
        do {
            const bool isLesser = data < parent->data;
            NodePtr workSide = isLesser ? parent->left : parent->right;
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

        return parent;
    }

    NodePtr findParentOrSame(const _Tp& data)
    {
        GD_ASSERT(_root);

        NodePtr parent = _root = balance(_root);
        do {
            const bool isLesser = data < parent->data;
            const bool isGreater = parent->data < data;
            if (!isLesser && !isGreater) {
                break;
            }

            NodePtr workSide = isLesser ? parent->left : parent->right;
            if (!workSide) {
                break;
            }

            workSide = balance(workSide);

            if (isLesser) {
                parent->left = workSide;
            } else {
                GD_ASSERT(isGreater);
                parent->right = workSide;
            }

            if (workSide->height >= parent->height) {
                fixHeight(parent);
            }

            parent = workSide;
        } while (parent);
        GD_ASSERT(parent);

        return parent;
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

    std::atomic<size_t> _size = { 0 };
    NodePtr _root = nullptr;
    NodePtr _santinel = nullptr;

    Region<((8 * 1024) - (int)sizeof(void*))> _region;
};

template<typename _Tp>
class _LinkedBinaryTree {
    struct _Node {
        _Node(_Tp d = _Tp()) : data(d) {}

        _Tp data;
        _Node* left = nullptr;
        _Node* right = nullptr;
        _Node* next = nullptr;
        int height = 0;
    };
    using NodePtr = _Node*;

public:
    class iterator : public std::iterator<std::forward_iterator_tag, _Node> {
        using NodePtr = _Node*;
        NodePtr _node;

    public:
        iterator(const NodePtr n) { _node = n; }

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

    _Tp& uniqueInsert(const _Tp& data)
    {
        NodePtr node = nullptr;

        if (!_root) {
            node = _first = _root = put(_root, data);
        } else {
            NodePtr parent = findParent(data);
            GD_ASSERT(parent);

            if (data < parent->data) {
                node = putLeft(parent, data);
            } else if (parent->data < data) {
                node = putRight(parent, data);
            } else {
                return parent->data;
            }

            fixHeight(parent);
        }

        GD_ASSERT(node);
        return node->data;
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

    NodePtr findParent(const _Tp& data)
    {
        GD_ASSERT(_root);

        NodePtr workSide, parent = _prev = _root = balance(_root);
        do {
            const bool isLesser = data < parent->data;
            const bool isGreater = parent->data < data;
            if (!isLesser && !isGreater) {
                break;
            }

            if (isLesser) {
                workSide = parent->left;
            } else {
                _prev = parent;
                workSide = parent->right;
            }

            if (!workSide) {
                break;
            }
            workSide = balance(workSide);

            if (isLesser) {
                parent->left = workSide;
            } else {
                GD_ASSERT(isGreater);
                parent->right = workSide;
            }

            if (workSide->height >= parent->height) {
                fixHeight(parent);
            }

            parent = workSide;
        } while (parent);
        GD_ASSERT(parent);

        return parent;
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

    size_t _size = 0;
    NodePtr _root = nullptr;
    NodePtr _prev = nullptr;
    NodePtr _first = nullptr;

    Region<((8 * 1024) - (int)sizeof(void*))> _region;
};

} // namespace gepard

#endif // GEPARD_BINARY_TREES_H
