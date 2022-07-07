#
# Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CPPAlliance/url
#

source_group("" FILES
        path.hpp
        )

add_executable(query
        query.cpp
        )
set_property(TARGET query PROPERTY FOLDER "example")
target_link_libraries(query PRIVATE Boost::url)

add_executable(path
        path.cpp
        )
set_property(TARGET path PROPERTY FOLDER "example")
target_link_libraries(path PRIVATE Boost::url Boost::filesystem)

add_executable(route
        route.cpp
        )
set_property(TARGET route PROPERTY FOLDER "example")
target_link_libraries(route PRIVATE Boost::url Boost::filesystem)

add_executable(router
        router.cpp
        )
set_property(TARGET router PROPERTY FOLDER "example")
target_link_libraries(router PRIVATE Boost::url Boost::filesystem Boost::container)

add_executable(dir_router
        dir_router.cpp
        )
set_property(TARGET dir_router PROPERTY FOLDER "example")
target_link_libraries(dir_router PRIVATE Boost::url Boost::filesystem Boost::container)
