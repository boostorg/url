#
# Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CPPAlliance/url
#

source_group("" FILES
        route.cpp
        )

add_executable(route
        route.cpp
        )

set_property(TARGET route PROPERTY FOLDER "boost_url_examples")
target_link_libraries(route PRIVATE Boost::url Boost::filesystem)
