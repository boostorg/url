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

query_param::
query_param(
    char const* s,
    std::size_t nk,
    std::size_t const nv,
    const_string::factory const& a)
{
    if(nk + nv == 0)
    {
        has_value = false;
        return;
    }
    // key
    string_view ek{s, nk};
    auto n =
        pct_decode_bytes_unchecked(ek);
    key = a(n, [nk, ek]
        (std::size_t, char* dest)
        {
            pct_decode_unchecked(
                dest, dest + nk, ek);
        });
    if(nv > 0)
    {
        // value
        BOOST_ASSERT(s[nk] == '=');
        has_value = true;
        string_view ev{
            s + nk + 1, nv - 1 };
        n = pct_decode_bytes_unchecked(ev);
        value = a(n, [ev]
            (std::size_t n, char* dest)
            {
                pct_decode_unchecked(
                    dest, dest + n, ev);
            });
    }
    else
    {
        has_value = false;
    }
}

} // urls
} // boost

#endif
