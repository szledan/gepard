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
    struct Stats {
        int64_t callConstructor = 0;
        int64_t callCopyAssignmentOp = 0;
        int64_t callCopyConstructor = 0;
        int64_t callDestructor = 0;
        int64_t callMoveAssignmentOp = 0;
        int64_t callMoveConstructor = 0;
        void resetCounters()
        {
            callConstructor = 0;
            callCopyAssignmentOp = 0;
            callCopyConstructor = 0;
            callDestructor = 0;
            callMoveAssignmentOp = 0;
            callMoveConstructor = 0;
        }
    } static s_stats;
#define TEST_STATS(C, CA, CC, D, MA, MC) EXPECT_EQ(TestData::s_stats.callConstructor, (C)); EXPECT_EQ(TestData::s_stats.callCopyAssignmentOp, (CA)); EXPECT_EQ(TestData::s_stats.callCopyConstructor, (CC)); EXPECT_EQ(TestData::s_stats.callDestructor, (D)); EXPECT_EQ(TestData::s_stats.callMoveAssignmentOp, (MA)); EXPECT_EQ(TestData::s_stats.callMoveConstructor, (MC));

    static int64_t s_uid;
    int64_t uid;
    int64_t value;

    TestData(const int64_t v = 0) : uid(getUId()), value(v) { s_stats.callConstructor++; }
    TestData(const TestData& other) : uid(getUId()), value(other.value) { s_stats.callCopyConstructor++; }
    TestData(TestData&& other) : uid(std::move(other.uid)), value(std::move(other.value)) { s_stats.callMoveConstructor++; }
    ~TestData() { s_stats.callDestructor++; }

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
        s_stats.callMoveAssignmentOp++;
        this->uid = std::move(other.uid);
        this->value = std::move(other.value);
        return *this;
    }

    TestData& operator=(TestData& other)
    {
        s_stats.callCopyAssignmentOp++;
        this->uid = getUId();
        this->value = other.value;
        return *this;
    }

    const TestData& operator=(const TestData& other)
    {
        s_stats.callCopyAssignmentOp++;
        this->value = other.value;
        this->uid = getUId();
        return *this;
    }

