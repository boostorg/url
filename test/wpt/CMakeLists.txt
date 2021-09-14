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

set(F
    CMakeLists.txt
    main.cpp
    )

if(BOOST_URL_IS_ROOT)
    #
    # Building inside Boost tree, but as a separate project e.g. on Travis or
    # other CI, or when producing Visual Studio Solution and Projects. This is
    # when we have to add dependencies to the project manually.
    set(BUILD_TESTING OFF)
    foreach(lib
        json
    )
        add_subdirectory(../../../${lib} libs/${lib} EXCLUDE_FROM_ALL)
        set(target boost_${lib})
        if(TARGET ${target})
          get_target_property(lib_type ${target} TYPE)
          if(NOT ${lib_type} STREQUAL INTERFACE_LIBRARY)
            set_property(TARGET ${target} PROPERTY FOLDER _deps/${lib})
          endif()
        endif()
    endforeach()
    unset(BUILD_TESTING)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES ${F})
add_executable(boost_wpt_tests ${F})
target_include_directories(boost_wpt_tests PRIVATE . )
target_link_libraries(boost_wpt_tests PRIVATE Boost::url Boost::json)
add_test(NAME boost_url_tests COMMAND boost_url_tests)
add_dependencies(tests boost_url_tests)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../../src PREFIX "" FILES ../../src/src.cpp)
