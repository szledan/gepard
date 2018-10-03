#! /usr/bin/python -B
# -*- coding: utf-8 -*-
#
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

import argparse
import util
import build
import os.path


def check_stamp(path):
    # TODO(dbatyai): check if CMakeLists has been modified
    return os.path.isfile(path)


def build_dependencies(backends, force=False):
    source_path = os.path.join(util.get_base_path(), "thirdparty")
    build_path = os.path.join(source_path, "build")

    print('Building dependencies.')

    if 'all' in backends:
        backends = ['gles2', 'vulkan', 'software']

    if force:
        needs_build = backends
    else:
        needs_build = []
        for backend in backends:
            stamp_path = os.path.join(build_path, "stamps", "%s.stamp" % backend)
            if not check_stamp(stamp_path):
                needs_build.append(backend)

    if not needs_build:
        print('Nothing to do.')
        return

    build.configure(source_path, build_path)
    build.build_targets(build_path, needs_build)
    print('Done.')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('backends', action='store', nargs='*', choices=['gles2', 'vulkan', 'software', 'all'], default=['all'] , help='Which backends to build dependencies for.')
    parser.add_argument('--force', '-f', action='store_true', default=False, help='Force re-build.')
    arguments = parser.parse_args()

    try:
        build_dependencies(arguments.backends, arguments.force)
    except util.CommandError as e:
        print("Build failed: %s" % e)
        sys.exit(e.code)


if __name__ == "__main__":
    main()
