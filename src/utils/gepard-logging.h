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

#ifndef GEPARD_LOGGING_H
#define GEPARD_LOGGING_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace gepard {

enum LogLevel {
    NONE = 0,
    ERROR = 1,
    WARNING = 2,
    INFO = 3,
    DEBUG = 4,
    TRACE = 5,
    _MAX_LOG_LEVEL=TRACE
};

namespace logging {

#ifdef GD_LOGGING

const bool shouldLog(const LogLevel level);
std::ostream& logOutputStream();

const char* logPrefix(const LogLevel level);
const char* logColor(const LogLevel level);
const char* clearColor();

class LogSettings {
public:
    LogSettings()
      : _logLevel(0), _logOutputStream(nullptr)
    {
        if (const char* levelStr = std::getenv("GEPARD_LOG_LEVEL")) {
            _logLevel = std::stoi(std::string(levelStr));
        }

        if (const char* file = std::getenv("GEPARD_LOG_FILE")) {
            _logFile.open(file);
            _logOutputStream.rdbuf(_logFile.rdbuf());
        } else {
            _logOutputStream.rdbuf(std::cout.rdbuf());
        }
    }

    const bool shouldLog(const LogLevel level) const { return (const int) level <= _logLevel; }
    const bool logToFile() const { return _logFile.is_open(); }
    std::ostream& logOutputStream() { return _logOutputStream; }

private:
    int _logLevel;
    std::ofstream _logFile;
    std::ostream _logOutputStream;
};

class LogMessage {
public:
    LogMessage(const LogLevel level, const char* file, int line)
      : _os(), _line(line)
    {
        const std::string fileStr(file);
        _file = std::move(fileStr.substr(fileStr.rfind("/") + 1));
        _os << logColor(level) << logPrefix(level) << " ";
    }

    ~LogMessage()
    {
        _os << "    (" << _file << ":" << _line << ")" << clearColor();
        logOutputStream() << _os.str() << std::endl;
    }

    template <typename T>
    LogMessage& operator<<(T&& item)
    {
        _os << std::forward<T>(item);
        return *this;
    }

    LogMessage& operator<<(std::ostream& (*manip)(std::ostream&))
    {
        _os << manip;
        return *this;
    }

private:
    std::ostringstream _os;
    std::string _file;
    int _line;
};

#define MESSAGE(level) logging::LogMessage(level, __FILE__, __LINE__)
#define GD_LOG(level) if (!(logging::shouldLog(level))) {} else MESSAGE(level)

#else /* !GD_LOGGING */

class LogMessage {
public:
    LogMessage() = default;

    template <typename T>
    LogMessage& operator<<(T&& item)
    {
        return *this;
    }

    LogMessage& operator<<(std::ostream& (*manip)(std::ostream&))
    {
        return *this;
    }
};

#define GD_LOG(level) if (true) {} else logging::LogMessage()

#endif /* GD_LOGGING */

} // namespace logging
} // namespace gepard

#endif // GEPARD_LOGGING_H
