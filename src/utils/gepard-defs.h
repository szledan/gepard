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

namespace gepard {

#ifdef GD_ASSERT
#undef GD_ASSERT
#endif
#define GD_ASSERT(...) assert(__VA_ARGS__);

#ifdef GD_CRASH
#undef GD_CRASH
#endif
#define GD_CRASH(X) \
    do { \
        std::cerr << X << std::endl; \
        abort(); \
    } while (false)

#ifdef GD_NOT_IMPLEMENTED
#undef GD_NOT_IMPLEMENTED
#endif
#define GD_NOT_IMPLEMENTED(...) GD_ASSERT(0 && "Unimplemented function!")

#ifdef GD_LOG1
#undef GD_LOG1
#endif
#ifdef GD_LOG2
#undef GD_LOG2
#endif
#ifdef GD_LOG3
#undef GD_LOG3
#endif
#ifdef GD_LOG_ERR
#undef GD_LOG_ERR
#endif
#ifdef GD_LOG_FUNC
#undef GD_LOG_FUNC
#endif

#ifdef GD_LOG_LEVEL

#ifndef GD_DISABLE_LOG_COLORS
#define GD_LOG1_COLOR "\033[33m"
#define GD_LOG2_COLOR "\033[94m"
#define GD_LOG3_COLOR "\033[34m"
#define GD_LOG_ERR_COLOR "\033[41m"
#define GD_LOG_FUNC_COLOR "\033[2m"
#define GD_CLEAR_COLOR "\033[39m \033[22m \033[49m"
#else /* !GD_DISABLE_LOG_COLORS */
#define GD_LOG1_COLOR ""
#define GD_LOG2_COLOR ""
#define GD_LOG3_COLOR ""
#define GD_LOG_ERR_COLOR ""
#define GD_LOG_FUNC_COLOR ""
#define GD_CLEAR_COLOR ""
#endif

#define GD_NORMAL_LOG 0
#define GD_ERROR_LOG 1
#define GD_FUNCTION_LOG 2

#define GD_LOG1(MSG) _GD_LOG(1, MSG, GD_LOG1_COLOR, GD_NORMAL_LOG)
#define GD_LOG2(MSG) _GD_LOG(2, MSG, GD_LOG2_COLOR, GD_NORMAL_LOG)
#define GD_LOG3(MSG) _GD_LOG(3, MSG, GD_LOG3_COLOR, GD_NORMAL_LOG)
#define GD_LOG_ERR(MSG) _GD_LOG(1, MSG, GD_LOG_ERR_COLOR, GD_ERROR_LOG)
#define GD_LOG_FUNC(MSG) _GD_LOG(4, MSG, GD_LOG_FUNC_COLOR, GD_FUNCTION_LOG)

#define _GD_LOG(LEVEL, MSG, LOG_COLOR, TYPE) do {\
  std::ostringstream os; \
  if (TYPE == GD_FUNCTION_LOG) { \
    os << "[FUNC] " << std::string(__func__); \
  } else if (TYPE == GD_ERROR_LOG) { \
    os << "[ERR] "; \
  } else if (TYPE == GD_NORMAL_LOG) { \
    os << "[" << LEVEL << "] "; \
  } else { \
    GD_ASSERT(false && "Undefined log type."); \
  } \
  os << MSG; \
  GD_ASSERT(LEVEL > 0); \
  _log(LEVEL - 1, os.str(), LOG_COLOR, TYPE); \
} while(false)

/*!
 * \brief Logging helper function.
 * \param level  defines the depth of logging
 * \param msg  log message
 * \param color  color define with escape sequence
 * \param type  defines 'normal', 'error' or 'function name' logging
 *
 * \internal
 */
void _log(unsigned int level, const std::string& msg, const std::string& color, const int type);

#else /* !GD_LOG_LEVEL */

#define GD_LOG1(MSG)
#define GD_LOG2(MSG)
#define GD_LOG3(MSG)
#define GD_LOG_ERR(MSG)
#define GD_LOG_FUNC(MSG)

#endif /* GD_LOG_LEVEL */

} // namespace gepard

#endif // GEPARD_DEFS_H
