/* Copyright (C) 2015-2016, Gepard Graphics
 * Copyright (C) 2016, Peter Gal <galpeter@inf.u-szeged.hu>
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

#include "gepard-defs.h"
#include <iostream>
#include <sstream>
#include <string>

namespace gepard {

#ifdef GD_LOG_LEVEL

/*!
 * \brief Logging helper function.
 * \param level  defines the depth of logging
 * \param msg  log message
 * \param color  color define with escape sequence
 * \param type  defines 'normal', 'error' logging
 * \param file  filename where calls log
 * \param line  line where calls log
 * \param function  function name where calls log
 */
void _log(unsigned int level, const std::string& msg, const std::string& color, const int type, const std::string& file, const int line, const std::string& function)
{
    std::ostringstream oss;
    std::ostream& os = (type == (GD_ERROR_LOG)) ? std::cerr : std::cout;
    if (type == (GD_ERROR_LOG)) {
        oss << "[ERR] ";
    } else if (type == (GD_NORMAL_LOG)) {
        oss << "[" << level + 1 << "] ";
    } else {
        GD_ASSERT(false && "Undefined log type.");
    }
    if (level < (GD_LOG_LEVEL)) {
        oss  << msg;
        os << color << oss.str() << GD_LOG4_COLOR << "  (in " << function << "() at "<< file.substr(file.rfind("/") + 1) << ":" << line << ")" << GD_CLEAR_COLOR << std::endl;
    }
}

#endif /* GD_LOG_LEVEL */

} // namespace gepard
