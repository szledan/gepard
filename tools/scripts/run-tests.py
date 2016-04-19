#! /usr/bin/python

import argparse
import sys
from unittest import run_unittest
from cppcheck import run_cppcheck

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--debug', '-d', action='store_const', const='debug', default='release', dest='type')
    parser.add_argument('--all-cppcheck', '-a', action='store_true', default=False, dest='all', help='Enable all checks for cppcheck')

    arguments = parser.parse_args()

    print("Running cppcheck.")
    run_cppcheck(arguments.all)
    print("\nRunning unit-tests.")
    ret = run_unittest(arguments.type)
    sys.exit(ret)


if __name__ == "__main__":
    main()
