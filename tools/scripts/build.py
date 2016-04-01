#! /usr/bin/python2

import argparse
import sys
from os import path
from os import chdir
from os import getcwd
from os import makedirs
from subprocess import call


# Create cmake option list from parsed arguments
def create_options(arguments):
    opts = []
    opts.append('-DCMAKE_BUILD_TYPE=' + arguments.build_type.capitalize())
    opts.append('-DUSE_' + arguments.backend.upper() + '=ON')
    opts.append('-DLOG_LEVEL=' + str(arguments.log_level))
    if arguments.no_colored_logs:
        opts.append('-DDISABLE_LOG_COLORS=ON')

    if (arguments.backend != 'gles2'):
        opts.append('-DUSE_GLES2=OFF')  #GLES2 is enabled by default, so just disable it if we're not using it.

    return opts


# Perform the build
def build(arguments):
    basedir = path.join(getcwd(), path.split(sys.argv[0])[0], '..', '..')
    build_path = path.join(basedir, 'build', arguments.build_type)

    try:
        makedirs(build_path)
    except OSError:
        pass

    chdir(build_path)

    if arguments.clean:
        call(['make', 'clean'])

    ret = call(['cmake'] + create_options(arguments) + [basedir])
    if ret:
        return ret

    return call(['make']);


# Print build result
def print_result(ret):
    print('')
    print('------------------------------')
    if ret:
        print 'Build failed with exit code: ' + str(ret)
    else:
        print 'Build succeeded!'
    print('------------------------------')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--debug', '-d', action='store_const', const='debug', default='release', dest='build_type', help='Build with debug enabled')
    parser.add_argument('--clean', action='store_true', default=False, help='Perform clean build')
    parser.add_argument('--target', action='store', choices=['x86-linux', 'arm-linux'], help='Specify build target. Leave empty for native build.')
    parser.add_argument('--backend', action='store', choices=['gles2', 'vulkan'], default='gles2', help='Specify which graphics back-end to use.')
    parser.add_argument('--log-level', action='store', type=int, choices=range(0,4), default=0, help='Set logging level')
    parser.add_argument('--no-colored-logs', action='store_true', default=False, help='Disable colored log messages')

    arguments = parser.parse_args()

    ret = build(arguments)

    print_result(ret)
    sys.exit(ret)


if __name__ == "__main__":
    main()
