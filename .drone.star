# Use, modification, and distribution are
# subject to the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt)
#
# Copyright Rene Rivera 2020.

# For Drone CI we use the Starlark scripting language to reduce duplication.
# As the yaml syntax for Drone CI is rather limited.
#
#
globalenv={'B2_CI_VERSION': '1', 'B2_VARIANT': 'release'}
linuxglobalimage="cppalliance/droneubuntu1804:1"
windowsglobalimage="cppalliance/dronevs2019"

def main(ctx):
  return [
  linux_cxx("CMAKE_INSTALL_TEST=1 Job 0", "g++", packages="", buildscript="drone", buildtype="cmake1", image="cppalliance/droneubuntu1804:1", environment={'CMAKE_INSTALL_TEST': '1', 'DRONE_JOB_UUID': 'b6589fc6ab'}, globalenv=globalenv),
  linux_cxx("COMMENT=codecov.io LCOV_BRANCH_COVERAGE=0 B2_ Job 2", "g++-8", packages="g++-8", buildscript="drone", buildtype="codecov", image=linuxglobalimage, environment={'COMMENT': 'codecov.io', 'LCOV_BRANCH_COVERAGE': '0', 'B2_CXXSTD': '11', 'B2_TOOLSET': 'gcc-8', 'B2_DEFINES': 'BOOST_NO_STRESS_TEST=1', 'DRONE_JOB_UUID': 'da4b9237ba', "CODECOV_TOKEN": {"from_secret": "codecov_token"}}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=clang-8 B2_CXXSTD=17,2a Job 3", "clang++-8", packages="clang-8 libc6-dbg libc++-dev libstdc++-8-dev", llvm_os="bionic", llvm_ver="8", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'clang-8', 'B2_CXXSTD': '17,2a', 'DRONE_JOB_UUID': '77de68daec'}, globalenv=globalenv),
  linux_cxx("COMMENT=valgrind B2_TOOLSET=clang-6.0 B2_CXXS Job 4", "clang++-6.0", packages="clang-6.0 libc6-dbg libc++-dev libstdc++-8-dev", llvm_os="bionic", llvm_ver="6.0", buildscript="drone", buildtype="valgrind", image=linuxglobalimage, environment={'COMMENT': 'valgrind', 'B2_TOOLSET': 'clang-6.0', 'B2_CXXSTD': '11,14', 'B2_DEFINES': 'BOOST_NO_STRESS_TEST=1', 'B2_VARIANT': 'debug', 'B2_TESTFLAGS': 'testing.launcher=valgrind', 'VALGRIND_OPTS': '--error-exitcode=1', 'DRONE_JOB_UUID': '1b64538924'}, globalenv=globalenv),
  linux_cxx("COMMENT=asan B2_VARIANT=debug B2_TOOLSET=gcc- Job 5", "g++-8", packages="g++-8", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'COMMENT': 'asan', 'B2_VARIANT': 'debug', 'B2_TOOLSET': 'gcc-8', 'B2_CXXSTD': '11,14', 'B2_ASAN': '1', 'B2_DEFINES': 'BOOST_NO_STRESS_TEST=1', 'DRONE_EXTRA_PRIVILEGED': 'True', 'DRONE_JOB_UUID': 'ac3478d69a'}, globalenv=globalenv, privileged=True),
  linux_cxx("COMMENT=tsan B2_VARIANT=debug B2_TOOLSET=gcc- Job 6", "g++-8", packages="g++-8", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'COMMENT': 'tsan', 'B2_VARIANT': 'debug', 'B2_TOOLSET': 'gcc-8', 'B2_CXXSTD': '11,14', 'B2_TSAN': '1', 'B2_DEFINES': 'BOOST_NO_STRESS_TEST=1', 'DRONE_JOB_UUID': 'c1dfd96eea'}, globalenv=globalenv),
  linux_cxx("COMMENT=ubsan B2_VARIANT=debug B2_TOOLSET=gcc Job 7", "g++-8", packages="g++-8", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'COMMENT': 'ubsan', 'B2_VARIANT': 'debug', 'B2_TOOLSET': 'gcc-8', 'B2_CXXSTD': '11,14', 'B2_UBSAN': '1', 'B2_DEFINES': 'define=BOOST_NO_STRESS_TEST=1 define=BOOST_URL_HEADER_ONLY=1', 'B2_LINKFLAGS': '-fuse-ld=gold', 'DRONE_JOB_UUID': '902ba3cda1'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=gcc-4.8 B2_CXXSTD=11 Job 8", "g++-4.8", packages="g++-4.8", buildscript="drone", buildtype="boost", image="cppalliance/droneubuntu1604:1", environment={'B2_TOOLSET': 'gcc-4.8', 'B2_CXXSTD': '11', 'DRONE_JOB_UUID': 'fe5dbbcea5'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=gcc-4.9 B2_CXXSTD=11 Job 9", "g++-4.9", packages="g++-4.9", buildscript="drone", buildtype="boost", image="cppalliance/droneubuntu1604:1", environment={'B2_TOOLSET': 'gcc-4.9', 'B2_CXXSTD': '11', 'DRONE_JOB_UUID': '0ade7c2cf9'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=gcc-5 B2_CXXSTD=11 Job 10", "g++-5", packages="g++-5", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'gcc-5', 'B2_CXXSTD': '11', 'DRONE_JOB_UUID': 'b1d5781111'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=gcc-6 B2_CXXSTD=11,14 Job 11", "g++-6", packages="g++-6", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'gcc-6', 'B2_CXXSTD': '11,14', 'DRONE_JOB_UUID': '17ba079149'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=gcc-7 B2_CXXSTD=14,17 Job 12", "g++-7", packages="g++-7", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'gcc-7', 'B2_CXXSTD': '14,17', 'DRONE_JOB_UUID': '7b52009b64'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=gcc-8 B2_CXXSTD=17,2a Job 13", "g++-8", packages="g++-8", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'gcc-8', 'B2_CXXSTD': '17,2a', 'DRONE_JOB_UUID': 'bd307a3ec3'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=gcc-9 B2_CXXSTD=17,2a Job 14", "g++-9", packages="g++-9", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'gcc-9', 'B2_CXXSTD': '17,2a', 'DRONE_JOB_UUID': 'fa35e19212'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=clang-3.8 B2_CXXSTD=11 Job 15", "clang++-3.8", packages="clang-3.8", buildscript="drone", buildtype="boost", image="cppalliance/droneubuntu1604:1", environment={'B2_VARIANT': 'debug', 'B2_TOOLSET': 'clang-3.8', 'B2_CXXSTD': '11', 'DRONE_JOB_UUID': 'f1abd67035'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=clang-4.0 B2_CXXSTD=11,14 Job 16", "clang++-4.0", packages="clang-4.0 libstdc++-6-dev", llvm_os="xenial", llvm_ver="4.0", buildscript="drone", buildtype="boost", image="cppalliance/droneubuntu1804:1", environment={'B2_TOOLSET': 'clang-4.0', 'B2_CXXSTD': '11,14', 'DRONE_JOB_UUID': '1574bddb75'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=clang-5.0 B2_CXXSTD=11,14 Job 17", "clang++-5.0", packages="clang-5.0 libstdc++-7-dev", llvm_os="bionic", llvm_ver="5.0", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'clang-5.0', 'B2_CXXSTD': '11,14', 'DRONE_JOB_UUID': '0716d9708d'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=clang-6.0 B2_CXXSTD=14,17 Job 18", "clang++-6.0", packages="clang-6.0 libc6-dbg libc++-dev libc++abi-dev libstdc++-8-dev", llvm_os="bionic", llvm_ver="6.0", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'clang-6.0', 'B2_CXXSTD': '14,17', 'DRONE_JOB_UUID': '9e6a55b6b4'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=clang-7 B2_CXXSTD=17,2a Job 19", "clang++-7", packages="clang-7 libc6-dbg libc++-dev libstdc++-8-dev", llvm_os="bionic", llvm_ver="7", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'clang-7', 'B2_CXXSTD': '17,2a', 'DRONE_JOB_UUID': 'b3f0c7f6bb'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=clang-8 B2_CXXSTD=17,2a Job 20", "clang++-8", packages="clang-8 libc6-dbg libc++-dev libstdc++-8-dev", llvm_os="bionic", llvm_ver="8", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'clang-8', 'B2_CXXSTD': '17,2a', 'DRONE_JOB_UUID': '91032ad7bb'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=clang-9 B2_CXXSTD=11,14,17,2a Job 21", "clang++-9", packages="clang-9 libstdc++-9-dev", llvm_os="bionic", llvm_ver="9", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'clang-9', 'B2_CXXSTD': '11,14,17,2a', 'DRONE_JOB_UUID': '472b07b9fc'}, globalenv=globalenv),
  linux_cxx("B2_TOOLSET=clang-6.0 B2_CXXSTD=11,14 B2_STDLI Job 22", "clang++-6.0", packages="clang-6.0 libc6-dbg libc++-dev libc++abi-dev libstdc++-8-dev", llvm_os="bionic", llvm_ver="6.0", buildscript="drone", buildtype="boost", image=linuxglobalimage, environment={'B2_TOOLSET': 'clang-6.0', 'B2_CXXSTD': '11,14', 'B2_STDLIB': 'libc++', 'DRONE_JOB_UUID': '12c6fc06c9'}, globalenv=globalenv),
  osx_cxx("B2_TOOLSET=clang B2_CXXSTD=11,17 Job 23", "g++", packages="", buildscript="drone", buildtype="boost", environment={'B2_TOOLSET': 'clang', 'B2_CXXSTD': '11,17', 'DRONE_JOB_UUID': 'd435a6cdd7'}, globalenv=globalenv),
  linux_cxx("COMMENT=Coverity Scan B2_TOOLSET=clang Job 24", "g++", packages="", buildscript="drone", buildtype="coverity", image=linuxglobalimage, environment={'COMMENT': 'Coverity Scan', 'B2_TOOLSET': 'clang', 'DRONE_JOB_UUID': '4d134bc072'}, globalenv=globalenv),
  ]

# from https://github.com/boostorg/boost-ci
load("@boost_ci//ci/drone/:functions.star", "linux_cxx","windows_cxx","osx_cxx","freebsd_cxx")
