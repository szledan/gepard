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
#include <numeric>
#include <map>
#include <set>
#include <time.h>
#include <thread>
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

    friend bool operator!=(const TestData& lhs, const TestData& rhs)
    {
        return !(lhs.value == rhs.value);
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
    { 1, [](){ static thread_local int64_t counter = 0; return TestData(counter++); } },
    { 1, [](){ static thread_local int64_t counter = 10; return TestData(counter--); } },
    { 5, [](){ static thread_local int64_t counter = 0; return TestData(counter++); } },
    { 5, [](){ static thread_local int64_t counter = 10; return TestData(counter--); } },
    { 10, [](){ static thread_local int64_t counter = 0; return TestData(counter++); } },
    { 10, [](){ static thread_local int64_t counter = 10; return TestData(counter--); } },
    { 100, [](){ static thread_local int64_t counter = 0; return TestData(counter++); } },
    { 100, [](){ static thread_local int64_t counter = 100; return TestData(counter--); } },
    { 1 << 8, [](){ static thread_local int64_t counter = 1 << 8; return TestData(((counter % 2) ? -1 * (--counter) : 1 * (--counter))); } },
    { 100, [](){ return TestData(1); } },
    { 100, [](){ static thread_local int64_t counter = 0; return TestData(++counter % 5); } },
    { 1 << 16, [](){ static thread_local int64_t counter = (1ll << 31) / 2; static thread_local int pwr = 1, times = 1;
        if (!times) {
                counter = counter / 2;
                times = pwr = pwr << 1;
            }
            return TestData(counter + 2 * counter * --times);
        } },
};

TEST(BinaryTree, HeightAndSize)
{
    gepard::BinaryTree<int> binaryTree;

    EXPECT_EQ(binaryTree.size(), 0u);
    EXPECT_EQ(binaryTree.rootHeight(), -1);
    binaryTree.uniqueInsert(9);
    EXPECT_EQ(binaryTree.size(), 1u);
    EXPECT_EQ(binaryTree.rootHeight(), 0);
    binaryTree.uniqueInsert(9);
    EXPECT_EQ(binaryTree.size(), 1u);
    EXPECT_EQ(binaryTree.rootHeight(), 0);
    binaryTree.uniqueInsert(8);
    EXPECT_EQ(binaryTree.size(), 2u);
    EXPECT_EQ(binaryTree.rootHeight(), 1);
    binaryTree.uniqueInsert(8);
    EXPECT_EQ(binaryTree.size(), 2u);
    EXPECT_EQ(binaryTree.rootHeight(), 1);
    binaryTree.uniqueInsert(7);
    EXPECT_EQ(binaryTree.size(), 3u);
    EXPECT_EQ(binaryTree.rootHeight(), 1);
    binaryTree.uniqueInsert(6);
    EXPECT_EQ(binaryTree.size(), 4u);
    EXPECT_EQ(binaryTree.rootHeight(), 1);
    binaryTree.uniqueInsert(5);
    EXPECT_EQ(binaryTree.size(), 5u);
    EXPECT_EQ(binaryTree.rootHeight(), 2);
    binaryTree.uniqueInsert(4);
    EXPECT_EQ(binaryTree.size(), 6u);
    EXPECT_EQ(binaryTree.rootHeight(), 2);
    binaryTree.uniqueInsert(3);
    EXPECT_EQ(binaryTree.size(), 7u);
    EXPECT_EQ(binaryTree.rootHeight(), 2);
    binaryTree.uniqueInsert(2);
    EXPECT_EQ(binaryTree.size(), 8u);
    EXPECT_EQ(binaryTree.rootHeight(), 2);
    binaryTree.uniqueInsert(1);
    EXPECT_EQ(binaryTree.size(), 9u);
    EXPECT_EQ(binaryTree.rootHeight(), 2);
    binaryTree.uniqueInsert(1);
    EXPECT_EQ(binaryTree.size(), 9u);
    EXPECT_EQ(binaryTree.rootHeight(), 3);
    binaryTree.uniqueInsert(0);
    EXPECT_EQ(binaryTree.size(), 10u);
    EXPECT_EQ(binaryTree.rootHeight(), 3);
}

