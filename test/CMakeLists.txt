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

add_subdirectory(limits)
add_subdirectory(unit)
#add_subdirectory(wpt)
