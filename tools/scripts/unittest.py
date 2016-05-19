#! /usr/bin/python
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
import build
from os import path
from os import getcwd


def run_unittest(args):
    build_path = build.get_build_path(args)

    print("Building unit-tests...")
    # We use the argument parser from the main build script here to initialize all required members of the argument structure.
    ret = build.configure(build.get_args())

    if not ret:
      ret = build.build_unit(args)

    if not ret:
      ret = subprocess.call([path.join(build_path, 'bin', 'unit')])

    return ret


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--debug', '-d', action='store_const', const='debug', default='release', dest='build_type', help='Build debug.')
    parser.add_argument('--backend', action='store', choices=['gles2', 'vulkan'], default='gles2', help='Specify which graphics back-end to use.')

    arguments = parser.parse_args()

    ret = run_unittest(arguments)

    sys.exit(ret)


if __name__ == "__main__":
    main()
