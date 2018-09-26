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

#ifndef NANOARGPARSE_H
#define NANOARGPARSE_H

#define INIT_ARGS(ARGC, ARGV) \
    struct _AP {\
        int argc; char** argv; int* touched;\
        ~_AP(){ free(touched); }\
    } _ap = { ARGC, ARGV, (int*)calloc(ARGC, sizeof(int)) };

#define CHECK_FLAG(FLAGS) [&](){\
    char flags[] = FLAGS;\
    char* f = strtok(flags, ",");\
    while(f) {\
        while(isspace(*f)) f++;\
        for(int i = 1; i < _ap.argc;++i) {\
            if (!strcmp(_ap.argv[i], f)) {\
                _ap.touched[i]++;\
                return i;\
            }\
        }\
        f = strtok(NULL, ",");\
    }\
    return 0;\
}()

#define GET_VALUE(FLAGS, DEFAULT) [&](){\
    int n = -2;\
    if (strlen(FLAGS)) {\
        int f = CHECK_FLAG(FLAGS);\
        n = f ? f : -1;\
    };\
    if (n == -2) {\
        for (int i = 1; i < _ap.argc; ++i) {\
            if (!_ap.touched[i]) {\
                n = i - 1;\
                break;\
            }\
        }\
    }\
    if (n > -1 && (++n) < _ap.argc) {\
        _ap.touched[n]++;\
        return (const char*)_ap.argv[n];\
    }\
    return DEFAULT;\
}()

#endif // NANOARGPARSE_H
