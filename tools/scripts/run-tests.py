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
import build
import sys
import util
from cppcheck import run_cppcheck
from unittest import run_unittest

def print_results(result):
    longest = 0
    ret = 0
    for name, _ in result:
        if len(name) > longest:
            longest = len(name)

    for (name, code) in result:
        if code:
            msg = "\033[1;31mFAIL\033[0m"
            ret = 1
        else:
            msg = "\033[1;32mPASS\033[0m"

        print("".join([name, ':', ' ' * (longest - len(name) + 1), msg]))

    return ret


def main():
    parser = argparse.ArgumentParser()
    build.add_base_args(parser)
    arguments = parser.parse_args()
    result = []

    print("Running cppcheck.")
    result.append(("Cppcheck", run_cppcheck(throw=False)))

    print("\n")
    print("Running unit-tests.")
    result.append(("Unit-tests", run_unittest(arguments, throw=False)))

    print("\n")
    sys.exit(print_results(result))


if __name__ == "__main__":
    main()
