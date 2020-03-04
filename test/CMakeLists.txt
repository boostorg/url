#
# Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/url
#

source_group (boost FILES ${PROJECT_SOURCE_DIR}/include/boost/url.hpp)
source_group (_lib FILES ${PROJECT_SOURCE_DIR}/src/src.cpp)
source_group (TREE ${PROJECT_SOURCE_DIR}/include/boost/url PREFIX url FILES ${PROJECT_FILES})

GroupSources(test "/")

add_definitions(-DBOOST_URL_HEADER_ONLY=1)

include_directories(.)

add_executable (url-tests
    ${PROJECT_FILES}
    ${PROJECT_SOURCE_DIR}/include/boost/url.hpp
    ${PROJECT_SOURCE_DIR}/src/src.cpp
    Jamfile
    main.cpp
    test_suite.hpp
    _detail_char_type.cpp
    _detail_parse.cpp
    basic_url.cpp
    error.cpp
    host_type.cpp
    sandbox.cpp
    scheme.cpp
    static_pool.cpp
    static_url.cpp
    url.cpp
    url_base.cpp
    url_view.cpp
    urls.cpp
)

add_test(url-tests url-tests)
