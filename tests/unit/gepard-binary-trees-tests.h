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

namespace binaryTreesTests {

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
    static int64_t s_callConstructor;
    static int64_t s_callCopyAssignmentOp;
    static int64_t s_callCopyConstructor;
    static int64_t s_callDestructor;
    static int64_t s_callMoveAssignmentOp;
    static int64_t s_callMoveConstructor;
    static int64_t s_uid;
    int64_t uid;
    int64_t value;

    static void resetCounters()
    {
        TestData::s_callConstructor = 0;
        TestData::s_callCopyAssignmentOp = 0;
        TestData::s_callCopyConstructor = 0;
        TestData::s_callDestructor = 0;
        TestData::s_callMoveAssignmentOp = 0;
        TestData::s_callMoveConstructor = 0;
        TestData::s_uid = 0;
    }

    TestData(const int64_t v = 0) : uid(getUId()), value(v) { s_callConstructor++; }
    TestData(const TestData& other) : uid(getUId()), value(other.value) { s_callCopyConstructor++; }
    TestData(TestData&& other) : uid(std::move(other.uid)), value(std::move(other.value)) { s_callMoveConstructor++; }
    ~TestData() { s_callDestructor++; }

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

    static const bool stricktPreOrder(const TestData& lhs, const TestData& rhs)
    {
        return lhs.value < rhs.value || (lhs.value == rhs.value && lhs.uid < rhs.uid);
    }

    TestData& operator=(TestData&& other)
    {
        s_callMoveAssignmentOp++;
        this->uid = std::move(other.uid);
        this->value = std::move(other.value);
        return *this;
    }

    TestData& operator=(TestData& other)
    {
        s_callCopyAssignmentOp++;
        this->uid = getUId();
        this->value = other.value;
        return *this;
    }

    const TestData& operator=(const TestData& other)
    {
        s_callCopyAssignmentOp++;
        this->value = other.value;
        this->uid = getUId();
        return *this;
    }

private:
    const int64_t getUId() { return ++s_uid; }
};
int64_t TestData::s_callConstructor = 0;
int64_t TestData::s_callCopyAssignmentOp = 0;
int64_t TestData::s_callCopyConstructor = 0;
int64_t TestData::s_callDestructor = 0;
int64_t TestData::s_callMoveAssignmentOp = 0;
int64_t TestData::s_callMoveConstructor = 0;
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

TEST(_LinkedBinaryTree, HeightAndSize)
{
    gepard::_LinkedBinaryTree<int> linkedBinaryTree;

    EXPECT_EQ(linkedBinaryTree.size(), 0u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), -1);
    linkedBinaryTree.uniqueInsert(9);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 0);
    linkedBinaryTree.uniqueInsert(9);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 0);
    linkedBinaryTree.uniqueInsert(8);
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 1);
    linkedBinaryTree.uniqueInsert(8);
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 1);
    linkedBinaryTree.uniqueInsert(7);
    EXPECT_EQ(linkedBinaryTree.size(), 3u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 1);
    linkedBinaryTree.uniqueInsert(6);
    EXPECT_EQ(linkedBinaryTree.size(), 4u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 1);
    linkedBinaryTree.uniqueInsert(5);
    EXPECT_EQ(linkedBinaryTree.size(), 5u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 2);
    linkedBinaryTree.uniqueInsert(4);
    EXPECT_EQ(linkedBinaryTree.size(), 6u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 2);
    linkedBinaryTree.uniqueInsert(3);
    EXPECT_EQ(linkedBinaryTree.size(), 7u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 2);
    linkedBinaryTree.uniqueInsert(2);
    EXPECT_EQ(linkedBinaryTree.size(), 8u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 2);
    linkedBinaryTree.uniqueInsert(1);
    EXPECT_EQ(linkedBinaryTree.size(), 9u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 2);
    linkedBinaryTree.uniqueInsert(1);
    EXPECT_EQ(linkedBinaryTree.size(), 9u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 3);
    linkedBinaryTree.uniqueInsert(0);
    EXPECT_EQ(linkedBinaryTree.size(), 10u);
    EXPECT_EQ(linkedBinaryTree.rootHeight(), 3);
}

TEST(_LinkedBinaryTree, Iterator)
{
    gepard::_LinkedBinaryTree<int> uniqueLinkedBinaryTree;
    gepard::_LinkedBinaryTree<int> multiLinkedBinaryTree;

    EXPECT_EQ(uniqueLinkedBinaryTree.begin(), uniqueLinkedBinaryTree.end());
    EXPECT_EQ(multiLinkedBinaryTree.begin(), multiLinkedBinaryTree.end());

    for (int i = 0; i < 30; ++i) {
        uniqueLinkedBinaryTree.uniqueInsert(i / 3);
        multiLinkedBinaryTree.multiInsert(i / 3);
    }

    EXPECT_EQ(uniqueLinkedBinaryTree.size(), 10u);
    EXPECT_EQ(multiLinkedBinaryTree.size(), 30u);

    gepard::_LinkedBinaryTree<int>::iterator uItPrev, uIt = uniqueLinkedBinaryTree.begin();
    uItPrev = uIt++;
    while (uIt != uniqueLinkedBinaryTree.end()) {
        EXPECT_LT(uItPrev->data, uIt->data);
        uItPrev = uIt++;
    }
    gepard::_LinkedBinaryTree<int>::iterator mItPrev, mIt = multiLinkedBinaryTree.begin();
    mItPrev = mIt++;
    while (mIt != uniqueLinkedBinaryTree.end()) {
        EXPECT_LE(mItPrev->data, mIt->data);
        EXPECT_LE(TestData::stricktPreOrder(mItPrev->data, mIt->data), true);
        mItPrev = mIt++;
    }
}
TEST(_LinkedBinaryTree, UniqueInsert)
{
    TestData::resetCounters();
    TestData a(1);
    EXPECT_EQ(TestData::s_callConstructor, 1);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 0);
    EXPECT_EQ(TestData::s_callDestructor, 0);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);

    gepard::_LinkedBinaryTree<TestData> linkedBinaryTree;

    linkedBinaryTree.uniqueInsert(a);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    EXPECT_EQ(TestData::s_callConstructor, 1);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 1);
    EXPECT_EQ(TestData::s_callDestructor, 0);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);

    linkedBinaryTree.uniqueInsert(a);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    EXPECT_EQ(TestData::s_callConstructor, 1);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 1);
    EXPECT_EQ(TestData::s_callDestructor, 0);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);

    linkedBinaryTree.uniqueInsert(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    EXPECT_EQ(TestData::s_callConstructor, 2);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 2);
    EXPECT_EQ(TestData::s_callDestructor, 1);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);

    linkedBinaryTree.uniqueInsert(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    EXPECT_EQ(TestData::s_callConstructor, 3);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 2);
    EXPECT_EQ(TestData::s_callDestructor, 2);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);

    linkedBinaryTree.uniqueInsert(TestData(3));
    EXPECT_EQ(linkedBinaryTree.size(), 3u);
    EXPECT_EQ(TestData::s_callConstructor, 4);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 3);
    EXPECT_EQ(TestData::s_callDestructor, 3);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);
}

