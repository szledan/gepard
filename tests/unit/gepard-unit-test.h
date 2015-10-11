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

struct TestContext {
    int globalPASS = 0;
    int globalFAIL = 0;
    int testcaseIndex = 0;
    bool hasFail = false;
    bool logOn = true;
    std::ostringstream failLog, checkLog;
};

TestContext g_testCtx;

#define TEST(FUNCTION) do { \
        std::cout << #FUNCTION << "..."; \
        g_testCtx.testcaseIndex = 0; \
        g_testCtx.hasFail = false; \
        g_testCtx.failLog.str(""); \
        g_testCtx.checkLog.str(""); \
        FUNCTION; \
        if (g_testCtx.hasFail) \
            std::cout << "FAIL" << std::endl << g_testCtx.failLog.str(); \
        else \
            std::cout << "PASS" << std::endl; \
    } while (0)

#define FINALIZE() do { \
        int globalAll = g_testCtx.globalPASS + g_testCtx.globalFAIL; \
        if (globalAll > 0) \
            std::cout << std::endl \
                << "  === Finished. ===" << std::endl \
                << "   Passed: " << g_testCtx.globalPASS << " (" << g_testCtx.globalPASS / ((float)globalAll) * 100 << "%)" << std::endl \
                << "   Failed: " << g_testCtx.globalFAIL << " (" << g_testCtx.globalFAIL / ((float)globalAll) * 100 << "%)" << std::endl \
                << "   Total cases: " << globalAll << std::endl << std::endl; \
    } while (0)

#define EXIT_CODE() (g_testCtx.globalFAIL ? 1 : 0)

#define HASFAIL (g_testCtx.hasFail)

#define TESTCASE(OP, FAILMSG) do { \
        g_testCtx.testcaseIndex++; \
        if (OP) { \
            g_testCtx.globalPASS++; \
        } else { \
            g_testCtx.failLog << g_testCtx.testcaseIndex << ". testcase: " << FAILMSG << std::endl; \
            if (g_testCtx.checkLog.str() != "") { \
                g_testCtx.failLog << g_testCtx.checkLog.str(); \
            } \
            g_testCtx.globalFAIL++; \
            g_testCtx.hasFail = true; \
        } \
        g_testCtx.checkLog.str(""); \
    } while (0)

#define NTESTCASE(OP, FAILMSG) TESTCASE(!(OP), FAILMSG)

bool checkAndLog(bool op, std::ostream& FAILMSG)
{
    if (!op && g_testCtx.logOn) {
        g_testCtx.checkLog << dynamic_cast<std::ostringstream&>(FAILMSG).str() << std::endl;
    }
    return op;
}

#define CHECKLOGMSG(OP, FAILMSG) "  " << __LINE__ << ": (" << (#OP) << ") " << FAILMSG

#define CHECKANDMSG(OP, FAILMSG) checkAndLog(OP, std::ostringstream() << CHECKLOGMSG(OP, FAILMSG))
#define NCHECKANDMSG(OP, FAILMSG) !(CHECKANDMSG(!(OP), FAILMSG))
#define CHECKANDLOG(OP) CHECKANDMSG(OP, "")
#define NCHECKANDLOG(OP) !(CHECKANDLOG(!(OP)))

#define EQ(A, B) (A == B)
#define NEQ(A, B) (A != B)

#endif // gepard_unit_test_h
