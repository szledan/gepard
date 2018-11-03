# Copyright (C) 2018, Gepard Graphics
# Copyright (C) 2018, Dániel Bátyai <dbatyai@inf.u-szeged.hu>
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

cmake_minimum_required(VERSION 2.8.10)

set(THIRDPARTY_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/bin)
set(THIRDPARTY_LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)
set(THIRDPARTY_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(THIRDPARTY_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(THIRDPARTY_BUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/build)
set(THIRDPARTY_INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR})

# The outer build system will look in these folders for required files
set(THIRDPARTY_BINARY_DIR ${THIRDPARTY_BINARY_DIR} PARENT_SCOPE)
set(THIRDPARTY_LIB_DIR ${THIRDPARTY_LIB_DIR} PARENT_SCOPE)
set(THIRDPARTY_INCLUDE_DIR ${THIRDPARTY_INCLUDE_DIR} PARENT_SCOPE)

# When creating a build target, the following general rules apply:
# - Source code should be fetched into a subdirectory of the 'src' directory.
# - Created output files should be installed into their respective directories
#   defined above this comment block.
# - A subdirectory in PROJECT_BUILD_DIR can be used as a build directory.
# - Stamp files should be stored in the same build directory.

include(ExternalProject)

ExternalProject_Add(gtest
                    PREFIX ${THIRDPARTY_SRC_DIR}/googletest
                    URL https://github.com/google/googletest/archive/release-1.8.1.tar.gz
                    DOWNLOAD_DIR ${THIRDPARTY_SRC_DIR}/googletest
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR} -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_INCLUDEDIR=include -DBUILD_GMOCK=OFF
                    BINARY_DIR ${THIRDPARTY_BUILD_DIR}/googletest
                    STAMP_DIR ${THIRDPARTY_BUILD_DIR}/googletest/stamp
                    BUILD_COMMAND make gtest
                    INSTALL_COMMAND make install > /dev/null)

ExternalProject_Add(glslang
                    PREFIX ${THIRDPARTY_SRC_DIR}/glslang
                    URL https://github.com/KhronosGroup/glslang/archive/7.9.2888.tar.gz
                    DOWNLOAD_DIR ${THIRDPARTY_SRC_DIR}/glslang
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_INSTALL_DIR} -DCMAKE_INSTALL_BINDIR=bin
                    BINARY_DIR ${THIRDPARTY_BUILD_DIR}/glslang
                    STAMP_DIR ${THIRDPARTY_BUILD_DIR}/glslang/stamp
                    BUILD_COMMAND make glslangValidator
                    INSTALL_COMMAND make install > /dev/null)

add_custom_command(OUTPUT ${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan.h
                   COMMAND mkdir -p "${THIRDPARTY_INCLUDE_DIR}/vulkan"
                   COMMAND wget -O "${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan.h"
                                   "https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v1.1.90/include/vulkan/vulkan.h")

add_custom_command(OUTPUT ${THIRDPARTY_INCLUDE_DIR}/vulkan/vk_platform.h
                   COMMAND mkdir -p "${THIRDPARTY_INCLUDE_DIR}/vulkan"
                   COMMAND wget -O "${THIRDPARTY_INCLUDE_DIR}/vulkan/vk_platform.h"
                                   "https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v1.1.90/include/vulkan/vk_platform.h")

add_custom_command(OUTPUT ${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan_core.h
                   COMMAND mkdir -p "${THIRDPARTY_INCLUDE_DIR}/vulkan"
                   COMMAND wget -O "${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan_core.h"
                                   "https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v1.1.90/include/vulkan/vulkan_core.h")

add_custom_command(OUTPUT ${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan_xcb.h
                   COMMAND mkdir -p "${THIRDPARTY_INCLUDE_DIR}/vulkan"
                   COMMAND wget -O "${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan_xcb.h"
                                   "https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v1.1.90/include/vulkan/vulkan_xcb.h")

add_custom_command(OUTPUT ${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan_xlib.h
                   COMMAND mkdir -p "${THIRDPARTY_INCLUDE_DIR}/vulkan"
                   COMMAND wget -O "${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan_xlib.h"
                                   "https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/v1.1.90/include/vulkan/vulkan_xlib.h")

add_custom_target(vulkan_headers
                  DEPENDS ${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan.h
                          ${THIRDPARTY_INCLUDE_DIR}/vulkan/vk_platform.h
                          ${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan_core.h
                          ${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan_xcb.h
                          ${THIRDPARTY_INCLUDE_DIR}/vulkan/vulkan_xlib.h)
