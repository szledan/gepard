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

import subprocess
import sys
from os import path

# Get base path
def get_base_path():
    return path.abspath(path.join(path.dirname(__file__), '..', '..'))


# Get build path
def get_build_path(arguments):
    basedir = get_base_path()
    return path.join(basedir, 'build', arguments.build_type)


# Command wrapper
def call(command, throw=True):
    ret = subprocess.call(command)
    if ret and throw:
        raise CommandError(ret, command)

    return ret


def print_success():
    print('')
    print('-' * 30)
    print('Everything passed.')
    print('-' * 30)


def print_fail():
    print('')
    print('-' * 30)
    print('Something failed, please see the log.')
    print('-' * 30)


class CommandError(BaseException):
    def __init__(self, code, cmd):
        super(CommandError, self).__init__(code, cmd)
        self.code = code
        self.cmd = cmd

    def __str__(self):
        return 'Command "%s" failed with exit code: %d' % (" ".join(self.cmd), self.code)