TEST(BinaryTree, Iterator)
{
    gepard::BinaryTree<TestData> uniqueLinkedBinaryTree;
    gepard::BinaryTree<TestData> multiLinkedBinaryTree;

    EXPECT_EQ(uniqueLinkedBinaryTree.begin(), uniqueLinkedBinaryTree.end());
    EXPECT_EQ(multiLinkedBinaryTree.begin(), multiLinkedBinaryTree.end());

    TestData::s_stats.resetCounters();
    for (int i = 0; i < 30; ++i) {
        uniqueLinkedBinaryTree.uniqueInsert(TestData(i / 3));
        TEST_STATS(i + 1, 0, 0, i + 1, 0, i / 3 + 1);
    }
    TestData::s_stats.resetCounters();
    std::set<TestData> set;
    for (int i = 0; i < 30; ++i) {
        set.insert(TestData(i / 3));
        TEST_STATS(i + 1, 0, 0, i + 1, 0, i / 3 + 1);
    }

    TestData::s_stats.resetCounters();
    for (int i = 0; i < 30; ++i) {
        multiLinkedBinaryTree.multiInsert(TestData(i / 3));
        TEST_STATS(i + 1, 0, 0, i + 1, 0, i + 1);
    }
    TestData::s_stats.resetCounters();
    std::multiset<TestData> mset;
    for (int i = 0; i < 30; ++i) {
        mset.insert(TestData(i / 3));
        TEST_STATS(i + 1, 0, 0, i + 1, 0, i + 1);
    }

    EXPECT_EQ(uniqueLinkedBinaryTree.size(), 10u);
    EXPECT_EQ(multiLinkedBinaryTree.size(), 30u);

    gepard::BinaryTree<TestData>::iterator uIt = uniqueLinkedBinaryTree.begin();
    gepard::BinaryTree<TestData>::iterator uItPrev = uIt++;
    while (uIt != uniqueLinkedBinaryTree.end()) {
        EXPECT_LT(*uItPrev, *uIt);
        uItPrev = uIt++;
        TEST_STATS(30, 0, 0, 30, 0, 30);
    }

    gepard::BinaryTree<TestData>::iterator mIt = multiLinkedBinaryTree.begin();
    gepard::BinaryTree<TestData>::iterator mItPrev = mIt++;
    while (mIt != uniqueLinkedBinaryTree.end()) {
        EXPECT_LE(*mItPrev, *mIt);
        mItPrev = mIt++;
        TEST_STATS(30, 0, 0, 30, 0, 30);
    }
}
TEST(BinaryTree, UniqueInsert)
{
    TestData::s_stats.resetCounters();
    TestData td(1);
    TEST_STATS(1, 0, 0, 0, 0, 0);

    gepard::BinaryTree<TestData> binaryTree;

    binaryTree.uniqueInsert(td);
    EXPECT_EQ(binaryTree.size(), 1u);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    binaryTree.uniqueInsert(td);
    EXPECT_EQ(binaryTree.size(), 1u);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    binaryTree.uniqueInsert(TestData(2));
    EXPECT_EQ(binaryTree.size(), 2u);
    TEST_STATS(2, 0, 1, 1, 0, 1);

    binaryTree.uniqueInsert(TestData(2));
    EXPECT_EQ(binaryTree.size(), 2u);
    TEST_STATS(3, 0, 1, 2, 0, 1);

    binaryTree.uniqueInsert(TestData(3));
    EXPECT_EQ(binaryTree.size(), 3u);
    TEST_STATS(4, 0, 1, 3, 0, 2);
}

