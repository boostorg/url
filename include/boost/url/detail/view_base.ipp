//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_VIEW_BASE_IPP
#define BOOST_URL_DETAIL_VIEW_BASE_IPP

#include <boost/url/detail/view_base.hpp>
#include <boost/url/error.hpp>

namespace boost {
namespace url {
namespace detail {

auto
view_base::
id_segment(
    int index) const ->
    id_type
{
    if(index < 0)
    {
        index = -index;
        if(static_cast<id_type>(
            index) > nseg_)
            out_of_range::raise();
        return static_cast<id_type>(
            id_query() - index);
    }

    if(static_cast<id_type>(
        index) >= nseg_)
        out_of_range::raise();
    return static_cast<id_type>(
        id_path + index);
}

} // detail
} // url
} // boost

#endif
