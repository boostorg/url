//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_QUERY_PARAM_IPP
#define BOOST_URL_IMPL_QUERY_PARAM_IPP

#include <boost/url/query_param.hpp>
#include <boost/url/pct_encoding.hpp>

namespace boost {
namespace urls {

query_param_encoded_view::
query_param_encoded_view(
    char const* s,
    std::size_t nk,
    std::size_t const nv)
{
    if(nk + nv == 0)
    {
        has_value = false;
        return;
    }
    // key;
    key = {s, nk};
    if(nv > 0)
    {
        // value
        BOOST_ASSERT(s[nk] == '=');
        has_value = true;
        value = {s + nk + 1, nv - 1 };
    }
    else
    {
        has_value = false;
    }
}

} // urls
} // boost

#endif
