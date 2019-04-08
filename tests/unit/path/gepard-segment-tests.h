/* Copyright (C) 2018-2019, Gepard Graphics
 * Copyright (C) 2018-2019, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_SEGMENT_TESTS_H
#define GEPARD_SEGMENT_TESTS_H

#include "gepard-binary-tree.h"
#include "path/gepard-segment.h"
#include "gtest/gtest.h"
#include <cmath>
#include <vector>

namespace {

TEST(SegmentTest, Constructor)
{
    const double pi = 2.0 * std::asin(1.0);
    const uint32_t currentUID = gepard::s_uid;

    {
        const gepard::Segment segment(gepard::FloatPoint(-pi, -pi), gepard::FloatPoint(pi, pi));
        EXPECT_EQ(segment.uid, currentUID + 1) << "";
        EXPECT_EQ(segment.topX, -pi) << "";
        EXPECT_EQ(segment.topY, std::floor(-pi)) << "";
        EXPECT_EQ(segment.bottomX, pi) << "";
        EXPECT_EQ(segment.bottomY, std::floor(pi)) << "";
        EXPECT_EQ(segment.dx, (segment.bottomX - segment.topX) / (segment.bottomY - segment.topY)) << "";
        EXPECT_EQ(segment.direction, 1) << "";
    }

    {
        const gepard::Segment segment(gepard::FloatPoint(pi, pi), gepard::FloatPoint(-pi, -pi));
        EXPECT_EQ(segment.uid, currentUID + 2) << "";
        EXPECT_EQ(segment.topX, -pi) << "";
        EXPECT_EQ(segment.topY, std::floor(-pi)) << "";
        EXPECT_EQ(segment.bottomX, pi) << "";
        EXPECT_EQ(segment.bottomY, std::floor(pi)) << "";
        EXPECT_EQ(segment.dx, (segment.bottomX - segment.topX) / (segment.bottomY - segment.topY)) << "";
        EXPECT_EQ(segment.direction, -1) << "";
    }

    {
        const gepard::Segment segment(gepard::FloatPoint(pi, pi), gepard::FloatPoint(pi, -pi));
        EXPECT_EQ(segment.uid, currentUID + 3) << "";
        EXPECT_EQ(segment.topX, pi) << "";
        EXPECT_EQ(segment.topY, std::floor(-pi)) << "";
        EXPECT_EQ(segment.bottomX, pi) << "";
        EXPECT_EQ(segment.bottomY, std::floor(pi)) << "";
        EXPECT_EQ(segment.dx, 0.0) << "";
        EXPECT_EQ(segment.direction, -1) << "";
    }

    {
        const gepard::Segment segment(gepard::FloatPoint(pi, pi), gepard::FloatPoint(-pi, pi));
        EXPECT_EQ(segment.uid, currentUID + 4) << "";
        EXPECT_EQ(segment.topY, std::floor(pi)) << "";
        EXPECT_EQ(segment.bottomY, std::floor(pi)) << "";
        EXPECT_EQ(segment.direction, 0) << "";
    }
}

TEST(SegmentTest, XFunction)
{
    const gepard::Segment segment(gepard::FloatPoint(-1.0, -2.2), gepard::FloatPoint(1.0, 3));
    EXPECT_EQ(segment.x(0), 0.0) << "";
    EXPECT_FLOAT_EQ(segment.x(1), 1.0 / 3.0) << "";
    EXPECT_FLOAT_EQ(segment.x(-1), -1.0 / 3.0) << "";
}

TEST(SegmentTest, CutAndRemoveTop)
{
    const uint32_t currentUID = gepard::s_uid;
    gepard::Segment sBase(gepard::FloatPoint(-1.0, -2.2), gepard::FloatPoint(1.0, 3.9));
    gepard::Segment sTop = sBase.cutAndRemoveTop(1);

    EXPECT_EQ(sBase.uid, currentUID + 1) << "";
    EXPECT_FLOAT_EQ(sBase.topX, 1.0 / 3.0) << "";
    EXPECT_EQ(sBase.topY, 1) << "";
    EXPECT_EQ(sBase.bottomX, 1.0) << "";
    EXPECT_EQ(sBase.bottomY, 3) << "";
    EXPECT_FLOAT_EQ(sBase.dx, 1.0 / 3.0) << "";
    EXPECT_EQ(sBase.direction, 1) << "";

    EXPECT_EQ(sTop.uid, currentUID + 1) << "";
    EXPECT_EQ(sTop.topX, -1.0) << "";
    EXPECT_EQ(sTop.topY, -3) << "";
    EXPECT_FLOAT_EQ(sTop.bottomX, 1.0 / 3.0) << "";
    EXPECT_EQ(sTop.bottomY, 1) << "";
    EXPECT_FLOAT_EQ(sTop.dx, 1.0 / 3.0) << "";
    EXPECT_EQ(sTop.direction, 1) << "";
}

TEST(SegmentTest, CutAndRemoveBottom)
{
    const uint32_t currentUID = gepard::s_uid;
    gepard::Segment sBase(gepard::FloatPoint(-1.0, -2.2), gepard::FloatPoint(1.0, 3.9));
    gepard::Segment sTop = sBase.cutAndRemoveBottom(1);

    EXPECT_EQ(sBase.uid, currentUID + 1) << "";
    EXPECT_EQ(sBase.topX, -1.0) << "";
    EXPECT_EQ(sBase.topY, -3) << "";
    EXPECT_FLOAT_EQ(sBase.bottomX, 1.0 / 3.0) << "";
    EXPECT_EQ(sBase.bottomY, 1) << "";
    EXPECT_FLOAT_EQ(sBase.dx, 1.0 / 3.0) << "";
    EXPECT_EQ(sBase.direction, 1) << "";

    EXPECT_EQ(sTop.uid, currentUID + 1) << "";
    EXPECT_FLOAT_EQ(sTop.topX, 1.0 / 3.0) << "";
    EXPECT_EQ(sTop.topY, 1) << "";
    EXPECT_EQ(sTop.bottomX, 1.0) << "";
    EXPECT_EQ(sTop.bottomY, 3) << "";
    EXPECT_FLOAT_EQ(sTop.dx, 1.0 / 3.0) << "";
    EXPECT_EQ(sTop.direction, 1) << "";
}

TEST(SegmentTest, IntersectionY)
{
    gepard::Segment sBase(gepard::FloatPoint(20.0, 10.1), gepard::FloatPoint(30.0, 30.9));
    gepard::Segment sNext(gepard::FloatPoint(30.0, 10.9), gepard::FloatPoint(10.0, 30.1));
    gepard::Segment sParallel(gepard::FloatPoint(25.0, 10.2), gepard::FloatPoint(35.0, 30.8));

    EXPECT_FLOAT_EQ(sBase.dx, sParallel.dx) << "";
    int y = 0;
    EXPECT_FLOAT_EQ(sBase.intersectionY(sParallel, &y), false) << "";
    EXPECT_FLOAT_EQ(sBase.intersectionY(sNext, &y), true) << "";
    EXPECT_FLOAT_EQ(y, 16) << "";
    EXPECT_FLOAT_EQ(sParallel.intersectionY(sNext, &y), true) << "";
    EXPECT_FLOAT_EQ(y, 13) << "";
}

struct SegmentList {
    typedef gepard::BinaryTree<gepard::Segment> List;

    int y;
    List* list;

    SegmentList(const int y_, List* l = nullptr)
        : y(y_), list(l)
    {}
    ~SegmentList()
    {
        if (list) {
            delete list;
        }
    }

    friend const bool operator<(const SegmentList& lhs, const SegmentList& rhs)
    {
        return lhs.y < rhs.y;
    }

    SegmentList& operator=(SegmentList& other) = delete;
};

std::ostream& operator<<(std::ostream& os, const SegmentList& obj)
{
    os << obj.y << ":";
    if (obj.list) {
        for (auto& s : (*obj.list)) {
            os << " " << s;
        }
    }
    return os;
}

#define __REF 0
#if __REF
typedef std::set<SegmentList> SegmentConainer;
#else
typedef gepard::BinaryTree<SegmentList> SegmentConainer;
#endif

void displaySegments(SegmentConainer* segments)
{
    for (auto& sl : *segments) {
        std::cout << sl << std::endl;
    }
}

TEST(SegmentTest, IntersectionPoints)
{
    SegmentConainer segments;
    SegmentList segList(1, new SegmentList::List());
    for (int i = 0, xt = 0; i < 3; xt += ++i) {
        segList.list->multiInsert(gepard::Segment(gepard::FloatPoint(xt, 1.0), gepard::FloatPoint(-2 * i, 10.0)));
    }
    segments.uniqueInsert(segList);
    for (auto& segment : *(segments.begin()->list)) {
        std::cout << segment << std::endl;
    }
    std::cout << std::endl;

    for (auto& sl : segments) {
        SegmentList::List* list = sl.list;
        SegmentList::List::iterator it = list->begin();
        if (it == list->end())
            continue;
        SegmentList::List::iterator pit = it++;
        for (; it != list->end(); pit = it++) {
            int y;
            if (pit->intersectionY(*it, &y)) {
                SegmentList& nsl = *(segments.uniqueInsert(SegmentList(y)).it);
                std::cout << "y=" << y << " ps x s: " << *pit << " x " << *it << std::endl;
                if (!nsl.list) {
                    nsl.list = new SegmentList::List();
                    SegmentList& nnsl = *(segments.uniqueInsert(SegmentList(y + 1)).it);
                    if (!nnsl.list) {
                        nnsl.list = new SegmentList::List();
                    }
                    for (SegmentList::List::iterator sit = list->begin(); sit != list->end(); ++sit) {
                        gepard::Segment ns = (*sit).cutAndRemoveBottom(y);
                        gepard::Segment nns = ns.cutAndRemoveBottom(y + 1);
                        std::cout << "ns: " << ns << " nns: " << nns << std::endl;
                        nsl.list->multiInsert(std::move(ns));
                        nnsl.list->multiInsert(std::move(nns));
                    }
                }
                std::cout << std::endl;
                displaySegments(&segments);
//                segments.displayTree();
//                std::cout << std::endl;
//                for (auto& segment : *(segments.begin()->list)) {
//                    std::cout << segment << std::endl;
//                }
//                std::cout << std::endl;
//                for (auto& segment : *((++(segments.begin()))->list)) {
//                    std::cout << segment << std::endl;
//                }
//                std::cout << std::endl;
            }
        }
        list->displayTree();
        std::cout << std::endl;
    }
    for (auto& it : segments) {
        std::cout << it.y << std::endl;
    }
    std::cout << std::endl;
    for (auto& segment : *(segments.begin()->list)) {
        std::cout << segment << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::endl;
    for (auto& segment : *((++(segments.begin()))->list)) {
        std::cout << segment << std::endl;
    }
    std::cout << std::endl;


//        gepard::MultiSet<gepard::Segment>& ms = i->data().value;
//        for (gepard::MultiSet<gepard::Segment>::iterator it : ms) {
//            gepard::MultiSet<gepard::Segment>::iterator pit = it++;
//            if (it == ms.end())
//                break;
//            int y;
//            if (pit->data().intersectionY(it->data(), &y)) {
//                gepard::MultiSet<gepard::Segment>& nextms = segments.insert(y)->data().value;
//for (auto& j : segments) {
//    std::cout << j.data().key << ":" << &(j.data().value) << std::endl;
//}
//                EXPECT_EQ(nextms.size(), 0u);
//                EXPECT_EQ(ms.size(), 6u);
//                for (auto& s : ms) {
//std::cout << &(s.data()) << std::endl;
//gepard::Segment seg = s.data().cutAndRemoveBottom(y);
//std::cout << s.data() << std::endl;
//auto ss = nextms.insert(seg);
//std::cout << &(ss->data()) << std::endl;
//std::cout << &(s.data()) << std::endl;
//std::cout << s.data() << std::endl;
//                }
//                //EXPECT_EQ(ms.size(), nextms.size());
//for (auto& j : ms) {
//    std::cout << j.data() << std::endl;
//}
//            }
//        }
//    }
////    EXPECT_EQ(segments.size(), 5);
}

} // anonymous namespace

#endif // GEPARD_SEGMENT_TESTS_H
