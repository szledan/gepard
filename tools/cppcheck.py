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
import subprocess
import sys
import util
from os import chdir
from os import errno
from os import getcwd
from os import path


def run_cppcheck(throw=True):
    """ Runs cppcheck. """
    basedir = util.get_base_path()
    cmd = [
        'cppcheck',
        '--enable=all',
#        '--error-exitcode=2', # Uncomment when cppcheck issues are fixed
        '-UGD_ENABLE_LOGGING',
        '-UGD_BACKEND_GLES2',
        '-UGD_BACKEND_VULKAN',
        '-UGD_BACKEND_SOFTWARE',
        '--suppressions-list=%s' % (path.join('tools', 'cppcheck-suppr-list')),
        '--includes-file=%s' % (path.join('tools', 'cppcheck-incl-list')),
        'src',
        'examples',
    ]

    print('')
    print("Running cppcheck...")

    try:
        chdir(basedir)
        return util.call(cmd, throw)
    except OSError as e:
        if e.errno == errno.ENOENT:
            raise OSError("Cppcheck is not installed.")
        else:
            raise


def main():
    try:
        run_cppcheck()
    except util.CommandError as e:
        print(e)
        sys.exit(e.code)


if __name__ == "__main__":
    main()
