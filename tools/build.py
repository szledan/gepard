#! /usr/bin/python -B
# -*- coding: utf-8 -*-
#
# Copyright (C) 2015-2018, Gepard Graphics
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

import argparse
import dependencies
import subprocess
import sys
import util
from os import path
from os import getcwd
from os import makedirs


def create_options(arguments=None):
    """ Create a CMake option list from parsed arguments. """

    opts = []

    if hasattr(arguments, 'build_type'):
        opts.append('-DCMAKE_BUILD_TYPE=' + arguments.build_type.capitalize())

    if hasattr(arguments, 'backend'):
        opts.append('-DBACKEND=' + arguments.backend.upper())

    if hasattr(arguments, 'log_level'):
        opts.append('-DLOG_LEVEL=' + str(arguments.log_level))

    if hasattr(arguments, 'no_colored_logs') and arguments.no_colored_logs:
        opts.append('-DDISABLE_LOG_COLORS=ON')

    if hasattr(arguments, 'install_prefix') and arguments.install_prefix:
        opts.append('-DCMAKE_INSTALL_PREFIX=' + arguments.install_prefix)

    return opts


def add_args(parser):
    """ Adds arguments to the parser. """
    parser.add_argument('--build-dir', '-b', action='store', dest='build_dir', help='Specify build directory.')
    parser.add_argument('--install-prefix', action='store', dest='install_prefix', help='Specify install prefix.')
    parser.add_argument('--debug', '-d', action='store_const', const='debug', default='release', dest='build_type', help='Build debug.')
    parser.add_argument('--rebuild-deps', action='store_true', default=False, help='Rebuild thirdparty dependencies.')
    parser.add_argument('--backend', action='store', choices=['gles2', 'software', 'vulkan'], default='gles2', help='Specify which graphics back-end to use.')
    parser.add_argument('--log-level', '-l', action='store', type=int, choices=range(0,5), default=0, help='Set logging level.')
    parser.add_argument('--no-colored-logs', action='store_true', default=False, help='Disable colored log messages.')
    parser.add_argument('targets', action='store', nargs='*', default=['gepard'], help='List of targets to build')


def get_args(skip_unknown=False):
    """ Parses input arguments. """
    parser = argparse.ArgumentParser()
    add_args(parser)

    return parser.parse_args()


def configure(source_path, build_path, arguments=None):
    """ Configures the build based on the supplied arguments. """
    try:
        makedirs(build_path)
    except OSError:
        pass

    util.call(['cmake', '-B' + build_path, '-H' + source_path] + create_options(arguments))


def build_targets(build_path, targets):
    """ Builds the specified targets. """
    build_command = ['make', '-s', '-C', build_path]
    build_command.extend(targets)

    util.call(build_command)


def print_success():
    """ Prints the 'Build succeeded' message. """
    print('')
    print('-' * 30)
    print('Build succeeded!')
    print('-' * 30)


def main():
    arguments = get_args()

    try:
        dependencies.build_dependencies([arguments.backend], arguments.rebuild_deps)
        build_path = util.get_build_path(arguments)
        configure(util.get_base_path(), build_path, arguments)
        build_targets(build_path, arguments.targets)
    except util.CommandError as e:
        print("Build failed: %s" % e)
        sys.exit(e.code)

    print_success()


if __name__ == "__main__":
    main()
