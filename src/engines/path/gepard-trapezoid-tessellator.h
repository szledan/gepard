/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2015-2018, Szilard Ledan <szledan@gmail.com>
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
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#ifndef GEPARD_TRAPEZOID_TESSELLATOR_H
#define GEPARD_TRAPEZOID_TESSELLATOR_H

#include "gepard-bounding-box.h"
#include "gepard-defs.h"
#include "gepard-float-point.h"
#include "gepard-float.h"
#include "gepard-line-types.h"
#include "gepard-path.h"
#include "gepard-state.h"
#include <list>

#include <functional>
#include <queue>
#include <thread>
#include <vector>

namespace gepard {

/* Trapezoid */

struct Trapezoid {
    const bool isMergableInTo(const Trapezoid* other) const;

    Float topY;
    Float topLeftX;
    Float topRightX;
    Float bottomY;
    Float bottomLeftX;
    Float bottomRightX;

    unsigned leftId;
    unsigned rightId;
    Float leftSlope;
    Float rightSlope;
};

inline std::ostream& operator<<(std::ostream& os, const Trapezoid& t)
{
    return os << t.topY << "," << t.topLeftX << "," << t.topRightX << "," << t.bottomY << "," << t.bottomLeftX << "," << t.bottomRightX;
}

inline bool operator<(const Trapezoid& lhs, const Trapezoid& rhs);
inline bool operator==(const Trapezoid& lhs, const Trapezoid& rhs);
inline bool operator<=(const Trapezoid& lhs, const Trapezoid& rhs);

/* TrapezoidList */

typedef std::list<Trapezoid> TrapezoidList;

/* TrapezoidTessellator */

class TrapezoidTessellator {
public:
    enum FillRule {
        EvenOdd,
        NonZero,
    };

    TrapezoidTessellator(PathData&, FillRule = NonZero, int antiAliasingLevel = GD_ANTIALIAS_LEVEL);

    const TrapezoidList trapezoidList(const GepardState& state);

    const FillRule fillRule() const { return _fillRule; }
    const BoundingBox& boundingBox() const { return _boundingBox; }
    const int antiAliasingLevel() const { return _antiAliasingLevel; }

private:
    PathData& _pathData;
    const FillRule _fillRule;
    const int _antiAliasingLevel;

    BoundingBox _boundingBox;
};

template<const uint8_t WORKER_COUNT = 1 + 8>
class TrapezoidBuilder {
public:
    TrapezoidBuilder()
        : yWorker(yWorks)
    {
        workers.reserve(WORKER_COUNT);
        for (uint8_t t = 0; t < WORKER_COUNT; ++t) {
            workers[t] = std::thread();
        }
    }
    ~TrapezoidBuilder()
    {
        yWorker.join();
    }
private:
    struct Scheduler {
        struct Job {
            Job(std::function<void()> func) : boundFunc(func) {}
            void run()
            {
                GD_ASSERT(boundFunc != nullptr);
                boundFunc();
            }

            std::function<void()> boundFunc;
        };

        void addJob(std::function<void()> func)
        {
            ;
        }
    };

    void yWorks()
    {
        while ();
    }

    std::thread yWorker;
    std::queue<std::function<void()>> yJobs;
    std::vector<std::thread> xWorkers;
};

} // namespace gepard

#endif // GEPARD_TRAPEZOID_TESSELLATOR_H
