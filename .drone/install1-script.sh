#!/bin/bash

set -ex
export TRAVIS_BUILD_DIR=$(pwd)
export DRONE_BUILD_DIR=$(pwd)
export TRAVIS_BRANCH=$DRONE_BRANCH
export VCS_COMMIT_ID=$DRONE_COMMIT
export GIT_COMMIT=$DRONE_COMMIT
export REPO_NAME=$DRONE_REPO
export PATH=~/.local/bin:/usr/local/bin:$PATH
export SELF=`basename $REPO_NAME`

# echo '==================================> BEFORE_INSTALL'
# 
# . .drone/before-install.sh

echo '==================================> INSTALL'

pip install --user cmake

# echo '==================================> BEFORE_SCRIPT'
# 
# . $DRONE_BUILD_DIR/.drone/before-script.sh

echo '==================================> SCRIPT'
BOOST_BRANCH=develop && [ "$DRONE_BRANCH" == "master" ] && BOOST_BRANCH=master || true
echo BOOST_BRANCH: $BOOST_BRANCH
cd ..
git clone -b $BOOST_BRANCH --depth 1 https://github.com/boostorg/boost.git boost-root
cd boost-root
mkdir -p libs/$SELF
cp -r $DRONE_BUILD_DIR/* libs/$SELF
git submodule update --init tools/boostdep
git submodule update --init --recursive

cd libs/$SELF
mkdir __build__ && cd __build__
cmake -DCMAKE_INSTALL_PREFIX=~/.local ..
cmake --build . --target install
#
# cmake_install_test directory was removed. So the following test would fail.
# cd ../test/cmake_install_test && mkdir __build__ && cd __build__
# cmake -DCMAKE_INSTALL_PREFIX=~/.local ..
# cmake --build .
# cmake --build . --target check

# echo '==================================> AFTER_SUCCESS'
# 
# . $DRONE_BUILD_DIR/.drone/after-success.sh
