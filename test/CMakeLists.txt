#
# Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/url
#

file(GLOB_RECURSE BOOST_JSON_TESTS_FILES CONFIGURE_DEPENDS Jamfile *.cpp *.hpp)
list(FILTER BOOST_JSON_TESTS_FILES EXCLUDE REGEX "^${CMAKE_CURRENT_SOURCE_DIR}/cmake_install_test/.*$")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES ${BOOST_JSON_TESTS_FILES})
add_executable(tests ${BOOST_JSON_TESTS_FILES})
target_include_directories(tests PRIVATE .)
target_link_libraries(tests PRIVATE Boost::url)
add_test(NAME url-tests COMMAND tests)
