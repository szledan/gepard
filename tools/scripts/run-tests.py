#! /usr/bin/python2

import argparse
import sys
from unittest import run_unittest
from cppcheck import run_cppcheck

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--debug', '-d', action='store_const', const='debug', default='release', dest='type')

    arguments = parser.parse_args()

    print "Running cppcheck."
    run_cppcheck()
    print "\nRunning unit-tests."
    run_unittest(arguments.type)


if __name__ == "__main__":
    main()
