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

#ifndef GEPARD_BINARY_TREES_TESTS_H
#define GEPARD_BINARY_TREES_TESTS_H

#include "gepard-binary-trees.h"
#include "gepard-logging.h"
#include "gtest/gtest.h"
#include <chrono>
#include <functional>
#include <map>
#include <set>
#include <utility>
#include <vector>

namespace {

template<typename T>
const std::vector<T> fillVector(const int64_t count, const std::function<T()> constructor)
{
    std::vector<T> data(count);
    for (int64_t i = 0; i < count; ++i) {
        data[i] = constructor();
    }

    return data;
}

struct TestData {
    static int64_t s_uid;
    TestData(int64_t v = 0) : uid(++s_uid), value(v) {}
    int64_t uid;
    int64_t value;
    friend bool operator<(const TestData& lhs, const TestData& rhs)
    {
        return lhs.value < rhs.value;
    }
    friend bool operator<=(const TestData& lhs, const TestData& rhs)
    {
        return lhs.value <= rhs.value;
    }
    friend bool operator==(const TestData& lhs, const TestData& rhs)
    {
        return lhs.value == rhs.value;
    }
};
int64_t TestData::s_uid = 0;

std::ostream& operator<<(std::ostream& os, const TestData& obj)
{
    os << obj.value;
    return os;
}

struct {
    size_t count;
    std::function<TestData()> func;
} s_fillStrategys[] = {
    { 1, [](){ static int64_t counter = 0; return TestData(counter++); } },
    { 1, [](){ static int64_t counter = 10; return TestData(counter--); } },
    { 10, [](){ static int64_t counter = 0; return TestData(counter++); } },
    { 10, [](){ static int64_t counter = 10; return TestData(counter--); } },
    { 100, [](){ static int64_t counter = 0; return TestData(counter++); } },
    { 100, [](){ static int64_t counter = 100; return TestData(counter--); } },
    { 1 << 8, [](){ static int64_t counter = 1 << 8; return TestData(((counter % 2) ? -1 : 1) * (--counter)); } },
    { 100, [](){ return TestData(1); } },
    { 100, [](){ static int64_t counter = 0; return TestData(++counter % 5); } },
    { 1 << 20, [](){ static int64_t counter = (1 << 20) / 2; static int pwr = 1, times = 1;
        if (!times) {
                counter = counter / 2;
                times = pwr = pwr << 1;
            }
            return TestData(counter + 2 * counter * --times);
        } },
};

TEST(BinaryTree, CompareToSTDMultimap)
{
    for (size_t f = 0; f < sizeof(s_fillStrategys) / sizeof(s_fillStrategys[0]); ++f) {
        std::vector<TestData> data = fillVector(s_fillStrategys[f].count, s_fillStrategys[f].func);

        {
            std::multiset<TestData> refMultiSet;
            auto refStart = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < data.size(); ++i) {
                refMultiSet.emplace(data[i]);
            }
            auto refDuration = std::chrono::high_resolution_clock::now() - refStart;

            gepard::AVLTree<TestData> avlTree;
            auto avlStart = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < data.size(); ++i) {
                avlTree.multiInsert(data[i]);
            }
            auto avlDuration = std::chrono::high_resolution_clock::now() - avlStart;

            gepard::Set<TestData> multiSet;
            auto binaryStart = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < data.size(); ++i) {
                multiSet.multiInsert(data[i]);
            }
            auto binaryDuration = std::chrono::high_resolution_clock::now() - binaryStart;

            EXPECT_EQ(multiSet.size(), refMultiSet.size()) << f;
            EXPECT_EQ(avlTree.size(), refMultiSet.size()) << f;
            std::cout << "fillStrategy: " << f
                      << " count: " << s_fillStrategys[f].count
                      << " MultiSet: binaryDuration / refDuration = " << double(binaryDuration.count()) / double(refDuration.count())
                      << " AVL: avlDuration / refDuration = " << double(avlDuration.count()) / double(refDuration.count()) << std::endl;

            std::multiset<TestData>::iterator refIt = refMultiSet.begin();
            gepard::Set<TestData>::iterator avlIt = avlTree.begin(), avlPIt = avlTree.begin();
            gepard::Set<TestData>::iterator multiSetIt = multiSet.begin(), multiSetPIt = multiSet.begin();
            while (multiSetIt != multiSet.end() && avlIt != avlTree.end() && refIt != refMultiSet.end()) {
                EXPECT_EQ(avlIt->data, *refIt) << f;
                EXPECT_LE(avlPIt->data, avlIt->data) << f;

                EXPECT_EQ(multiSetIt->data, *refIt) << f;
                EXPECT_LE(multiSetPIt->data, multiSetIt->data) << f;

                ++refIt;
                avlPIt = avlIt++;
                multiSetPIt = multiSetIt++;
            }
        }

        {
            std::map<TestData, char> refMap;
            auto refStart = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < data.size(); ++i) {
                refMap.emplace(data[i], 0);
            }
            auto refDuration = std::chrono::high_resolution_clock::now() - refStart;

            gepard::AVLTree<TestData> avlTree;
            auto avlStart = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < data.size(); ++i) {
                avlTree.uniqueInsert(data[i]);
            }
            auto avlDuration = std::chrono::high_resolution_clock::now() - avlStart;

            gepard::Set<TestData> set;
            auto binaryStart = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < data.size(); ++i) {
                set.uniqueInsert(data[i]);
            }
            auto binaryDuration = std::chrono::high_resolution_clock::now() - binaryStart;

            gepard::_LinkedBinaryTree<TestData> linkedMap;
            auto linkedStart = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < data.size(); ++i) {
                linkedMap.uniqueInsert(data[i]);
            }
            auto linkedDuration = std::chrono::high_resolution_clock::now() - linkedStart;

            EXPECT_EQ(set.size(), refMap.size()) << f;
            EXPECT_EQ(avlTree.size(), refMap.size()) << f;
            EXPECT_EQ(linkedMap.size(), refMap.size()) << f;
            std::cout << "fillStrategy: " << f
                      << " count: " << s_fillStrategys[f].count
                      << " Set: binaryDuration / refDuration = " << double(binaryDuration.count()) / double(refDuration.count())
                      << " AVL: avlDuration / refDuration = " << double(avlDuration.count()) / double(refDuration.count())
                      << " Linked: linkedDuration / refDuration = " << double(linkedDuration.count()) / double(refDuration.count()) << std::endl;

            std::map<TestData, char>::iterator refIt = refMap.begin();
            gepard::Set<TestData>::iterator avlIt = avlTree.begin(), avlPIt = avlTree.begin();
            gepard::Set<TestData>::iterator setIt = set.begin(), setPIt = set.begin();
            gepard::_LinkedBinaryTree<TestData>::iterator linkedIt = linkedMap.begin(), linkedPIt = linkedMap.begin();
            while (refIt != refMap.end() && avlIt != avlTree.end() && setIt != set.end() && linkedIt != linkedMap.end()) {
                EXPECT_EQ(avlIt->data, refIt->first) << f;
                EXPECT_LE(avlPIt->data, avlIt->data) << f;

                EXPECT_EQ(setIt->data, refIt->first) << f;
                EXPECT_LE(setPIt->data, setIt->data) << f;

//                EXPECT_EQ(linkedIt->data, refIt->first) << f;
//                EXPECT_LE(linkedPIt->data, linkedIt->data) << f;

                ++refIt;
                avlPIt = avlIt++;
                setPIt = setIt++;
                linkedPIt = linkedIt++;
            }
        }
    }
}

} // anonymous namespace

#endif // GEPARD_BINARY_TREES_TESTS_H
