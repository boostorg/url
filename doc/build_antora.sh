#
# Copyright (c) 2023 Alan de Freitas (alandefreitas@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/boostorg/url
#

set -e

# If CXX was not already set, then determine the newest clang.
if [ ! -n "$CXX" ]; then
    for i in $(seq 40 -1 18); do
        if [ -f /usr/bin/clang++-${i} ]; then
            export CXX="/usr/bin/clang++-$i"
            export CC="/usr/bin/clang-$i"
            break
        fi
    done
fi

set -x

if [ $# -eq 0 ]
  then
    echo "No playbook supplied, using default playbook"
    PLAYBOOK="local-playbook.yml"
  else
    PLAYBOOK=$1
fi

echo "Building documentation with Antora..."
echo "Installing npm dependencies..."
npm ci

echo "Building docs in custom dir..."
PATH="$(pwd)/node_modules/.bin:${PATH}"
export PATH
npx antora --clean --fetch "$PLAYBOOK"
echo "Done"
