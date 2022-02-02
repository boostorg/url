#
# Copyright (c) 2021 Alan Freitas (alandefreitas@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/CppAlliance/url
#

source_group("" FILES
    quicklook.cpp
)

#

add_executable(quicklook
        quicklook.cpp
)
set_property(TARGET quicklook PROPERTY FOLDER "example")
target_link_libraries(quicklook PRIVATE Boost::url)

#