TEST(_LinkedBinaryTree, MultiInsert)
{
    TestData::resetCounters();
    TestData a(1);
    EXPECT_EQ(TestData::s_callConstructor, 1);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 0);
    EXPECT_EQ(TestData::s_callDestructor, 0);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);

    gepard::_LinkedBinaryTree<TestData> linkedBinaryTree;

    linkedBinaryTree.multiInsert(a);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    EXPECT_EQ(TestData::s_callConstructor, 1);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 1);
    EXPECT_EQ(TestData::s_callDestructor, 0);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);

    linkedBinaryTree.multiInsert(a);
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    EXPECT_EQ(TestData::s_callConstructor, 1);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 2);
    EXPECT_EQ(TestData::s_callDestructor, 0);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);

    linkedBinaryTree.multiInsert(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 3u);
    EXPECT_EQ(TestData::s_callConstructor, 2);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 3);
    EXPECT_EQ(TestData::s_callDestructor, 1);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);

    linkedBinaryTree.multiInsert(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 4u);
    EXPECT_EQ(TestData::s_callConstructor, 3);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 4);
    EXPECT_EQ(TestData::s_callDestructor, 2);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);

    linkedBinaryTree.multiInsert(TestData(3));
    EXPECT_EQ(linkedBinaryTree.size(), 5u);
    EXPECT_EQ(TestData::s_callConstructor, 4);
    EXPECT_EQ(TestData::s_callCopyAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callCopyConstructor, 5);
    EXPECT_EQ(TestData::s_callDestructor, 3);
    EXPECT_EQ(TestData::s_callMoveAssignmentOp, 0);
    EXPECT_EQ(TestData::s_callMoveConstructor, 0);
}

TEST(BinaryTree, DISABLED_CompareToSTDMultimap)
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

            gepard::MultiSet<TestData> multiSet;
            auto binaryStart = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < data.size(); ++i) {
                multiSet.insert(data[i]);
            }
            auto binaryDuration = std::chrono::high_resolution_clock::now() - binaryStart;

            EXPECT_EQ(multiSet.size(), refMultiSet.size()) << f;
            std::cout << "fillStrategy: " << f
                      << " count: " << s_fillStrategys[f].count
                      << " MultiSet: binaryDuration / refDuration = " << double(binaryDuration.count()) / double(refDuration.count()) << std::endl;

            std::multiset<TestData>::iterator refIt = refMultiSet.begin();
            gepard::MultiSet<TestData>::iterator multiSetIt = multiSet.begin(), multiSetPIt = multiSet.begin();
            while (multiSetIt != multiSet.end() && refIt != refMultiSet.end()) {
                EXPECT_EQ(*multiSetIt, *refIt) << f;
                EXPECT_LE(*multiSetPIt, *multiSetIt) << f;

                ++refIt;
                multiSetPIt = multiSetIt++;
            }
        }

        {
            auto refStart = std::chrono::high_resolution_clock::now();
            std::map<TestData, char> refMap;
            for (size_t i = 0; i < data.size(); ++i) {
                refMap[data[i]] = 0;
            }
            auto refDuration = std::chrono::high_resolution_clock::now() - refStart;

            auto mapStart = std::chrono::high_resolution_clock::now();
            gepard::Map<TestData, char> map;
            for (size_t i = 0; i < data.size(); ++i) {
                map[data[i]] = 0;
            }
            auto mapDuration = std::chrono::high_resolution_clock::now() - mapStart;

            EXPECT_EQ(map.size(), refMap.size()) << f;
            std::cout << "fillStrategy: " << f
                      << " count: " << s_fillStrategys[f].count
                      << " Map: mapDuration / refDuration = " << double(mapDuration.count()) / double(refDuration.count()) << std::endl;

            std::map<TestData, char>::iterator refIt = refMap.begin();
            gepard::Map<TestData, char>::iterator mapIt = map.begin(), mapPIt = map.begin();
            while (refIt != refMap.end() && mapIt != map.end()) {
                EXPECT_EQ(mapIt->key, refIt->first) << f;
                EXPECT_LE(mapPIt->key, mapIt->key) << f;

                ++refIt;
                mapPIt = mapIt++;
            }
        }
    }
}

} // namespace binaryTreesTests

#endif // GEPARD_BINARY_TREES_TESTS_H