TEST(BinaryTree, MultiInsert)
{
    TestData::s_stats.resetCounters();
    TestData td(1);
    TEST_STATS(1, 0, 0, 0, 0, 0);

    gepard::BinaryTree<TestData> binaryTree;

    binaryTree.multiInsert(td);
    EXPECT_EQ(binaryTree.size(), 1u);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    binaryTree.multiInsert(td);
    EXPECT_EQ(binaryTree.size(), 2u);
    TEST_STATS(1, 0, 2, 0, 0, 0);

    binaryTree.multiInsert(TestData(2));
    EXPECT_EQ(binaryTree.size(), 3u);
    TEST_STATS(2, 0, 2, 1, 0, 1);

    binaryTree.multiInsert(TestData(2));
    EXPECT_EQ(binaryTree.size(), 4u);
    TEST_STATS(3, 0, 2, 2, 0, 2);

    binaryTree.multiInsert(TestData(3));
    EXPECT_EQ(binaryTree.size(), 5u);
    TEST_STATS(4, 0, 2, 3, 0, 3);
}

TEST(BinaryTree, Find)
{
    TestData::s_stats.resetCounters();
    TestData td(1);
    TEST_STATS(1, 0, 0, 0, 0, 0);

    gepard::BinaryTree<TestData> binaryTree;
    EXPECT_EQ(binaryTree.find(td).it, binaryTree.end());
    TEST_STATS(1, 0, 0, 0, 0, 0);

    td.value = 2;
    binaryTree.multiInsert(td);
    td.value = -3;
    binaryTree.multiInsert(td);
    td.value = 4;
    binaryTree.multiInsert(td);
    EXPECT_EQ(*(binaryTree.find(td).it), TestData(4));
    TEST_STATS(2, 0, 3, 1, 0, 0);

    EXPECT_EQ(*(binaryTree.find(TestData(-3)).it), *(binaryTree.begin()));
    TEST_STATS(3, 0, 3, 2, 0, 0);
}

TEST(BinaryTree, ReturnType)
{
    TestData::s_stats.resetCounters();
    TestData td(1);
    TEST_STATS(1, 0, 0, 0, 0, 0);
    gepard::BinaryTree<TestData> binaryTree;
    auto a = binaryTree.multiInsert(td);
    auto b = binaryTree.begin();
    EXPECT_EQ(binaryTree.size(), 1u);
    EXPECT_EQ(*(a.it), td);
    EXPECT_EQ(a.prev, nullptr);
    EXPECT_EQ(a.isNew, true);
    EXPECT_EQ(*b, td);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    td.value = 2;
    auto c = binaryTree.multiInsert(td);
    EXPECT_EQ(binaryTree.size(), 2u);
    EXPECT_EQ(*(c.it), td);
    EXPECT_EQ(c.prev->value, 1);
    EXPECT_EQ(c.isNew, true);
    EXPECT_EQ(*(c.prev), *(a.it));
    EXPECT_EQ(*(c.prev), *b);
    TEST_STATS(1, 0, 2, 0, 0, 0);

    gepard::BinaryTree<TestData> linkedBinaryTree2;
    auto d = linkedBinaryTree2.begin();
    EXPECT_EQ(linkedBinaryTree2.size(), 0u);
    EXPECT_EQ(d, linkedBinaryTree2.end());
}

#define VERBOSE false
#define REPEAT 1
#define THRD_CNT 1
#define MILLI_SEC 1000
#define MICRO_SEC 1000 * MILLI_SEC
#define NANO_SEC 1000 * MICRO_SEC
#define ACCUMULATE(TS_START, TS_STOP) NANO_SEC * (double(TS_STOP.tv_sec - TS_START.tv_sec) + double(TS_STOP.tv_nsec - TS_START.tv_nsec) / (1000000000.0));