private:
    const int64_t getUId() { return ++s_uid; }
};
TestData::Stats TestData::s_stats;
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
    { 1 << 16, [](){ static int64_t counter = (1 << 20) / 2; static int pwr = 1, times = 1;
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
    TestData::s_stats.resetCounters();
    TestData td(1);
    TEST_STATS(1, 0, 0, 0, 0, 0);

    gepard::_LinkedBinaryTree<TestData> linkedBinaryTree;

    linkedBinaryTree.uniqueInsert(td);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    linkedBinaryTree.uniqueInsert(td);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    linkedBinaryTree.uniqueInsert(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    TEST_STATS(2, 0, 2, 1, 0, 0);

    linkedBinaryTree.uniqueInsert(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    TEST_STATS(3, 0, 2, 2, 0, 0);

    linkedBinaryTree.uniqueInsert(TestData(3));
    EXPECT_EQ(linkedBinaryTree.size(), 3u);
    TEST_STATS(4, 0, 3, 3, 0, 0);
}

TEST(_LinkedBinaryTree, MultiInsert)
{
    TestData::s_stats.resetCounters();
    TestData td(1);
    TEST_STATS(1, 0, 0, 0, 0, 0);

    gepard::_LinkedBinaryTree<TestData> linkedBinaryTree;

    linkedBinaryTree.multiInsert(td);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    linkedBinaryTree.multiInsert(td);
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    TEST_STATS(1, 0, 2, 0, 0, 0);

    linkedBinaryTree.multiInsert(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 3u);
    TEST_STATS(2, 0, 3, 1, 0, 0);

    linkedBinaryTree.multiInsert(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 4u);
    TEST_STATS(3, 0, 4, 2, 0, 0);

    linkedBinaryTree.multiInsert(TestData(3));
    EXPECT_EQ(linkedBinaryTree.size(), 5u);
    TEST_STATS(4, 0, 5, 3, 0, 0);
}

TEST(_LinkedBinaryTree, Find)
{
    TestData::s_stats.resetCounters();
    TestData td(1);
    TEST_STATS(1, 0, 0, 0, 0, 0);

    gepard::_LinkedBinaryTree<TestData> linkedBinaryTree;
    EXPECT_EQ(linkedBinaryTree.find(td), linkedBinaryTree.end());
    TEST_STATS(1, 0, 0, 0, 0, 0);

    td.value = 2;
    linkedBinaryTree.multiInsert(td);
    td.value = -3;
    linkedBinaryTree.multiInsert(td);
    td.value = 4;
    linkedBinaryTree.multiInsert(td);
    EXPECT_EQ(linkedBinaryTree.find(td)->data, TestData(4));
    TEST_STATS(2, 0, 3, 1, 0, 0);

    EXPECT_EQ(linkedBinaryTree.find(TestData(-3))->data, linkedBinaryTree.begin()->data);
    TEST_STATS(3, 0, 3, 2, 0, 0);
}

TEST(Map, Insert)
{
    TestData::s_stats.resetCounters();
    gepard::Map<int, TestData> map;
    TEST_STATS(1, 0, 0, 0, 0, 0);

    TestData td(2);
    map.insert(1)->value = td;
    EXPECT_EQ(map.size(), 1u);
    TEST_STATS(2, 1, 1, 0, 0, 0);
    EXPECT_EQ(map[1], td);
    EXPECT_EQ(map.size(), 1u);
    TEST_STATS(2, 1, 1, 0, 0, 0);
    td.value = 3;
    EXPECT_EQ(map[1], TestData(2));
    EXPECT_EQ(map.size(), 1u);
    TEST_STATS(3, 1, 1, 1, 0, 0);
    map[4] = td;
    EXPECT_EQ(map[4], td);
    EXPECT_EQ(map.size(), 2u);
    TEST_STATS(3, 2, 2, 1, 0, 0);
}

TEST(Map, CompareSTDMap)
{
    for (size_t f = 0; f < sizeof(s_fillStrategys) / sizeof(s_fillStrategys[0]); ++f) {
        std::vector<TestData> data = fillVector(s_fillStrategys[f].count, s_fillStrategys[f].func);
        std::stringstream mapTestLog;
        mapTestLog << "fillStrategy: " << f << " count: " << s_fillStrategys[f].count;

        std::map<TestData, char> refMap;
        for (size_t i = 0; i < data.size(); ++i) {
            refMap[data[i]] = i % (sizeof(char));
        }
        gepard::Map<TestData, char> map;
        for (size_t i = 0; i < data.size(); ++i) {
            map[data[i]] = i % (sizeof(char));
        }

        EXPECT_EQ(map.size(), refMap.size()) << mapTestLog.str();

        std::map<TestData, char>::iterator refIt = refMap.begin();
        gepard::Map<TestData, char>::iterator mapIt = map.begin(), mapPIt;
        while (refIt != refMap.end() && mapIt != map.end()) {
            EXPECT_EQ(mapIt->key, refIt->first) << mapTestLog.str();
            EXPECT_EQ(mapIt->value, refIt->second) << mapTestLog.str();
            if (mapPIt != mapIt) {
                EXPECT_LT(mapPIt->key, mapIt->key) << mapTestLog.str();
            }
            ++refIt;
            mapPIt = mapIt++;
        }
    }
}

TEST(Map, DISABLED_SpeedTest)
{
    for (size_t f = 0; f < sizeof(s_fillStrategys) / sizeof(s_fillStrategys[0]); ++f) {
        std::vector<TestData> data = fillVector(s_fillStrategys[f].count, s_fillStrategys[f].func);

        if (s_fillStrategys[f].count < (1 << 8))
            continue;

        std::stringstream mapTestLog;
        mapTestLog << "fillStrategy: " << f << " count: " << s_fillStrategys[f].count;

        std::vector<std::chrono::high_resolution_clock::duration> refDur;
        std::vector<std::chrono::high_resolution_clock::duration> mapDur;
        for (int i = 0; i < 5; ++i) {
            size_t refSize;
            auto refMapStart = std::chrono::high_resolution_clock::now();
            {
                std::map<TestData, char> refMap;
                for (size_t i = 0; i < data.size(); ++i) {
                    refMap[data[i]] = i % (sizeof(char));
                }
                refSize = refMap.size();
            }
            auto refMapDuration = std::chrono::high_resolution_clock::now() - refMapStart;
            refDur.push_back(refMapDuration);

            size_t mapSize;
            auto mapStart = std::chrono::high_resolution_clock::now();
            {
                gepard::Map<TestData, char> map;
                for (size_t i = 0; i < data.size(); ++i) {
                    map[data[i]] = i % (sizeof(char));
                }
                mapSize = map.size();
            }
            auto mapDuration = std::chrono::high_resolution_clock::now() - mapStart;
            mapDur.push_back(mapDuration);

            EXPECT_EQ(mapSize, refSize) << mapTestLog.str();
        }
        std::sort(refDur.begin(), refDur.end());
        std::sort(mapDur.begin(), mapDur.end());
        // Compare medians.
        EXPECT_LT((double)mapDur[2].count() / (double)refDur[2].count(), 0.999);
        std::cout << ": " << (double)mapDur[2].count() / (double)refDur[2].count() << std::endl;
    }
}

} // namespace binaryTreesTests

#endif // GEPARD_BINARY_TREES_TESTS_H
