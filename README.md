[![Boost.URL](https://raw.githubusercontent.com/vinniefalco/url/master/doc/images/repo-logo.png)](http://master.url.cpp.al/)

Branch          | [`master`](https://github.com/CPPAlliance/url/tree/master) | [`develop`](https://github.com/CPPAlliance/url/tree/develop) |
--------------- | ----------------------------------------------------------- | ------------------------------------------------------------- |
Docs            | [![Documentation](https://img.shields.io/badge/docs-master-brightgreen.svg)](http://master.url.cpp.al/) | [![Documentation](https://img.shields.io/badge/docs-develop-brightgreen.svg)](http://develop.url.cpp.al/)
[Drone](https://drone.io/) | [![Build Status](https://drone.cpp.al/api/badges/CPPAlliance/url/status.svg?ref=refs/heads/master)](https://drone.cpp.al/CPPAlliance/url) | [![Build Status](https://drone.cpp.al/api/badges/CPPAlliance/url/status.svg?ref=refs/heads/develop)](https://drone.cpp.al/CPPAlliance/url)
[GitHub Actions](https://github.com/) | [![CI](https://github.com/CPPAlliance/url/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/CPPAlliance/url/actions/workflows/ci.yml) | [![CI](https://github.com/CPPAlliance/url/actions/workflows/ci.yml/badge.svg?branch=develop)](https://github.com/CPPAlliance/url/actions/workflows/ci.yml)
Matrix          | --- | --- 
Fuzzing         | --- | --- 
[codecov.io](https://codecov.io) | [![codecov](https://codecov.io/gh/CPPAlliance/url/branch/master/graph/badge.svg)](https://codecov.io/gh/CPPAlliance/url/branch/master) | [![codecov](https://codecov.io/gh/CPPAlliance/url/branch/develop/graph/badge.svg)](https://codecov.io/gh/CPPAlliance/url/branch/develop)

# Boost.URL

## This is currently **NOT** an official Boost library.

## Overview

Boost.URL is a portable C++ library which provides containers and algorithms
which model a "URL", more formally described using the Uniform Resource
Identifier (URI) specification (henceforth referred to as rfc3986). A URL
is a compact sequence of characters that identifies an abstract or physical
resource. For example, this is a valid URL which satisfies the
absolute-URI grammar:
```
https://www.example.com/path/to/file.txt?userid=1001&page=2&results=full
```

This library understands the various grammars related to URLs and provides
for validating and parsing of strings, manipulation of URL strings, and
algorithms operating on URLs such as normalization and resolution. While
the library is general purpose, special care has been taken to ensure that
the implementation and data representation are friendly to network programs
which need to handle URLs efficiently and securely, including the case where
the inputs come from untrusted sources. Interfaces are provided for using
error codes instead of exceptions as needed, and all algorithms provide a
mechanism for avoiding memory allocations entirely if desired. Another
feature of the library is that all container mutations leave the URL in
a valid state. Code which uses Boost.URL will be easy to read, flexible,
and performant.

Network programs such as those using Boost.Asio or Boost.Beast often
encounter the need to process, generate, or modify URLs. This library
provides a very much needed modular component for handling these
use-cases.

## Example
```
using namespace boost::urls;

// Parse a URL. This allocates no memory. The view
// references the character buffer without taking ownership.
//
url_view uv = parse_uri( "https://www.example.com/path/to/file.txt?id=1001&name=John%20Doe&results=full" ).value();

// Print the query parameters with percent-decoding applied
//
for( auto v : uv.params() )
    std::cout << v.key << "=" << v.value << " ";

// Prints: id=1001 name=John Doe results=full

// Create a modifiable copy of `uv`, with ownership of the buffer
//
url u = uv;

// Change some elements in the URL
//
u.set_scheme( "http" )
 .set_encoded_host( "boost.org" )
 .set_encoded_path( "/index.htm" )
 .remove_query()
 .remove_fragment()
 .params().emplace_back( "key", "value" );

std::cout << u;

// Prints: http://boost.org/index.htm?key=value
```

## Design Goals

The library achieves these goals:

* Require only C++11
* Works without exceptions
* Fast compilation, no templates
* Strict compliance with rfc3986
* Allocator control, including avoiding allocation entirely
* Optional header-only, without linking to a library

## Requirements

* Requires Boost and a compiler supporting at least C++11
* Aliases for standard types use their Boost equivalents
* Link to a built static or dynamic Boost library, or use header-only (see below)
* Supports -fno-exceptions, detected automatically

### Header-Only

To use as header-only; that is, to eliminate the requirement to
link a program to a static or dynamic Boost.URL library, simply
place the following line in exactly one new or existing source
file in your project.
```
#include <boost/url/src.hpp>
```

### Embedded

Boost.URL works great on embedded devices. It can be used in a way
that avoids all dynamic memory allocations. Furthermore it is designed 
to work without exceptions if desired.

### Supported Compilers

Boost.URL is tested with the following compilers:

* clang: 3.8, 4, 5, 6, 7, 8, 9, 10, 11, 12
* gcc: 4.8, 4.9, 5, 6, 7, 8, 9, 10, 11
* msvc: 14.0, 14.1, 14.2, 14.3

### Quality Assurance

The development infrastructure for the library includes
these per-commit analyses:

* Coverage reports
* Benchmark performance comparisons
* Compilation and tests on Drone.io, Azure Pipelines, Appveyor
* Fuzzing using clang-llvm and machine learning

## Visual Studio Solution Generation

    cmake -G "Visual Studio 16 2019" -A Win32 -B bin -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/msvc.cmake
    cmake -G "Visual Studio 16 2019" -A x64 -B bin64 -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/msvc.cmake

## License

Distributed under the Boost Software License, Version 1.0.
(See accompanying file [LICENSE_1_0.txt](LICENSE_1_0.txt) or copy at
https://www.boost.org/LICENSE_1_0.txt)
