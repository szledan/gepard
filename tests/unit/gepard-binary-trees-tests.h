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
        return lhs.value == rhs.value && lhs.uid == rhs.uid;
    }
};
int64_t TestData::s_uid = 0;

std::ostream& operator<<(std::ostream& os, const TestData& obj)
{
    os << obj.value;
    return os;
}

TEST(BinaryTree, CompareToSTDMultimap)
{
    std::vector<std::pair<size_t, std::function<TestData()>>> functions = {
        std::make_pair(1, [](){ static int64_t counter = 0; return TestData(counter++); }),
        std::make_pair(1, [](){ static int64_t counter = 10; return TestData(counter--); }),
        std::make_pair(10, [](){ static int64_t counter = 0; return TestData(counter++); }),
        std::make_pair(10, [](){ static int64_t counter = 10; return TestData(counter--); }),
        std::make_pair(100, [](){ static int64_t counter = 0; return TestData(counter++); }),
        std::make_pair(100, [](){ static int64_t counter = 100; return TestData(counter--); }),
        std::make_pair(1 << 8, [](){ static int64_t counter = 1 << 8; return TestData(((counter % 2) ? -1 : 1) * (--counter)); }),
        std::make_pair(100, [](){ return TestData(1); }),
        std::make_pair(100, [](){ static int64_t counter = 0; return TestData(++counter % 5); }),
        std::make_pair(1 << 16, [](){ static int64_t counter = (1 << 16) / 2; static int pwr = 1, times = 1;
            if (!times) {
                    counter = counter / 2;
                    times = pwr = pwr << 1;
                }
                return TestData(counter + 2 * counter * --times);
            }),
    };

    for (size_t f = 0; f < functions.size(); ++f) {
        std::vector<TestData> data = fillVector(functions[f].first, functions[f].second);
        std::multimap<TestData, size_t> refTree;
        gepard::BinaryTree<TestData> binaryTree;

        auto refStart = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < data.size(); ++i) {
            refTree.emplace(data[i], 0);
        }
        auto refDuration = std::chrono::high_resolution_clock::now() - refStart;

        auto binaryStart = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < data.size(); ++i) {
            binaryTree.insert(data[i]);
        }
        auto binaryDuration = std::chrono::high_resolution_clock::now() - binaryStart;

        EXPECT_EQ(binaryTree.size(), refTree.size()) << f;
        if (functions[f].first > (1 << 5))
            EXPECT_LT(binaryDuration, refDuration) << f;
//        std::cout << "binaryDuration / refDuration : " << double(binaryDuration.count()) / double(refDuration.count()) << std::endl;

        std::multimap<TestData, size_t>::iterator rit = refTree.begin();
        gepard::BinaryTree<TestData>::iterator it = binaryTree.begin(), pit = binaryTree.begin();
        while (it != binaryTree.end() && rit != refTree.end()) {
            EXPECT_EQ(it->data, rit->first) << f;
            EXPECT_LE(pit->data, it->data) << f;
            pit = it++; ++rit;
        }
    }
}

} // anonymous namespace

#endif // GEPARD_BINARY_TREES_TESTS_H
