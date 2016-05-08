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
 */

#ifndef gepard_unit_test_h
#define gepard_unit_test_h

#include <iostream>

int globalPASS = 0;
int globalFAIL = 0;
int testcaseIndex = 0;

#define TESTER(FUNCTION) do { \
        std::cout << #FUNCTION << " START" << std::endl; \
        testcaseIndex = 0; \
        FUNCTION; \
        std::cout << #FUNCTION << " STOP" << std::endl; \
    } while (0)

#define FINALIZER() do { \
        int globalAll = globalPASS + globalFAIL; \
        if (globalAll > 0) \
            std::cout << std::endl \
                << "  === Finished. ===" << std::endl \
                << "   Passed: " << globalPASS << " (" << globalPASS / ((float)globalAll) * 100 << "%)" << std::endl \
                << "   Failed: " << globalFAIL << " (" << globalFAIL / ((float)globalAll) * 100 << "%)" << std::endl \
                << "   Total cases: " << globalAll << std::endl << std::endl; \
    } while (0)

#define EXIT_CODE() (globalFAIL ? 1 : 0)

#define TEST(OP, M) do { \
        std::cout << testcaseIndex++ << ". testcase..."; \
        if (OP) { \
            std::cout << "PASS";\
            globalPASS++; \
        } else { \
            std::cout << "FAIL" << std::endl << M; \
            globalFAIL++; \
        } \
        std::cout << std::endl; \
    } while (0)

#define EQ(A, B) (A == B)
#define NEQ(A, B) (A != B)

#endif // gepard_unit_test_h
