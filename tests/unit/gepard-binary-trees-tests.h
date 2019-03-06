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
    { 1, [](){ static int64_t counter = 0; return TestData(counter++); } },
    { 1, [](){ static int64_t counter = 10; return TestData(counter--); } },
    { 10, [](){ static int64_t counter = 0; return TestData(counter++); } },
    { 10, [](){ static int64_t counter = 10; return TestData(counter--); } },
    { 100, [](){ static int64_t counter = 0; return TestData(counter++); } },
    { 100, [](){ static int64_t counter = 100; return TestData(counter--); } },
    { 1 << 8, [](){ static int64_t counter = 1 << 8; return TestData(((counter % 2) ? -1 * (--counter) : 1 * (--counter))); } },
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
    gepard::_LinkedBinaryTree<TestData> uniqueLinkedBinaryTree;
    gepard::_LinkedBinaryTree<TestData> multiLinkedBinaryTree;

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

    gepard::_LinkedBinaryTree<TestData>::iterator uIt = uniqueLinkedBinaryTree.begin();
    gepard::_LinkedBinaryTree<TestData>::iterator uItPrev = uIt++;
    while (uIt != uniqueLinkedBinaryTree.end()) {
        EXPECT_LT(*(uItPrev->data), *(uIt->data));
        if (uItPrev->prev) {
            EXPECT_EQ(*(uItPrev->data), *(uIt->prev));
        }
        uItPrev = uIt++;
        TEST_STATS(30, 0, 0, 30, 0, 30);
    }

    gepard::_LinkedBinaryTree<TestData>::iterator mIt = multiLinkedBinaryTree.begin();
    gepard::_LinkedBinaryTree<TestData>::iterator mItPrev = mIt++;
    while (mIt != uniqueLinkedBinaryTree.end()) {
        EXPECT_LE(*(mItPrev->data), *(mIt->data));
        if (mItPrev->prev) {
            EXPECT_EQ(*(mItPrev->data), *(mIt->prev));
        }
        mItPrev = mIt++;
        TEST_STATS(30, 0, 0, 30, 0, 30);
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
    TEST_STATS(2, 0, 1, 1, 0, 1);

    linkedBinaryTree.uniqueInsert(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    TEST_STATS(3, 0, 1, 2, 0, 1);

    linkedBinaryTree.uniqueInsert(TestData(3));
    EXPECT_EQ(linkedBinaryTree.size(), 3u);
    TEST_STATS(4, 0, 1, 3, 0, 2);
}

TEST(_LinkedBinaryTree, UniqueEmplace)
{
    TestData::s_stats.resetCounters();
    TestData td(1);
    TEST_STATS(1, 0, 0, 0, 0, 0);

    gepard::_LinkedBinaryTree<TestData> linkedBinaryTree;

    linkedBinaryTree.uniqueEmplace(td);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    linkedBinaryTree.uniqueEmplace(td);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    linkedBinaryTree.uniqueEmplace(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    TEST_STATS(2, 0, 1, 1, 0, 1);

    linkedBinaryTree.uniqueEmplace(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    TEST_STATS(3, 0, 1, 2, 0, 1);

    linkedBinaryTree.uniqueEmplace(TestData(3));
    EXPECT_EQ(linkedBinaryTree.size(), 3u);
    TEST_STATS(4, 0, 1, 3, 0, 2);
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
    TEST_STATS(2, 0, 2, 1, 0, 1);

    linkedBinaryTree.multiInsert(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 4u);
    TEST_STATS(3, 0, 2, 2, 0, 2);

    linkedBinaryTree.multiInsert(TestData(3));
    EXPECT_EQ(linkedBinaryTree.size(), 5u);
    TEST_STATS(4, 0, 2, 3, 0, 3);
}

TEST(_LinkedBinaryTree, MultiEmplace)
{
    TestData::s_stats.resetCounters();
    TestData td(1);
    TEST_STATS(1, 0, 0, 0, 0, 0);

    gepard::_LinkedBinaryTree<TestData> linkedBinaryTree;

    linkedBinaryTree.multiEmplace(td);
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    linkedBinaryTree.multiEmplace(td);
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    TEST_STATS(1, 0, 2, 0, 0, 0);

    linkedBinaryTree.multiEmplace(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 3u);
    TEST_STATS(2, 0, 2, 1, 0, 1);

    linkedBinaryTree.multiEmplace(TestData(2));
    EXPECT_EQ(linkedBinaryTree.size(), 4u);
    TEST_STATS(3, 0, 2, 2, 0, 2);

    linkedBinaryTree.multiEmplace(TestData(3));
    EXPECT_EQ(linkedBinaryTree.size(), 5u);
    TEST_STATS(4, 0, 2, 3, 0, 3);
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
    EXPECT_EQ(*(linkedBinaryTree.find(td)->data), TestData(4));
    TEST_STATS(2, 0, 3, 1, 0, 0);

    EXPECT_EQ(*(linkedBinaryTree.find(TestData(-3))->data), *(linkedBinaryTree.begin()->data));
    TEST_STATS(3, 0, 3, 2, 0, 0);
}

TEST(_LinkedBinaryTree, RetType)
{
    TestData::s_stats.resetCounters();
    TestData td(1);
    TEST_STATS(1, 0, 0, 0, 0, 0);
    gepard::_LinkedBinaryTree<TestData> linkedBinaryTree;
    auto a = linkedBinaryTree.multiInsert(td);
    auto b = linkedBinaryTree.begin();
    EXPECT_EQ(linkedBinaryTree.size(), 1u);
    EXPECT_EQ(a->isNew, true);
    EXPECT_EQ(b->isNew, false);
    EXPECT_EQ(*(b->data), td);
    EXPECT_EQ(b->prev, nullptr);
    TEST_STATS(1, 0, 1, 0, 0, 0);

    td.value = 2;
    auto it = linkedBinaryTree.multiInsert(td);
    EXPECT_EQ(linkedBinaryTree.size(), 2u);
    EXPECT_EQ(it->isNew, true);
    EXPECT_EQ(*(it->data), td);
    EXPECT_EQ(*(it->prev), *(b->data));
    TEST_STATS(1, 0, 2, 0, 0, 0);

    gepard::_LinkedBinaryTree<TestData> linkedBinaryTree2;
    auto b2 = linkedBinaryTree2.begin();
    EXPECT_EQ(linkedBinaryTree2.size(), 0u);
    EXPECT_EQ(b2->data, nullptr);
    EXPECT_EQ(b2->prev, nullptr);
    EXPECT_EQ(b2->isNew, false);
}

TEST(Set, Find)
{
    //! \todo: unittest
}

TEST(Set, Insert)
{
    //! \todo: unittest
}

TEST(Set, Emplace)
{
    //! \todo: unittest
}

TEST(Set, CompareSTDSet)
{
    //! \todo: unittest
}

TEST(Set, DISABLED_SpeedTest)
{
    //! \todo: unittest
}

TEST(MultiSet, Find)
{
    //! \todo: unittest
}

TEST(MultiSet, Insert)
{
    //! \todo: unittest
}

TEST(MultiSet, Emplace)
{
    //! \todo: unittest
}

TEST(MultiSet, CompareSTDMultiset)
{
    for (size_t f = 0; f < sizeof(s_fillStrategys) / sizeof(s_fillStrategys[0]); ++f) {
        std::vector<TestData> data = fillVector(s_fillStrategys[f].count, s_fillStrategys[f].func);
        std::stringstream multiSetTestLog;
        multiSetTestLog << "fillStrategy: " << f << " count: " << s_fillStrategys[f].count;

        TestData::s_stats.resetCounters();
        gepard::MultiSet<TestData> multiSet;
        for (size_t i = 0; i < data.size(); ++i) {
            multiSet.insert(data[i]);
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
        gepard::MultiSet<TestData>::iterator msetIt = multiSet.begin(), msetPIt = multiSet.begin();
        while (refIt != refMultiset.end() && msetIt != multiSet.end()) {
            EXPECT_EQ(*msetIt->data, *refIt) << multiSetTestLog.str();
            EXPECT_LE(*msetPIt->data, *msetIt->data) << multiSetTestLog.str();
            ++refIt;
            msetPIt = msetIt++;
        }
    }
}

TEST(MultiSet, DISABLED_SpeedTest)
{
    for (size_t f = 0; f < sizeof(s_fillStrategys) / sizeof(s_fillStrategys[0]); ++f) {
        std::vector<TestData> data = fillVector(s_fillStrategys[f].count, s_fillStrategys[f].func);

        if (s_fillStrategys[f].count < (1 << 8))
            continue;

        std::stringstream multiSetTestLog;
        multiSetTestLog << "fillStrategy: " << f << " count: " << s_fillStrategys[f].count;

        std::vector<std::chrono::high_resolution_clock::duration> refDur;
        std::vector<std::chrono::high_resolution_clock::duration> msetDur;
        for (int i = 0; i < 5; ++i) {
            size_t refSize;
            auto refMultisetStart = std::chrono::high_resolution_clock::now();
            {
                std::multiset<TestData> refMultiset;
                for (size_t i = 0; i < data.size(); ++i) {
                    refMultiset.insert(data[i]);
                }
                refSize = refMultiset.size();
            }
            auto refMultisetDuration = std::chrono::high_resolution_clock::now() - refMultisetStart;
            refDur.push_back(refMultisetDuration);

            size_t msetSize;
            auto msetStart = std::chrono::high_resolution_clock::now();
            {
                gepard::MultiSet<TestData> mset;
                for (size_t i = 0; i < data.size(); ++i) {
                    mset.insert(data[i]);
                }
                msetSize = mset.size();
            }
            auto msetDuration = std::chrono::high_resolution_clock::now() - msetStart;
            msetDur.push_back(msetDuration);

            EXPECT_EQ(msetSize, refSize) << multiSetTestLog.str();
        }
        std::sort(refDur.begin(), refDur.end());
        std::sort(msetDur.begin(), msetDur.end());
        // Compare medians.
        EXPECT_LT((double)msetDur[2].count() / (double)refDur[2].count(), 0.999);
        std::cout << "multiset: " << (double)msetDur[2].count() / (double)refDur[2].count() << std::endl;
    }
}

} // namespace binaryTreesTests

#endif // GEPARD_BINARY_TREES_TESTS_H
