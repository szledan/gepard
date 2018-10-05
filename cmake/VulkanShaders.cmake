# Copyright (C) 2018, Gepard Graphics
# Copyright (C) 2018, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
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

set(VK_SHADER_DIRECTORY ${PROJECT_SOURCE_DIR}/src/engines/vulkan/shaders)
set(SPIRV_DIRECTORY ${PROJECT_BINARY_DIR}/spirv)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/spirv)

add_custom_target(
    vulkan_shaders
    DEPENDS ${SPIRV_DIRECTORY}/fill-rect.vert.inc
            ${SPIRV_DIRECTORY}/fill-rect.frag.inc
            ${SPIRV_DIRECTORY}/image.vert.inc
            ${SPIRV_DIRECTORY}/image.frag.inc
)

add_custom_command(
    OUTPUT ${SPIRV_DIRECTORY}/fill-rect.vert.inc
    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${SPIRV_DIRECTORY}/fill-rect.vert.inc ${VK_SHADER_DIRECTORY}/fill-rect.vert
    DEPENDS ${VK_SHADER_DIRECTORY}/fill-rect.vert ${GLSLANG_VALIDATOR}
)

add_custom_command(
    OUTPUT ${SPIRV_DIRECTORY}/fill-rect.frag.inc
    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${SPIRV_DIRECTORY}/fill-rect.frag.inc ${VK_SHADER_DIRECTORY}/fill-rect.frag
    DEPENDS ${VK_SHADER_DIRECTORY}/fill-rect.frag ${GLSLANG_VALIDATOR}
)

add_custom_command(
    OUTPUT ${SPIRV_DIRECTORY}/image.vert.inc
    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${SPIRV_DIRECTORY}/image.vert.inc ${VK_SHADER_DIRECTORY}/image.vert
    DEPENDS ${VK_SHADER_DIRECTORY}/image.vert ${GLSLANG_VALIDATOR}
)

add_custom_command(
    OUTPUT ${SPIRV_DIRECTORY}/image.frag.inc
    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${SPIRV_DIRECTORY}/image.frag.inc ${VK_SHADER_DIRECTORY}/image.frag
    DEPENDS ${VK_SHADER_DIRECTORY}/image.frag ${GLSLANG_VALIDATOR}
)
