#! /usr/bin/python -B
# -*- coding: utf-8 -*-
#
# Copyright (C) 2015-2016, Gepard Graphics
# Copyright (C) 2016, Dániel Bátyai <dbatyai@inf.u-szeged.hu>
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

import argparse
import subprocess
import sys
from os import path
from os import getcwd
from os import makedirs


# Create cmake option list from parsed arguments
def create_options(arguments):
    opts = [
            '-DCMAKE_BUILD_TYPE=' + arguments.build_type.capitalize(),
            '-DUSE_%s=ON' % arguments.backend.upper(),
            '-DLOG_LEVEL=' + str(arguments.log_level)
    ]

    if arguments.no_colored_logs:
        opts.append('-DDISABLE_LOG_COLORS=ON')

    if arguments.build_examples:
        opts.append('-DBUILD_EXAMPLES=ON')

    if arguments.backend != 'gles2':
        opts.append('-DUSE_GLES2=OFF')  #GLES2 is enabled by default, so just disable it if we're not using it.

    return opts


# Base arguments
def add_base_args(parser):
    parser.add_argument('--debug', '-d', action='store_const', const='debug', default='release', dest='build_type', help='Build debug.')
    parser.add_argument('--backend', action='store', choices=['gles2', 'vulkan'], default='gles2', help='Specify which graphics back-end to use.')


# Extra build arguments
def add_extra_build_args(parser):
    parser.add_argument('--clean', action='store_true', default=False, help='Perform clean build.')
    parser.add_argument('--examples', '-e', action='store_true', default=False, dest='build_examples', help='Build example applications.')
    parser.add_argument('--target', action='store', choices=['x86-linux', 'arm-linux'], help='Specify build target. Leave empty for native build.')
    parser.add_argument('--log-level', action='store', type=int, choices=range(0,4), default=0, help='Set logging level.')
    parser.add_argument('--no-colored-logs', action='store_true', default=False, help='Disable colored log messages.')


# Parse build args
def get_args():
    parser = argparse.ArgumentParser()
    add_base_args(parser)
    add_extra_build_args(parser)

    return parser.parse_args()


# Get base path
def get_base_path():
    return path.abspath(path.join(path.dirname(__file__), '..', '..'))


# Get build path
def get_build_path(arguments):
    basedir = get_base_path()
    return path.join(basedir, 'build', arguments.build_type)


# Generate build config
def configure(arguments):
    build_path = get_build_path(arguments)

    try:
        makedirs(build_path)
    except OSError:
        pass

    return subprocess.call(['cmake', '-B' + build_path, '-H' + get_base_path()] + create_options(arguments))


# Build unit tests
def build_unit(arguments):
    build_path = get_build_path(arguments)

    if not path.isfile(path.join(build_path, 'Makefile')):
        raise RuntimeError('Build is not configured.')

    return subprocess.call(['make', '-C', build_path, 'unit'])


# Perform the build
def build_gepard(arguments):
    build_path = get_build_path(arguments)

    if not path.isfile(path.join(build_path, 'Makefile')):
        raise RuntimeError('Build is not configured.')

    if arguments.clean:
        subprocess.call(['make', '-C', build_path, 'clean'])

    return subprocess.call(['make', '-C', build_path])


# Print build result
def print_result(ret):
    print('')
    print('-' * 30)
    if ret:
        print('Build failed with exit code: %s' % (ret))
    else:
        print('Build succeeded!')
    print('-' * 30)


def main():
    arguments = get_args()
    ret = configure(arguments)

    if not ret:
        ret = build_gepard(arguments)

    print_result(ret)
    sys.exit(ret)


if __name__ == "__main__":
    main()
