#
# Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/url
#

cmake_minimum_required(VERSION 3.5...3.16)

set(BOOST_URL_VERSION 1)
if(BOOST_SUPERPROJECT_VERSION)
    set(BOOST_URL_VERSION ${BOOST_SUPERPROJECT_VERSION})
endif()

project(boost_url VERSION "${BOOST_URL_VERSION}" LANGUAGES CXX)

option(BOOST_URL_STANDALONE "Build boost::url as a standalone library" OFF)
option(BOOST_URL_BUILD_TESTS "Build boost::url tests" ON)
option(BOOST_URL_BUILD_EXAMPLES "Build boost::url examples" ON)

file(GLOB_RECURSE BOOST_URL_HEADERS $<$<VERSION_GREATER_EQUAL:${CMAKE_VERSION},3.12>:CONFIGURE_DEPENDS>
    include/boost/*.hpp
    include/boost/*.ipp
    include/boost/*.natvis
)

set(BOOST_URL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/src.cpp
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/boost PREFIX "" FILES ${BOOST_URL_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "" FILES ${BOOST_URL_SOURCES})

# TODO: For Boost superproject, do we want to support header-only mode?
#       Then, this needs to read `add_library(boost_url INTERFACE)`
#       and related settings need to be INTERFACE-ed as well.
add_library(boost_url ${BOOST_URL_HEADERS} ${BOOST_URL_SOURCES})
add_library(Boost::url ALIAS boost_url)

target_compile_features(boost_url PUBLIC cxx_constexpr)

# TODO: For Boost superproject, this may need to be INTERFACE setting.
include(GNUInstallDirs)
target_include_directories(boost_url
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_compile_definitions(boost_url PUBLIC BOOST_URL_NO_LIB=1)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(boost_url PUBLIC BOOST_URL_DYN_LINK=1)
else()
    target_compile_definitions(boost_url PUBLIC BOOST_URL_STATIC_LINK=1)
endif()

if(BOOST_URL_STANDALONE)
    #
    # Building out of Boost superproject tree, without Boost as dependency.
    # e.g. for packaging or added with add_subdirectory.
    #
    target_compile_definitions(boost_url PUBLIC BOOST_URL_STANDALONE)
    target_compile_features(boost_url PUBLIC cxx_std_17)

elseif(BOOST_SUPERPROJECT_VERSION)
    #
    # Building as part of Boost superproject tree, with Boost as dependency.
    #
    # TODO: This CMake support for is currently experimental.
    #       This needs to be reviewed, tested
    target_link_libraries(boost_url
        PUBLIC
            Boost::assert
            Boost::config
            Boost::container
            Boost::core
            Boost::exception
            Boost::system
            Boost::utility
    )

    include(BoostInstall)
    boost_install(TARGETS boost_url HEADER_DIRECTORY include/)

elseif(BOOST_URL_IN_BOOST_TREE)
    #
    # Building inside Boost tree, out of Boost superproject tree, with Boost as dependency.
    # e.g. on Travis or other CI, or when producing Visual Studio Solution and Projects.
    #
    get_filename_component(BOOST_ROOT ../.. ABSOLUTE)
    target_include_directories(boost_url PUBLIC ${BOOST_ROOT})
    target_link_directories(boost_url PUBLIC ${BOOST_ROOT}/stage/lib)

else()
    #
    # Building out of Boost tree, out of Boost superproject tree, with Boost as dependency.
    # e.g. for packaging or added with add_subdirectory.
    #
    find_package(Boost REQUIRED COMPONENTS system)
    target_link_libraries(boost_url
        PUBLIC
            Boost::system
    )
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND NOT BOOST_URL_IN_BOOST_TREE)
    set_target_properties(boost_url PROPERTIES EXPORT_NAME url)
    install(TARGETS boost_url EXPORT boost_url_targets)

    install(EXPORT boost_url_targets
        FILE boost_url-targets.cmake
        NAMESPACE Boost::
        DESTINATION lib/cmake/boost_url
    )

    include(CMakePackageConfigHelpers)

    configure_package_config_file(cmake/config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/boost_url-config.cmake
        INSTALL_DESTINATION lib/cmake/boost_url
    )

    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/boost_url-config-version.cmake
        VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/boost_url-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/boost_url-config-version.cmake
        DESTINATION lib/cmake/boost_url
    )

    install(DIRECTORY include/ DESTINATION include)
endif()

if(BOOST_URL_BUILD_TESTS)
    include(CTest)
    add_subdirectory(test)
endif()

if(BOOST_URL_BUILD_EXAMPLES AND NOT BOOST_SUPERPROJECT_VERSION)
    add_subdirectory(example)
endif()
