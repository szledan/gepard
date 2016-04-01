#! /usr/bin/python2

import sys
from os import path
from os import chdir
from os import getcwd
from subprocess import call


def run_cppcheck():
    basedir = path.join(getcwd(), path.split(sys.argv[0])[0], '..', '..')
    chdir(basedir)

    call(['cppcheck', 'src'])
    call(['cppcheck', 'examples'])


if __name__ == "__main__":
    run_cppcheck()
