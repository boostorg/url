#!/bin/bash

set -ex
export TRAVIS_BUILD_DIR=$(pwd)
export DRONE_BUILD_DIR=$(pwd)
export TRAVIS_BRANCH=$DRONE_BRANCH
export VCS_COMMIT_ID=$DRONE_COMMIT
export GIT_COMMIT=$DRONE_COMMIT
export REPO_NAME=$DRONE_REPO
export PATH=~/.local/bin:/usr/local/bin:$PATH

# echo '==================================> BEFORE_INSTALL'
# 
# . .drone/before-install.sh

echo '==================================> INSTALL'

pip install --user cmake

# echo '==================================> BEFORE_SCRIPT'
# 
# . $DRONE_BUILD_DIR/.drone/before-script.sh

echo '==================================> SCRIPT'

mkdir __build__ && cd __build__
cmake -DCMAKE_INSTALL_PREFIX=~/.local ..
cmake --build . --target install
cd ../test/cmake_install_test && mkdir __build__ && cd __build__
cmake -DCMAKE_INSTALL_PREFIX=~/.local ..
cmake --build .
cmake --build . --target check

# echo '==================================> AFTER_SUCCESS'
# 
# . $DRONE_BUILD_DIR/.drone/after-success.sh
