//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SCHEME_RULE_IPP
#define BOOST_URL_IMPL_SCHEME_RULE_IPP

#include <boost/url/rfc/scheme_rule.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    scheme_rule& t) noexcept
{
    auto const start = it;
    if(it == end)
    {
        // expected alpha
        ec = grammar::error::incomplete;
        return;
    }
    if(! grammar::alpha_chars(*it))
    {
        // expected alpha
        ec = error::bad_alpha;
        return;
    }
    static
    constexpr
    grammar::lut_chars scheme_chars(
        "0123456789" "+-."
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz");
    it = grammar::find_if_not(
        it + 1, end, scheme_chars);
    t.scheme = string_view(
        start, it - start);
    t.scheme_id = string_to_scheme(
        t.scheme);
}

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    scheme_part_rule& t) noexcept
{
    auto const start = it;
    scheme_rule t0;
    if(! grammar::parse(
        it, end, ec, t0, ':'))
        return;
    t.scheme = t0.scheme;
    t.scheme_id = t0.scheme_id;
    t.scheme_part = string_view(
        start, it - start);
}

} // urls
} // boost

#endif
