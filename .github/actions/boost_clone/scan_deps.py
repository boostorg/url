#!/usr/bin/env python

#
# Copyright (c) 2023 Alan de Freitas (alandefreitas@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CPPAlliance/url
#

# Adapted from boostorg/boostdep/blob/develop/depinst/depinst.py

# depinst.py - installs the dependencies needed to test
#              a Boost library
#
# Copyright 2016-2020 Peter Dimov
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt

# The difference between this script and depinst.py is that depinst.py
# is intended to clone the dependencies of other boost modules, while
# this script is only intended to find what modules should be cloned
# for a library that depends on boost needs to work.

# The intention of the script is to determine what modules should be
# cloned *before* boost is cloned, thus allowing us to determine if
# we can use cached results.

from __future__ import print_function

import re
import sys
import os
import argparse

verbose = 0


def vprint(level, *args):
    if verbose >= level:
        print(*args)


def is_module(m, gm):
    return ('libs/' + m) in gm


def module_for_header(h, x, gm):
    if h in x:
        return x[h]
    else:
        # boost/function.hpp
        m = re.match('boost/([^\\./]*)\\.h[a-z]*$', h)

        if m and is_module(m.group(1), gm):
            return m.group(1)

        # boost/numeric/conversion.hpp
        m = re.match('boost/([^/]*/[^\\./]*)\\.h[a-z]*$', h)

        if m and is_module(m.group(1), gm):
            return m.group(1)

        # boost/numeric/conversion/header.hpp
        m = re.match('boost/([^/]*/[^/]*)/', h)

        if m and is_module(m.group(1), gm):
            return m.group(1)

        # boost/function/header.hpp
        m = re.match('boost/([^/]*)/', h)

        if m and is_module(m.group(1), gm):
            return m.group(1)

        vprint(0, 'Cannot determine module for header', h)

        return None


def scan_header_dependencies(f, exceptions, submodule_paths):
    deps = set()
    for line in f:
        m = re.match('[ \t]*#[ \t]*include[ \t]*["<](boost/[^">]*)[">]', line)
        if m:
            h = m.group(1)
            mod = module_for_header(h, exceptions, submodule_paths)
            deps.add(mod)
    return deps


def scan_directory(d, exceptions, submodule_paths):
    vprint(1, 'Scanning directory', d)

    if os.name == 'nt' and sys.version_info[0] < 3:
        d = unicode(d)

    deps = set()
    for root, dirs, files in os.walk(d):
        for file in files:
            fn = os.path.join(root, file)
            vprint(2, 'Scanning file', fn)
            if sys.version_info[0] < 3:
                with open(fn, 'r') as f:
                    deps.update(scan_header_dependencies(f, exceptions, submodule_paths))
            else:
                with open(fn, 'r', encoding='latin-1') as f:
                    deps.update(scan_header_dependencies(f, exceptions, submodule_paths))
    return deps


def list_boost_dependencies(dir, subdirs, exceptions, submodule_paths):
    vprint(1, 'Scanning dir', dir)
    deps = set()
    for subdir in subdirs:
        deps.update(scan_directory(os.path.join(dir, subdir), exceptions, submodule_paths))
    return deps


def read_exceptions(branch):
    # exceptions.txt is the output of "boostdep --list-exceptions"
    vprint(1, 'Reading exceptions.txt')
    x = {}
    module = None

    exceptions_path = os.path.join(os.path.dirname(sys.argv[0]), branch + '.exceptions.txt')
    if not os.path.exists(exceptions_path):
        import requests
        url = "https://raw.githubusercontent.com/boostorg/boostdep/" + branch + "/depinst/exceptions.txt"
        response = requests.get(url)
        if response.status_code == 200:
            content = response.text
            with open(exceptions_path, "w") as f:
                f.write(content)

    with open(exceptions_path, 'r') as f:
        for line in f:
            line = line.rstrip()
            m = re.match('(.*):$', line)
            if m:
                module = m.group(1).replace('~', '/')
            else:
                header = line.lstrip()
                x[header] = module
    return x


def read_gitmodules(branch):
    vprint(1, 'Reading .gitmodules')
    gm = []

    gitmodules_path = os.path.join(os.path.dirname(sys.argv[0]), branch + '.gitmodules')
    if not os.path.exists(gitmodules_path):
        import requests
        url = "https://raw.githubusercontent.com/boostorg/boost/" + branch + "/.gitmodules"
        response = requests.get(url)
        if response.status_code == 200:
            content = response.text
            with open(gitmodules_path, "w") as f:
                f.write(content)

    with open(gitmodules_path, 'r') as f:
        for line in f:
            line = line.strip()
            m = re.match('path[ \t]*=[ \t]*(.*)$', line)
            if m:
                gm.append(m.group(1))

    return gm


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Installs the dependencies needed to test a Boost library.')

    parser.add_argument('--dir', help="directory to scan")
    parser.add_argument('--branch', help="boost branch", default='master')

    parser.add_argument('-I', '--include', help="additional subdirectory to scan; can be repeated", metavar='DIR',
                        action='append', default=[])
    parser.add_argument('-X', '--exclude',
                        help="exclude a default subdirectory ('include', 'src', or 'test') from scan; can be repeated",
                        metavar='DIR', action='append', default=[])
    parser.add_argument('-N', '--ignore', help="exclude top-level dependency even when found in scan; can be repeated",
                        metavar='LIB', action='append', default=[])
    parser.add_argument('-v', '--verbose', help='enable verbose output', action='count', default=0)
    parser.add_argument('-q', '--quiet', help='quiet output (opposite of -v)', action='count', default=0)

    args = parser.parse_args()

    verbose = args.verbose - args.quiet

    vprint(2, '-X:', args.exclude)
    vprint(2, '-I:', args.include)
    vprint(2, '-N:', args.ignore)

    exceptions = read_exceptions(args.branch)
    vprint(2, 'Exceptions:', exceptions)

    submodule_paths = read_gitmodules(args.branch)
    vprint(2, '.gitmodules:', submodule_paths)

    subdirs = ['include', 'src', 'source', 'test', 'tests', 'example', 'examples']
    for subdir in args.exclude:
        if subdir in subdirs:
            subdirs.remove(subdir)
    for subdir in args.include:
        if subdir not in subdirs:
            subdirs.append(subdir)
    vprint(1, 'Directories to scan:', *subdirs)

    modules = list_boost_dependencies(args.dir, subdirs, exceptions, submodule_paths)
    for ignored in args.ignore:
        if ignored in modules:
            modules.remove(ignored)

    sorted_modules = sorted(modules)
    print(' '.join(sorted_modules))
