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
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#ifndef GEPARD_TRAPEZOID_CONTAINER_H
#define GEPARD_TRAPEZOID_CONTAINER_H

#include "gepard-defs.h"
#include "gepard-segment.h"
#include "gepard-binary-trees.h"
#include "gepard-job-scheduler.h"
#include <mutex>
#include <thread>

namespace gepard {

class TrapezoidContainer {
public:
    TrapezoidContainer()
    {
    }
    ~TrapezoidContainer()
    {
//        _yJobs.waitForJobs();
//        _xJobs[0].waitForJobs();
//        _xJobs[1].waitForJobs();
//        _runner.waitForJobs();
    }

    void addSegment(const Segment& segment)
    {
//        _yMutex.lock();
//        _yJobs.addJob(std::bind(&TrapezoidContainer::addYJob, this, segment));
//        _yMutex.unlock();
    }
    void horizontalMergeTrapezoids(){}
    void verticalMergeTrapezoids(){}

private:
    void addYJob(const Segment& segment)
    {
//        const int topY = segment.topY();
//        auto bit = _segments.insert(segment.topY());
//        auto eit = _segments.insert(segment.bottomY());
//        _segments.displayList();
//        Segment s(segment.from(), segment.to(), segment.uid(), segment.slope());
//        do {
//            auto cit = bit++;
//            Segment cs = s.split(bit->key);
//            _xJobs[std::abs(topY % 2)].addJob(std::bind(&TrapezoidContainer::insertSegment, this, &(cit->value), cs));
//        } while (bit != eit);
    }
    void splitXSet(const int y, MultiSet<Segment>* xSet1, MultiSet<Segment>* xSet2)
    {
    }
    void insertSegment(MultiSet<Segment>* xSet, const Segment& segment)
    {
//        xSet->insert(segment);
//        std::cout << segment.topY() << std::endl;
    }

//    JobRunner _runner;
//    JobScheduler _yJobs = JobScheduler(_runner);
//    std::mutex _yMutex;

//    JobRunner _xRunners[2];
//    JobScheduler _xJobs[2] = { JobScheduler(_xRunners[0]), JobScheduler(_xRunners[1]) };
//    Map<int, MultiSet<Segment>> _segments;
};
} // namespace gepard

#endif // GEPARD_TRAPEZOID_CONTAINER_H