TEST(BinaryTree, CloneUnique)
{
    for (size_t f = 0; f < sizeof(s_fillStrategys) / sizeof(s_fillStrategys[0]); ++f) {
        std::vector<TestData> data = fillVector(s_fillStrategys[f].count, s_fillStrategys[f].func);

        std::stringstream cloneTestLog;
        cloneTestLog << "fillStrategy: " << f << " count: " << s_fillStrategys[f].count;

        const int num = REPEAT;
        const int thrdCnt = THRD_CNT;
        std::vector<std::thread> threads;
        std::vector<double> cloneDur(num * thrdCnt, 0.0f);
        std::vector<double> insertDur(num * thrdCnt, 0.0f);
        for (int t = 0; t < thrdCnt; ++t) {
            threads.push_back(std::thread([&](int thrd){
                for (int i = 0; i < num; ++i) {
                    const clockid_t clk_id = CLOCK_THREAD_CPUTIME_ID;
                    struct timespec start, stop;

                    clock_gettime(clk_id, &start);
                    gepard::BinaryTree<TestData> insertBinTree;
                    for (size_t j = 0; j < data.size(); ++j) {
                        insertBinTree.uniqueInsert(data[j]);
                    }
                    clock_gettime(clk_id, &stop);
                    insertDur[i * thrdCnt + thrd] = ACCUMULATE(start, stop);

                    clock_gettime(clk_id, &start);
                    gepard::BinaryTree<TestData> clonedBinTree = insertBinTree.clone();
                    clock_gettime(clk_id, &stop);
                    cloneDur[i * thrdCnt + thrd] = ACCUMULATE(start, stop);

                    EXPECT_EQ(insertBinTree.size(), clonedBinTree.size()) << cloneTestLog.str();
                    if (i == 0) {
                        gepard::BinaryTree<TestData>::iterator insertLbtIt = insertBinTree.begin(), clonedLbtIt = clonedBinTree.begin();
                        while (insertLbtIt != insertBinTree.end() && clonedLbtIt != clonedBinTree.end()) {
                            EXPECT_EQ(*insertLbtIt, *clonedLbtIt) << cloneTestLog.str();
                            ++insertLbtIt;
                            ++clonedLbtIt;
                        }
                    }
                }
            }, t));
        }
        for (size_t t = 0; t < threads.size(); ++t) {
            threads[t].join();
        }

        std::sort(cloneDur.begin(), cloneDur.end());
        std::sort(insertDur.begin(), insertDur.end());
        for (size_t i = 0; i < cloneDur.size(); ++i) {
            cloneDur[i] = cloneDur[i] / insertDur[i];
        }
        std::sort(cloneDur.begin(), cloneDur.end());
        if (VERBOSE) {
            std::cout << std::fixed << std::setprecision(5) << " clone/insert:";
            std::cout << "  avg: " << std::accumulate(cloneDur.begin(), cloneDur.end(), 0.0) / cloneDur.size();
            std::cout << "  min: " << cloneDur.front();
            std::cout << "  med: " << cloneDur[cloneDur.size() / 2];
            std::cout << "  max: " << cloneDur.back();
            std::cout << std::endl;
        }
    }
}

