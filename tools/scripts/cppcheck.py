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
from os import chdir
from os import errno
from os import getcwd
from os import path


def run_cppcheck():
    basedir = path.abspath(path.join(path.dirname(__file__), '..', '..'))
    cmd = [
        'cppcheck',
        '--enable=all',
#        '--error-exitcode=2', # Uncomment when cppcheck issues are fixed
        '-ULOG_LEVEL', '-UDISABLE_LOG_COLORS',
        '--suppressions-list=%s' % (path.join('tools', 'cppcheck-suppr-list')),
        '--includes-file=%s' % (path.join('tools', 'cppcheck-incl-list')),
        'src',
        'examples',
    ]

    try:
        chdir(basedir)
        return subprocess.call(cmd)
    except OSError as e:
        if e.errno == errno.ENOENT:
            print("Error: cppcheck is not installed.")
            return 1
        else:
            raise


def print_result(ret):
    print('')
    print('-' * 30)
    if ret:
        print('There were some issues.\nPlease check the log above.')
    else:
        print('All checks passed.')

    print('-' * 30)


def main():
    ret = run_cppcheck()

    print_result(ret)
    sys.exit(ret)


if __name__ == "__main__":
    main()
