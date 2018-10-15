/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2018, Dániel Bátyai <dbatyai@inf.u-szeged.hu>
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

#include "gepard-logging.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace gepard {
namespace logging {

#ifdef GD_LOGGING

LogSettings logSettings;

/*!
 * \brief  Decides whether we should log with the specified log level
 * \param level  Log level to check
 * \return  true if we should log with the given log level
 */
const bool shouldLog(const LogLevel level)
{
    return logSettings.shouldLog(level);
}

/*!
 * \brief Getter for the current output stream
 * \return The stream that is used to log messages
 */
std::ostream& logOutputStream()
{
    return logSettings.logOutputStream();
}

/*!
 * \brief  Determines the prefix that should be used for the given log level
 * \param level  The used log level
 * \return  The corresponding prefix string for the given log level
 */
const char* logPrefix(const LogLevel level)
{
    static const char prefixError[] = "[ERROR]";
    static const char prefixWarning[] = "[WARNING]";
    static const char prefixInfo[] = "[INFO]";
    static const char prefixDebug[] = "[DEBUG]";
    static const char prefixTrace[] = "[TRACE]";
    static const char prefixNone[] = "";

    switch (level) {
        case ERROR:
            return prefixError;
        case WARNING:
            return prefixWarning;
        case INFO:
            return prefixInfo;
        case DEBUG:
            return prefixDebug;
        case TRACE:
            return prefixTrace;
        default:
            return prefixNone;
    }
}

/*!
 * \brief  Determines the color escape sequence for a given log level
 * \param level  The used log level
 * \return  The color escape sequence string corresponding to the given log level
 */
const char* logColor(const LogLevel level)
{
    static const char colorRed[] = "\033[31m";
    static const char colorYellow[] = "\033[33m";
    static const char colorDefault[] = "\033[39m";
    static const char colorLightBlue[] = "\033[94m";
    static const char colorBlue[] = "\033[34m";
    static const char colorNone[] = "";

    if (logSettings.logToFile()) {
        return colorNone;
    }

    switch (level) {
        case ERROR:
            return colorRed; // Red
        case WARNING:
            return colorYellow; // Yellow
        case INFO:
            return colorDefault; // Default
        case DEBUG:
            return colorLightBlue; // LightBlue
        case TRACE:
            return colorBlue; // Blue
        default:
            return colorNone;
    }
}

/*!
 * \brief  Gives the escape sequence string that clears the terminal colors
 * \return  Escape sequence string that clears terminal colors
 */
const char* clearColor()
{
    static const char colorClear[] = "\033[0m";
    static const char colorNone[] = "";

    return logSettings.logToFile() ? colorNone : colorClear;
}

#endif /* GD_LOGGING */

} // namespace logging
} // namespace gepard
