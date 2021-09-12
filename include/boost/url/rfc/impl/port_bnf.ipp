//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PORT_BNF_IPP
#define BOOST_URL_IMPL_PORT_BNF_IPP

#include <boost/url/rfc/port_bnf.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/bnf/token.hpp>
#include <boost/static_assert.hpp>
#include <type_traits>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    port_bnf& t)
{
    bnf::digit_chars cs;
    port_bnf::number_type u = 0;
    auto const start = it;
    while(it != end)
    {
        if(! cs(*it))
            break;
        auto u0 = u;
        u = 10 * u + *it - '0';
        if(u < u0)
        {
            // overflow
            it = bnf::find_if_not(
                it, end, cs);
            t.str = string_view(
                start, it - start);
            t.has_number = false;
            ec = {};
            return true;
        }
        ++it;
    }
    t.str = string_view(
        start, it - start);
    if(! t.str.empty())
    {
        t.has_number = true;
        t.number = u;
    }
    else
    {
        t.has_number = false;
    }
    return true;
}

} // urls
} // boost

#endif
