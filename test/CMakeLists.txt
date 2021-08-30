#
# Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
# Copyright (c) 2021 DMitry Arkhipov (grisumbras@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CPPAlliance/url
#


if(NOT TARGET tests)
    add_custom_target(tests)
    set_property(TARGET tests PROPERTY FOLDER _deps)
endif()


file(GLOB_RECURSE BOOST_URL_TESTS_FILES CONFIGURE_DEPENDS Jamfile *.cpp *.hpp)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES ${BOOST_URL_TESTS_FILES})
add_executable(boost_url_tests ${BOOST_URL_TESTS_FILES})
target_include_directories(boost_url_tests PRIVATE . ./include)
target_link_libraries(boost_url_tests PRIVATE Boost::url)
add_test(NAME boost_url_tests COMMAND boost_url_tests)
add_dependencies(tests boost_url_tests)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../src PREFIX "" FILES ../src/src.cpp)