TEST(BinaryTree, CloneMulti)
{
    for (size_t f = 0; f < sizeof(s_fillStrategys) / sizeof(s_fillStrategys[0]); ++f) {
        std::vector<TestData> data = fillVector(s_fillStrategys[f].count, s_fillStrategys[f].func);

        std::stringstream cloneTestLog;
        cloneTestLog << "fillStrategy: " << f << " count: " << s_fillStrategys[f].count;

        const int num = REPEAT;
        const int thrdCnt = THRD_CNT;
        std::vector<std::thread> threads;
        std::vector<double> cloneDur(num * thrdCnt, 0.0f);
        std::vector<double> insertDur(num * thrdCnt, 0.0f);
        for (int t = 0; t < thrdCnt; ++t) {
            threads.push_back(std::thread([&](int thrd){
                for (int i = 0; i < num; ++i) {
                    const clockid_t clk_id = CLOCK_THREAD_CPUTIME_ID;
                    struct timespec start, stop;

                    clock_gettime(clk_id, &start);
                    gepard::BinaryTree<TestData> insertBinTree;
                    for (size_t j = 0; j < data.size(); ++j) {
                        insertBinTree.multiInsert(data[j]);
                    }
                    clock_gettime(clk_id, &stop);
                    insertDur[i * thrdCnt + thrd] = ACCUMULATE(start, stop);

                    clock_gettime(clk_id, &start);
                    gepard::BinaryTree<TestData> clonedBinTree = insertBinTree.clone();
                    clock_gettime(clk_id, &stop);
                    cloneDur[i * thrdCnt + thrd] = ACCUMULATE(start, stop);

                    EXPECT_EQ(insertBinTree.size(), clonedBinTree.size()) << cloneTestLog.str();
                    if (i == 0) {
                        gepard::BinaryTree<TestData>::iterator insertLbtIt = insertBinTree.begin(), clonedLbtIt = clonedBinTree.begin();
                        while (insertLbtIt != insertBinTree.end() && clonedLbtIt != clonedBinTree.end()) {
                            EXPECT_EQ(*insertLbtIt, *clonedLbtIt) << cloneTestLog.str();
                            ++insertLbtIt;
                            ++clonedLbtIt;
                        }
                    }
                }
            }, t));
        }
        for (size_t t = 0; t < threads.size(); ++t) {
            threads[t].join();
        }

        std::sort(insertDur.begin(), insertDur.end());
        for (size_t i = 0; i < cloneDur.size(); ++i) {
            cloneDur[i] = cloneDur[i] / insertDur[i];
        }
        std::sort(cloneDur.begin(), cloneDur.end());
        if (VERBOSE) {
            std::cout << std::fixed << std::setprecision(5) << " clone/insert:";
            std::cout << "  avg: " << std::accumulate(cloneDur.begin(), cloneDur.end(), 0.0) / cloneDur.size();
            std::cout << "  min: " << cloneDur.front();
            std::cout << "  med: " << cloneDur[cloneDur.size() / 2];
            std::cout << "  max: " << cloneDur.back();
            std::cout << std::endl;
        }
    }
}

TEST(BinaryTree, CompareSTDSet)
{
    for (size_t f = 0; f < sizeof(s_fillStrategys) / sizeof(s_fillStrategys[0]); ++f) {
        std::vector<TestData> data = fillVector(s_fillStrategys[f].count, s_fillStrategys[f].func);
        std::stringstream uniqueSetTestLog;
        uniqueSetTestLog << "fillStrategy: " << f << " count: " << s_fillStrategys[f].count;

        TestData::s_stats.resetCounters();
        gepard::BinaryTree<TestData> uniqueSet;
        int j = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            auto ret = uniqueSet.uniqueInsert(data[i]);
            if (ret.isNew) {
                TEST_STATS(0, 0, ++j, 0, 0, 0);
            }
        }
        TestData::s_stats.resetCounters();
        std::set<TestData> refSet;
        j = 0;
        for (size_t i = 0; i < data.size(); ++i) {
            auto ret = refSet.insert(data[i]);
            if (ret.second) {
                TEST_STATS(0, 0, ++j, 0, 0, 0);
            }
        }

        EXPECT_EQ(uniqueSet.size(), refSet.size()) << uniqueSetTestLog.str();

        std::set<TestData>::iterator refIt = refSet.begin();
        gepard::BinaryTree<TestData>::iterator usetIt = uniqueSet.begin(), usetPIt = uniqueSet.begin();
        while (refIt != refSet.end() && usetIt != uniqueSet.end()) {
            EXPECT_EQ(*usetIt, *refIt) << uniqueSetTestLog.str();
            EXPECT_LE(*usetPIt, *usetIt) << uniqueSetTestLog.str();
            ++refIt;
            usetPIt = usetIt++;
        }
    }
}

