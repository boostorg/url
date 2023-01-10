#
# Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/boostorg/url
#

source_group("" FILES
        router.cpp
        )

add_executable(router
        router.cpp
        )

set_property(TARGET router PROPERTY FOLDER "Examples")
target_link_libraries(router PRIVATE Boost::url Boost::beast)

# router_test.cpp
set(SUITE_FILES ../../extra/test_main.cpp ../../extra/test_suite.hpp)
add_executable(boost_url_router_test router_test.cpp ${SUITE_FILES})
target_include_directories(boost_url_router_test PRIVATE . ../../extra)
target_link_libraries(boost_url_router_test PRIVATE Boost::url)
add_test(NAME boost_url_router_test COMMAND boost_url_router_test)
add_dependencies(boost_url_all_tests boost_url_router_test)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(boost_url_router_test PUBLIC -Wno-error=unused-but-set-variable)
endif ()