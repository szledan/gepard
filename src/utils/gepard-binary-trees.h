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
#include <cmath>
#include <iostream>
#include <iterator>
#include <memory>
#include <stack>
#include <vector>

namespace gepard {

template<typename _Tp>
class BinaryTree {
    struct Node {
        Node(_Tp d) : data(d) {}
        _Tp data;
        Node* left = nullptr;
        Node* right = nullptr;
        int height = 1;
    };

public:
    ~BinaryTree()
    {
        makeEmpty(_root);
    }

    const _Tp& insert(const _Tp& data)
    {
        insert(new Node(data));
        return data;
    }

    const size_t size() const { return _size; }
    const int height() { return _root ? h(_root) : 0; }

    class iterator : public std::iterator<std::forward_iterator_tag, Node> {
        std::stack<Node*> _nodes;
    public:
        iterator(Node* node = nullptr)
        {
            _nodes.push(nullptr);
            while (node) {
                _nodes.push(node);
                node = node->left;
            };
        }

        iterator operator++(int) /* postfix */
        {
            iterator it = *this;
            operator++();
            return it;
        }
        iterator& operator++() /* prefix */
        {
            if (_nodes.top()) {
                Node* node = _nodes.top()->right;
                _nodes.pop();
                while (node) {
                    _nodes.push(node);
                    node = node->left;
                };
            }
            return *this;
        }
        Node& operator*() const { return *(_nodes.top()); }
        Node* operator->() const { return _nodes.top(); }
        //! \todo: iterator operator+ (difference_type v)   const { return ???; }
        bool operator==(const iterator& rhs) const { return _nodes.top() == rhs._nodes.top(); }
        bool operator!=(const iterator& rhs) const { return _nodes.top() != rhs._nodes.top(); }
    };

    iterator begin() { return iterator(_root); }
    iterator end() { return iterator(nullptr); }

    void displayStat()
    {
        std::cout << std::endl;
        std::cout << "height: " << height() << std::endl;
        std::cout << "c_break: " << c_break << std::endl;
        std::cout << "c_leftRotate: " << c_leftRotate << std::endl;
        std::cout << "c_rightRotate: " << c_rightRotate << std::endl;
        std::cout << "c_leftRightRotate: " << c_leftRightRotate << std::endl;
        std::cout << "c_rightLeftRotate: " << c_rightLeftRotate << std::endl;
    }

    void displayTree()
    {
        for (auto& node : *this) {
            std::cout << std::string(2 * node.height, ' ') << node.data << "(" << node.height << ")" << std::endl;
        }
    }

private:
    void insert(Node* node)
    {
        _size++;

        if (!_root) {
            _root = node;
            return;
        }

        int balanceFactor = needBalancing(_root);
        if (balanceFactor) {
            _root = balance(_root, balanceFactor);
        }

        Node* currRoot = _root;
        do {
            const bool isLesser = node->data < currRoot->data;
            Node* workSide = isLesser ? currRoot->left : currRoot->right;
            if (!workSide) {
                c_break++;
                break;
            }
            Node* otherSide = isLesser ? currRoot->right : currRoot->left;

            if ((balanceFactor = needBalancing(workSide))) {
                workSide = balance(workSide, balanceFactor);
                if (isLesser) {
                    currRoot->left = workSide;
                } else {
                    currRoot->right = workSide;
                }
            }

            if (workSide->height >= currRoot->height) {
                currRoot->height = std::max(h(workSide), h(otherSide)) + 1;
            }

            currRoot = workSide;
        } while (currRoot);
        GD_ASSERT(currRoot);

        if (node->data < currRoot->data) {
            currRoot->left = node;
        } else {
            currRoot->right = node;
        }

        currRoot->height = std::max(h(currRoot->left), h(currRoot->right)) + 1;
    }

    Node* balance(Node* node, const int balanceFactor)
    {
        GD_ASSERT(balanceFactor);

        if (balanceFactor < 0) {
            if (!(node->left->right) || (h(node->left->right) < h(node->left->left))) {
                return rightRotate(node);
            }
            return leftRightRotate(node);
        }

        if (!(node->right->left) || (h(node->right->left) < h(node->right->right))) {
            return leftRotate(node);
        }
        return rightLeftRotate(node);
    }

    const int needBalancing(Node* node)
    {
        const int dh = h(node->right) - h(node->left);
        return dh >= 2 ? 1 : (dh <= -2 ? -1 : 0);
    }

    void makeEmpty(Node* node)
    {
        if(node == nullptr)
            return;
        makeEmpty(node->left);
        makeEmpty(node->right);
        delete node;
    }

    Node* leftRotate(Node* node)
    {
        c_leftRotate++;
        Node* child = node->right;
        node->right = child->left;
        child->left = node;

        node->height = std::max(h(node->left), h(node->right)) + 1;
        child->height = std::max(h(child->left), h(child->right)) + 1;
        return child;
    }

    Node* rightRotate(Node* node)
    {
        c_rightRotate++;
        Node* child = node->left;
        node->left = child->right;
        child->right = node;

        node->height = std::max(h(node->left), h(node->right)) + 1;
        child->height = std::max(h(child->left), h(child->right)) + 1;
        return child;
    }

    Node* leftRightRotate(Node* node)
    {
        c_leftRightRotate++;
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    Node* rightLeftRotate(Node* node)
    {
        c_rightLeftRotate++;
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    int h(const Node* node) const
    {
        return (node == nullptr ? -1 : node->height);
    }

    int c_break = 0, c_rightRotate = 0, c_leftRotate = 0, c_leftRightRotate = 0, c_rightLeftRotate = 0;

    Node* _root = nullptr;
    size_t _size = 0;
    size_t _height = 0;
};

} // namespace gepard

#endif // GEPARD_BINARY_TREES_H
