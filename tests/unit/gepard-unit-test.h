/* Copyright (C) 2015, Szilard Ledan <szledan@gmail.com>
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

#ifndef gepard_unit_test_h
#define gepard_unit_test_h

#include <iostream>
#include <string>

int globalPASS = 0;
int globalFAIL = 0;
int testcaseIndex = 0;
bool hasFail = false;
bool logOn = true;
std::ostringstream failLog;

#define TEST(FUNCTION) do { \
        std::cout << #FUNCTION << "..."; \
        testcaseIndex = 0; \
        hasFail = false; \
        failLog.str(""); \
        FUNCTION; \
        if (hasFail) \
            std::cout << "FAIL" << std::endl << failLog.str(); \
        else \
            std::cout << "PASS" << std::endl; \
    } while (0)

#define FINALIZE() do { \
        int globalAll = globalPASS + globalFAIL; \
        if (globalAll > 0) \
            std::cout << std::endl \
                << "  === Finished. ===" << std::endl \
                << "   Passed: " << globalPASS << " (" << globalPASS / ((float)globalAll) * 100 << "%)" << std::endl \
                << "   Failed: " << globalFAIL << " (" << globalFAIL / ((float)globalAll) * 100 << "%)" << std::endl \
                << "   Total cases: " << globalAll << std::endl << std::endl; \
    } while (0)

#define EXIT_CODE() (globalFAIL ? 1 : 0)

#define TESTPREMSG "  " << testcaseIndex++ << ". testcase: " << " FAIL (Line: " << __LINE__ << ") "
#define HASFAIL (hasFail)

#define TESTCASE(OP, MSG) do { \
        if (OP) { \
            globalPASS++; \
        } else { \
            failLog << TESTPREMSG << MSG << std::endl; \
            globalFAIL++; \
            hasFail = true; \
        } \
    } while (0)

bool checkAndLog(bool op, std::ostream& msg)
{
    if (!op && logOn) {
        failLog << dynamic_cast<std::ostringstream&>(msg).str() << std::endl;
        hasFail = true;
    }
    return op;
}

#define CHECKANDMSG(OP, MSG) checkAndLog(OP, std::ostringstream() << TESTPREMSG << (#OP) << " " << MSG)
#define NCHECKANDMSG(OP, MSG) !(CHECKANDMSG(!(OP), MSG))
#define CHECKANDLOG(OP) checkAndLog(OP, std::ostringstream() << TESTPREMSG << #OP)
#define NCHECKANDLOG(OP) !(CHECKANDLOG(!(OP)))

#define EQ(A, B) (A == B)
#define NEQ(A, B) (A != B)

#endif // gepard_unit_test_h
