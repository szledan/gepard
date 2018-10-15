# Copyright (C) 2015-2016, Gepard Graphics
# Copyright (C) 2015-2016, Péter Gál
# Copyright (C) 2015-2018, Szilard Ledan <szledan@gmail.com>
# Copyright (C) 2016, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
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

set(COMMON_SOURCES
    engines/gepard-context.cpp
    engines/gepard-path.cpp
    engines/gepard-stroke-builder.cpp
    engines/gepard-trapezoid-tessellator.cpp
    gepard.cpp
    gepard-engine.cpp
    gepard-image.cpp
    utils/gepard-bounding-box.cpp
    utils/gepard-color.cpp
    utils/gepard-float-point.cpp
    utils/gepard-line-types.cpp
    utils/gepard-logging.cpp
    utils/gepard-transform.cpp
    utils/gepard-vec4.cpp
)

set(GLES2_SOURCES
    engines/gles2/gepard-gles2.cpp
    engines/gles2/gepard-gles2-fill-path.cpp
    engines/gles2/gepard-gles2-fill-rect.cpp
    engines/gles2/gepard-gles2-shader-factory.cpp
    engines/gles2/gepard-gles2-stroke-path.cpp
)

set(VULKAN_SOURCES
    engines/vulkan/gepard-vulkan-interface.cpp
    engines/vulkan/gepard-vulkan.cpp
)

set(SOFTWARE_SOURCES
    engines/software/gepard-software.cpp
)

set(COMMON_INCLUDE_DIRS
    .
    engines
    surfaces
    utils
)

set(GLES2_INCLUDE_DIRS
    engines/gles2
)

set(VULKAN_INCLUDE_DIRS
    engines/vulkan
    ${PROJECT_BINARY_DIR}/spirv
)

set(SOFTWARE_INCLUDE_DIRS
    engines/software
)

set(GEPARD_HEADERS
    gepard.h
    gepard-image.h
)

set(SURFACE_HEADERS
    surfaces/gepard-png-surface.h
    surfaces/gepard-xsurface.h
    surfaces/gepard-memory-buffer-surface.h
)

set(SOURCES ${COMMON_SOURCES})
list(APPEND SOURCES ${${BACKEND}_SOURCES})

add_library(gepard SHARED ${SOURCES})

if (BACKEND STREQUAL "VULKAN")
  include(VulkanShaders)
  add_dependencies(gepard vulkan_shaders)
  if (NOT VULKAN_FOUND)
    add_dependencies(gepard vulkan_headers)
    target_include_directories(gepard PRIVATE ${THIRDPARTY_INCLUDE_DIR})
  endif()
endif()

target_compile_definitions(gepard PRIVATE "GD_USE_${BACKEND}")

target_include_directories(gepard PRIVATE ${COMMON_INCLUDE_DIRS})
target_include_directories(gepard PRIVATE ${${BACKEND}_INCLUDE_DIRS})
target_include_directories(gepard PRIVATE ${GEPARD_DEP_INCLUDES})

# TODO(dbatyai): Add a target to do this instead of doing it at configure time
file(COPY gepard.h DESTINATION ${INCLUDE_OUTPUT_DIR})
file(COPY gepard-image.h DESTINATION ${INCLUDE_OUTPUT_DIR})
file(COPY surfaces DESTINATION ${INCLUDE_OUTPUT_DIR})

set_target_properties(gepard
                      PROPERTIES
                      FRAMEWORK ON
                      VERSION ${PROJECT_VERSION}
                      PUBLIC_HEADER "${GEPARD_HEADERS}"
                      RESOURCE "${SURFACE_HEADERS}"
                      )

install(TARGETS gepard
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        RESOURCE DESTINATION ${CMAKE_INSTALL_PREFIX}/include/surfaces
        )
