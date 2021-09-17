[![Boost.URL](https://raw.githubusercontent.com/vinniefalco/url/master/doc/images/repo-logo.png)](http://master.url.cpp.al/)

Branch          | [`master`](https://github.com/CPPAlliance/url/tree/master) | [`develop`](https://github.com/CPPAlliance/url/tree/develop) |
--------------- | ----------------------------------------------------------- | ------------------------------------------------------------- |
[Azure](https://azure.microsoft.com/en-us/services/devops/pipelines/) | [![Build Status](https://img.shields.io/azure-devops/build/vinniefalco/67375b5f-3402-44a3-a4ee-d5e856f94690/7/master)](https://vinniefalco.visualstudio.com/url/_build/latest?definitionId=7&branchName=master) | [![Build Status](https://img.shields.io/azure-devops/build/vinniefalco/67375b5f-3402-44a3-a4ee-d5e856f94690/7/develop)](https://vinniefalco.visualstudio.com/url/_build/latest?definitionId=7&branchName=develop)
Docs            | [![Documentation](https://img.shields.io/badge/docs-master-brightgreen.svg)](http://master.url.cpp.al/) | [![Documentation](https://img.shields.io/badge/docs-develop-brightgreen.svg)](http://develop.url.cpp.al/)
[Drone](https://drone.io/) | [![Build Status](https://drone.cpp.al/api/badges/boostorg/json/status.svg?ref=refs/heads/master)](https://drone.cpp.al/boostorg/json) | [![Build Status](https://drone.cpp.al/api/badges/boostorg/json/status.svg?ref=refs/heads/develop)](https://drone.cpp.al/boostorg/json)
[GitHub Actions](https://github.com/) | [![CI](https://github.com/CPPAlliance/url/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/CPPAlliance/url/actions/workflows/ci.yml) | [![CI](https://github.com/CPPAlliance/url/actions/workflows/ci.yml/badge.svg?branch=develop)](https://github.com/CPPAlliance/url/actions/workflows/ci.yml)
[
Matrix          | --- | --- 
Fuzzing         | --- | --- 
[Appveyor](https://ci.appveyor.com/) | [![Build status](https://ci.appveyor.com/api/projects/status/aewl3yuwn7skl5sr?svg=true&branch=master)](https://ci.appveyor.com/project/vinniefalco/cppalliance-url/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/aewl3yuwn7skl5sr?svg=true&branch=develop)](https://ci.appveyor.com/project/vinniefalco/cppalliance-url/branch/develop)
[codecov.io](https://codecov.io) | [![codecov](https://codecov.io/gh/CPPAlliance/url/branch/master/graph/badge.svg)](https://codecov.io/gh/CPPAlliance/url/branch/master) | [![codecov](https://codecov.io/gh/CPPAlliance/url/branch/develop/graph/badge.svg)](https://codecov.io/gh/CPPAlliance/url/branch/develop)

# Boost.URL

## This is currently **NOT** an official Boost library.

## Introduction

## Motivation

## Design Goals

## CMake

    cmake -G "Visual Studio 16 2019" -A Win32 -B bin -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/msvc.cmake
    cmake -G "Visual Studio 16 2019" -A x64 -B bin64 -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/msvc.cmake

## License

Distributed under the Boost Software License, Version 1.0.
(See accompanying file [LICENSE_1_0.txt](LICENSE_1_0.txt) or copy at
https://www.boost.org/LICENSE_1_0.txt)
