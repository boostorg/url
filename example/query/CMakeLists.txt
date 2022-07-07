#
# Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CPPAlliance/url
#

source_group("" FILES
        query.cpp
        )

add_executable(query
        query.cpp
        )

set_property(TARGET query PROPERTY FOLDER "example")
target_link_libraries(query PRIVATE Boost::url)
