#
# Copyright (c) 2022 alandefreitas (alandefreitas@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt
#

cmake_minimum_required(VERSION 3.5...3.16)

project(cmake_subdir_test LANGUAGES CXX)

if(BOOST_CI_INSTALL_TEST)
    find_package(boost_url REQUIRED)
else()
    add_subdirectory(../.. boostorg/url)
    add_subdirectory(../../../throw_exception boostorg/throw_exception)
    add_subdirectory(../../../core boostorg/core)
    add_subdirectory(../../../assert boostorg/assert)
    add_subdirectory(../../../static_assert boostorg/static_assert)
    add_subdirectory(../../../config boostorg/config)
    add_subdirectory(../../../type_traits boostorg/type_traits)
    add_subdirectory(../../../predef boostorg/predef)
    add_subdirectory(../../../mp11 boostorg/mp11)
    add_subdirectory(../../../winapi boostorg/winapi)
    add_subdirectory(../../../container_hash boostorg/container_hash)
    add_subdirectory(../../../move boostorg/move)
    add_subdirectory(../../../intrusive boostorg/intrusive)
    add_subdirectory(../../../variant2 boostorg/variant2)
    add_subdirectory(../../../align boostorg/align)
    add_subdirectory(../../../system boostorg/system)
    add_subdirectory(../../../container boostorg/container)
endif()

add_executable(main main.cpp)
target_link_libraries(main Boost::url)

enable_testing()
add_test(NAME main COMMAND main)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>)
