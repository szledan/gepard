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
import util
from os import path
from os import getcwd
from os import makedirs


# Create cmake option list from parsed arguments
def create_options(arguments):
    opts = [
            '-DCMAKE_BUILD_TYPE=' + arguments.build_type.capitalize(),
            '-DBACKEND=' + arguments.backend.upper(),
            '-DLOG_LEVEL=' + str(arguments.log_level)
    ]

    if arguments.no_colored_logs:
        opts.append('-DDISABLE_LOG_COLORS=ON')

    return opts


# Base arguments
def add_base_args(parser):
    parser.add_argument('--debug', '-d', action='store_const', const='debug', default='release', dest='build_type', help='Build debug.')
    parser.add_argument('--backend', action='store', choices=['gles2', 'software', 'vulkan'], default='gles2', help='Specify which graphics back-end to use.')


# Extra build arguments
def add_extra_build_args(parser):
    parser.add_argument('--clean', action='store_true', default=False, help='Perform clean build.')
    parser.add_argument('--examples', '-e', action='store_true', default=False, dest='build_examples', help='Build example applications.')
    parser.add_argument('--target', action='store', choices=['x86-linux', 'arm-linux'], help='Specify build target. Leave empty for native build.')
    parser.add_argument('--log-level', action='store', type=int, choices=range(0,5), default=0, help='Set logging level.')
    parser.add_argument('--no-colored-logs', action='store_true', default=False, help='Disable colored log messages.')


# Parse build args
def get_args():
    parser = argparse.ArgumentParser()
    add_base_args(parser)
    add_extra_build_args(parser)

    return parser.parse_args()


# Generate build config
def configure(arguments):
    build_path = util.get_build_path(arguments)

    try:
        makedirs(build_path)
    except OSError:
        pass

    util.call_cmd(['cmake', '-B' + build_path, '-H' + util.get_base_path()] + create_options(arguments))


# Check if build is configured
def check_configured(arguments):
    build_path = util.get_build_path(arguments)

    if not path.isfile(path.join(build_path, 'Makefile')):
        raise RuntimeError('Build is not configured.')


# Clean build directory
def run_clean(arguments):
    build_path = util.get_build_path(arguments)

    try:
        check_configured(arguments)
    except Error:
        return 0

    util.call_cmd(['make', '-s', '-C', build_path, 'clean'])


# Build unit tests
def build_unit(arguments):
    build_path = util.get_build_path(arguments)
    check_configured(arguments)

    util.call_cmd(['make', '-s', '-C', build_path, 'unit'])


# Build examples
def build_examples(arguments):
    build_path = util.get_build_path(arguments)
    check_configured(arguments)

    util.call_cmd(['make', '-s', '-C', build_path, 'examples'])


# Perform the build
def build_gepard(arguments):
    build_path = util.get_build_path(arguments)
    check_configured(arguments)

    util.call_cmd(['make', '-s', '-C', build_path])


# Print success message
def print_success():
    print('')
    print('-' * 30)
    print('Build succeeded!')
    print('-' * 30)


def main():
    arguments = get_args()
    configure(arguments)

    if arguments.clean:
        run_clean(arguments)

    if arguments.build_examples:
        ret = build_examples(arguments)
    else:
        ret = build_gepard(arguments)

    print_success()
    sys.exit(0)


if __name__ == "__main__":
    main()
