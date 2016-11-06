#! /usr/bin/python -B
# -*- coding: utf-8 -*-
#
# Copyright (C) 2016, Gepard Graphics
# Copyright (C) 2016, Kristóf Kosztyó <kkristof@inf.u-szeged.hu>
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

import fnmatch
import os
import subprocess
import util

glslang_path = os.path.join(util.get_base_path(), 'thirdparty', 'glslang')
glslang_path_build = os.path.join(glslang_path, 'build')
glslang_git_url = 'https://github.com/KhronosGroup/glslang.git'

shader_source_directory = os.path.join(util.get_base_path(), 'src', 'engines', 'vulkan', 'shaders')

def prepare_glslang():
    if (not os.path.exists(glslang_path)):
        util.call(['git', 'clone', glslang_git_url, glslang_path])
    util.call(['cmake', '-B' + glslang_path_build, '-H' + glslang_path])
    util.call(['make', '-C' + glslang_path_build])

def collect_shader_sources():
    shader_files = []
    for file in os.listdir(shader_source_directory):
        if fnmatch.fnmatch(file, '*.frag') or fnmatch.fnmatch(file, '*.vert'):
            shader_files.append(file)
    return shader_files

def compile_shader(shader):
    glslang_binary = os.path.join(glslang_path_build, 'StandAlone', 'glslangValidator')
    shader_path = os.path.join(shader_source_directory, shader)
    util.call([glslang_binary, '-V', shader_path, '-o', shader_path + '.spv'])

if __name__ == '__main__':
    prepare_glslang()
    for shader in collect_shader_sources():
        compile_shader(shader)
