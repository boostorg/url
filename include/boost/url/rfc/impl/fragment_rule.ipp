//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_FRAGMENT_RULE_IPP
#define BOOST_URL_IMPL_FRAGMENT_RULE_IPP

#include <boost/url/rfc/fragment_rule.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    fragment_rule const& t) noexcept
{
    grammar::parse(it, end, ec,
        pct_encoded_rule(
            fragment_chars, t.v));
}

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    fragment_part_rule& t) noexcept
{
    if( it == end ||
        *it != '#')
    {
        t.has_fragment = false;
        ec = {};
        return;
    }
    auto start = it;
    ++it;
    if(! grammar::parse(it, end, ec, 
        fragment_rule{t.fragment}))
        return;
    t.has_fragment = true;
    t.fragment_part = string_view(
        start, it - start);
}

} // urls
} // boost

#endif
