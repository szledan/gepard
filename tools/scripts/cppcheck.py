#! /usr/bin/python

import subprocess
import sys
from os import path
from os import chdir
from os import getcwd


def run_cppcheck():
    basedir = path.abspath(path.dirname(path.dirname(path.join(getcwd(), path.dirname(sys.argv[0])))))

    subprocess.call(['cppcheck', path.join(basedir, 'src')])
    subprocess.call(['cppcheck', path.join(basedir, 'examples')])


if __name__ == "__main__":
    run_cppcheck()
