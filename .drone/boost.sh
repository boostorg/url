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

git clone https://github.com/boostorg/boost-ci.git boost-ci-cloned
cp -prf boost-ci-cloned/ci .
rm -rf boost-ci-cloned

if [ "$TRAVIS_OS_NAME" == "osx" ]; then
    unset -f cd
fi

export SELF=`basename $REPO_NAME`
export BOOST_CI_TARGET_BRANCH="$TRAVIS_BRANCH"
export BOOST_CI_SRC_FOLDER=$(pwd)

. ./ci/common_install.sh

# echo '==================================> BEFORE_SCRIPT'
# 
# . $DRONE_BUILD_DIR/.drone/before-script.sh

echo '==================================> SCRIPT'

$BOOST_ROOT/libs/$SELF/ci/travis/build.sh

# echo '==================================> AFTER_SUCCESS'
# 
# . $DRONE_BUILD_DIR/.drone/after-success.sh
