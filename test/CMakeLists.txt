#
# Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/uri
#

source_group (uri FILES ${PROJECT_SOURCE_DIR}/include/boost/uri.hpp)
source_group (TREE ${PROJECT_SOURCE_DIR}/include/boost/uri PREFIX uri FILES ${PROJECT_FILES})

GroupSources(test "/")

add_definitions(-DBOOST_URI_HEADER_ONLY=1)

add_executable (uri-tests
    ${BEAST_FILES}
    ${PROJECT_FILES}
    ${PROJECT_SOURCE_DIR}/include/boost/uri.hpp
    ${PROJECT_SOURCE_DIR}/src/src.cpp
    Jamfile
    main.cpp
    error.cpp
)

add_test(uri-tests uri-tests)
