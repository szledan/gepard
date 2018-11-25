# Copyright (C) 2016-2018, Gepard Graphics
# Copyright (C) 2016-2018, Szilard Ledan <szledan@gmail.com>
# Copyright (C) 2016-2018, Dániel Bátyai <dbatyai@inf.u-szeged.hu>
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set(SOURCES
    gepard-unit-main.cpp
    ${PROJECT_SOURCE_DIR}/src/engines/gepard-path.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-bounding-box.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-float-point.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-logging.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-transform.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-vec4.cpp
)

set(COMMON_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/src/utils
    ${PROJECT_SOURCE_DIR}/src/engines
)

add_executable(unittest ${SOURCES})

# Pthread required by gtest
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

target_include_directories(unittest PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/include)
target_include_directories(unittest PUBLIC ${COMMON_INCLUDE_DIRS})

add_dependencies(unittest gtest)
target_link_libraries(unittest ${THIRDPARTY_LIB_DIR}/libgtest.a ${CMAKE_THREAD_LIBS_INIT})
