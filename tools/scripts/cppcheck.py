#! /usr/bin/python

import argparse
import subprocess
import sys
from os import path
from os import getcwd


def run_cppcheck(all):
    basedir = path.abspath(path.join(path.dirname(__file__), '..', '..'))
    cmd = ['cppcheck', path.join(basedir, 'src'), path.join(basedir, 'examples')]

    if all:
        cmd.append('--enable=all')

    return subprocess.call(cmd)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--all', '-a', action='store_true', default=False, dest='all', help='Enable all checks')

    arguments = parser.parse_args()
    ret = run_cppcheck(arguments.all)

    sys.exit(ret)


if __name__ == "__main__":
    main()
