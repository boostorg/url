//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_URI_REFERENCE_RULE_IPP
#define BOOST_URL_IMPL_URI_REFERENCE_RULE_IPP

#include <boost/url/rfc/uri_reference_rule.hpp>
#include <boost/url/rfc/hier_part_rule.hpp>
#include <boost/url/rfc/relative_part_rule.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    uri_reference_rule& t) noexcept
{
    auto const start = it;

    // scheme ":"
    if(! grammar::parse(
        it, end, ec,
            t.scheme_part))
    {
        // rewind
        it = start;

        // relative-ref
        relative_part_rule t0;
        if(! grammar::parse(
            it, end, ec,t0))
            return;

        t.has_authority =
            t0.has_authority;
        t.authority = t0.authority;
        t.path = t0.path;
    }
    else
    {
        // hier-part
        hier_part_rule t0;
        if(! grammar::parse(
            it, end, ec, t0))
            return;

        t.has_authority =
            t0.has_authority;
        t.authority = t0.authority;
        t.path = t0.path;
    }

    // [ "?" query ]
    if(! grammar::parse(
        it, end, ec,
            t.query_part))
        return;

    // [ "#" fragment ]
    if(! grammar::parse(
        it, end, ec,
            t.fragment_part))
        return;
}

} // urls
} // boost

#endif
