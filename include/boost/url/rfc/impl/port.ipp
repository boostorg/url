//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PORT_IPP
#define BOOST_URL_RFC_IMPL_PORT_IPP

#include <boost/url/rfc/port.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/bnf/token.hpp>
#include <boost/static_assert.hpp>
#include <type_traits>

namespace boost {
namespace urls {
namespace rfc {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    port& t)
{
    using bnf::parse;
    bnf::digit_chars cs;
    port::number_type u = 0;
    auto it = start;
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
            t.number.reset();
            ec = {};
            return it;
        }
        ++it;
    }
    t.str = string_view(
        start, it - start);
    if(! t.str.empty())
        t.number.emplace(u);
    else
        t.number.reset();
    return it;
}

} // rfc
} // urls
} // boost

#endif
