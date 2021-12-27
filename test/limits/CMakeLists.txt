#
# Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CPPAlliance/url
#

set(TEST_MAIN ../../extra/test_main.cpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES limits.cpp Jamfile)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../extra PREFIX "_extra" FILES ${TEST_MAIN})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../src PREFIX "_extra" FILES ../../src/src.cpp)
add_executable(boost_url_limits limits.cpp Jamfile ${TEST_MAIN} ../../src/src.cpp)
target_include_directories(boost_url_limits PRIVATE ../../include ../../extra/include ../../..)
target_compile_definitions(boost_url_limits PRIVATE
    BOOST_URL_MAX_SIZE=16
    BOOST_URL_NO_LIB=1
)
target_link_libraries(boost_url_limits PRIVATE
    Boost::align
    Boost::config
    Boost::core
    Boost::exception
    Boost::type_traits
    Boost::system
    Boost::utility
    Boost::variant2)
target_link_libraries(boost_url_limits INTERFACE Boost::url)
add_test(NAME boost_url_limits COMMAND boost_url_limits)
add_dependencies(tests boost_url_limits)

