//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_URL_VIEW_HPP
#define BOOST_URL_IMPL_URL_VIEW_HPP

namespace boost {
namespace urls {

//------------------------------------------------
//
// Query
//
//------------------------------------------------

template<class Allocator>
params_view
url_view::
params(
    Allocator const& a) const noexcept
{
    auto s = get(id_query);
    if(s.empty())
        return params_view(
            s, 0, a);
    BOOST_ASSERT(s[0] == '?');
    s.remove_prefix(1);
    return params_view(
        s, nparam_, a);
}

} // urls
} // boost

#endif