TEST(BinaryTree, CompareSTDMultiset)
{
    for (size_t f = 0; f < sizeof(s_fillStrategys) / sizeof(s_fillStrategys[0]); ++f) {
        std::vector<TestData> data = fillVector(s_fillStrategys[f].count, s_fillStrategys[f].func);
        std::stringstream multiSetTestLog;
        multiSetTestLog << "fillStrategy: " << f << " count: " << s_fillStrategys[f].count;

        TestData::s_stats.resetCounters();
        gepard::BinaryTree<TestData> multiSet;
        for (size_t i = 0; i < data.size(); ++i) {
            multiSet.multiInsert(data[i]);
            TEST_STATS(0, 0, (int)i + 1, 0, 0, 0);
        }
        TestData::s_stats.resetCounters();
        std::multiset<TestData> refMultiset;
        for (size_t i = 0; i < data.size(); ++i) {
            refMultiset.insert(data[i]);
            TEST_STATS(0, 0, (int)i + 1, 0, 0, 0);
        }

        EXPECT_EQ(multiSet.size(), refMultiset.size()) << multiSetTestLog.str();

        std::multiset<TestData>::iterator refIt = refMultiset.begin();
        gepard::BinaryTree<TestData>::iterator msetIt = multiSet.begin(), msetPIt = multiSet.begin();
        while (refIt != refMultiset.end() && msetIt != multiSet.end()) {
            EXPECT_EQ(*msetIt, *refIt) << multiSetTestLog.str();
            EXPECT_LE(*msetPIt, *msetIt) << multiSetTestLog.str();
            ++refIt;
            msetPIt = msetIt++;
        }
    }
}

TEST(BinaryTree, DISABLED_SpeedTest)
{
    for (size_t f = 0; f < sizeof(s_fillStrategys) / sizeof(s_fillStrategys[0]); ++f) {
        if (s_fillStrategys[f].count < 1 << 8)
            continue;

        std::vector<TestData> data = fillVector(s_fillStrategys[f].count, s_fillStrategys[f].func);

        std::stringstream compareTestLog;
        compareTestLog << "fillStrategy: " << f << " count: " << s_fillStrategys[f].count;

        const int num = REPEAT;
        const int thrdCnt = THRD_CNT;
        std::vector<std::thread> threads;
        std::vector<double> msetDur(num * thrdCnt, 0.0f);
        std::vector<double> binTreeDur(num * thrdCnt, 0.0f);
        for (int t = 0; t < thrdCnt; ++t) {
            threads.push_back(std::thread([&](int thrd){
                for (int i = 0; i < num; ++i) {
                    const clockid_t clk_id = CLOCK_THREAD_CPUTIME_ID;
                    struct timespec start, stop;

                    clock_gettime(clk_id, &start);
                    gepard::BinaryTree<TestData> binTree;
                    for (size_t j = 0; j < data.size(); ++j) {
                        binTree.multiInsert(data[j]);
                    }
                    clock_gettime(clk_id, &stop);
                    binTreeDur[i * thrdCnt + thrd] = ACCUMULATE(start, stop);

                    clock_gettime(clk_id, &start);
                    std::multiset<TestData> mset;
                    for (size_t j = 0; j < data.size(); ++j) {
                        mset.insert(data[j]);
                    }
                    clock_gettime(clk_id, &stop);
                    msetDur[i * thrdCnt + thrd] = ACCUMULATE(start, stop);
                }
            }, t));
        }
        for (size_t t = 0; t < threads.size(); ++t) {
            threads[t].join();
        }

        std::sort(binTreeDur.begin(), binTreeDur.end());
        std::sort(msetDur.begin(), msetDur.end());
        for (size_t i = 0; i < msetDur.size(); ++i) {
            msetDur[i] = binTreeDur[i] / msetDur[i];
        }
        std::sort(msetDur.begin(), msetDur.end());
        if (VERBOSE) {
            std::cout << std::fixed << std::setprecision(5) << " binTree/ref:";
            std::cout << "  avg: " << std::accumulate(msetDur.begin(), msetDur.end(), 0.0) / msetDur.size();
            std::cout << "  min: " << msetDur.front();
            std::cout << "  med: " << msetDur[msetDur.size() / 2];
            std::cout << "  max: " << msetDur.back();
            std::cout << std::endl;
        }
        // Compare medians.
        EXPECT_LT(msetDur[msetDur.size() / 2], 0.999);
    }
}

} // namespace binaryTreesTests

#endif // GEPARD_BINARY_TREES_TESTS_H
