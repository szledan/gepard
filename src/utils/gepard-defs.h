/* Copyright (C) 2015-2016, Gepard Graphics
 * Copyright (C) 2015-2016, Szilard Ledan <szledan@gmail.com>
 * Copyright (C) 2015-2016, Dániel Bátyai <dbatyai@inf.u-szeged.hu>
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

#ifndef GEPARD_DEFS_H
#define GEPARD_DEFS_H

#include <assert.h>
#include <iostream>
#include <sstream>

#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(...) assert(__VA_ARGS__);

#ifdef NOT_IMPLEMENTED
#undef NOT_IMPLEMENTED
#endif
#define NOT_IMPLEMENTED(...) ASSERT(0 && "Unimplemented function!")

#ifdef LOG_LEVEL
#ifndef DISABLE_LOG_COLORS
#define LOG1_COLOR "\033[94m"
#define LOG2_COLOR "\033[36m"
#define LOG3_COLOR "\033[93m"
#define CLEAR_COLOR "\033[39m"
#else
#define LOG1_COLOR ""
#define LOG2_COLOR ""
#define LOG3_COLOR ""
#define CLEAR_COLOR ""
#endif

#ifdef LOG1
#undef LOG1
#endif
#define LOG1(MSG) do \
{ \
    std::ostringstream os; \
    os << LOG1_COLOR << MSG << CLEAR_COLOR; \
    _log(1, os.str()); \
} while (false)


#ifdef LOG2
#undef LOG2
#endif
#define LOG2(MSG) do \
{ \
    std::ostringstream os; \
    os << LOG2_COLOR << MSG << CLEAR_COLOR; \
    _log(2, os.str()); \
} while (false)


#ifdef LOG3
#undef LOG3
#endif
#define LOG3(MSG) do \
{ \
    std::ostringstream os; \
    os << LOG3_COLOR << MSG << CLEAR_COLOR; \
    _log(3, os.str()); \
} while (false)

void _log(int level, std::string msg) {
  const int max_level = LOG_LEVEL;
  if (level <= max_level) {
    std::cout << msg << std::endl;
  }
}

#else /* LOG_LEVEL */
#ifdef LOG1
#undef LOG1
#endif
#ifdef LOG2
#undef LOG2
#endif
#ifdef LOG3
#undef LOG3
#endif

#define LOG1(MSG)
#define LOG2(MSG)
#define LOG3(MSG)

#endif /* LOG_LEVEL */

namespace gepard {

} // namespace gepard

#endif // GEPARD_DEFS_H
