#! /usr/bin/python2

import argparse
import sys
from os import path
from os import chdir
from os import getcwd
from subprocess import call

def run_unittest(type):
    basedir = path.join(getcwd(), path.split(sys.argv[0])[0], '..', '..')
    build_path = path.join(basedir, 'build', type)
    if not path.isfile(path.join(build_path, 'bin', 'unit')):
        print "Error: Unit-tests are not built."
        return 1

    chdir(build_path)
    return call(['./bin/unit'])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--debug', '-d', action='store_const', const='debug', default='release', dest='type')

    arguments = parser.parse_args()
    ret = run_unittest(arguments.type)

    sys.exit(ret)


if __name__ == "__main__":
    main()
