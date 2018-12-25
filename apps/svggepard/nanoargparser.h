/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2018, Szilard Ledan <szledan@gmail.com>
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

#ifndef NANOARGPARSER_H
#define NANOARGPARSER_H

#include <cstring>
#include <sstream>
#include <vector>

namespace nanoargparser {

struct NanoArgParser {
    NanoArgParser(int argc, char* argv[]) : _argc(argc), _argv(argv), _touched(std::vector<int>(argc)) {}
    const bool hasFlag(const char* flags) { return flagPos(flags) != -1; }
    const int flagPos(const char* flags_)
    {
        char* flags = new char[strlen(flags_)];
        std::strcpy(flags, flags_);
        const char delimiters[] = ", ;|\t";
        char* flag = std::strtok(flags, delimiters);
        int pos = -1;
        while (flag && pos == -1) {
            for (int i = 1; pos == -1 && i < _argc; ++i) {
                if (!std::strcmp(_argv[i], flag)) {
                    _touched[i]++;
                    pos = i;
                }
            }
            flag = strtok(NULL, delimiters);
        }
        delete[] flags;
        return pos;
    }
    static const std::string basename(const std::string& path, const std::string& delimiters = "/") { return path.substr(path.find_last_of(delimiters) + 1); }
    const std::string getValue(const char* flags, const char* defValue) { return getValue(flags, std::string(defValue)); }
    template<class T> const T getValue(const char* flags, const T& defValue)
    {
        int pos = getPos(flags);
        T retValue = defValue;
        if (pos > -1 && (++pos) < _argc) {
            _touched[pos]++;
            std::stringstream ss(std::string(_argv[pos]));
            ss >> retValue;
        }
        return retValue;
    }

private:
    int _argc;
    char** _argv;
    std::vector<int> _touched;
    const int getPos(const char* flags)
    {
        int pos = strlen(flags) ? flagPos(flags) : -2;
        for (int i = 1; pos == -2 && i < _argc; ++i) {
            if (!_touched[i]) {
                pos = i - 1;
            }
        }
        return pos;
    }
};

} // namespace nanoargparser

#endif // NANOARGPARSER_H
