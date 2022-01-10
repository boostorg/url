//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_URI_RULE_IPP
#define BOOST_URL_IMPL_URI_RULE_IPP

#include <boost/url/rfc/uri_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/fragment_rule.hpp>

namespace boost {
namespace urls {

void
uri_rule::
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    uri_rule& t) noexcept
{
    // scheme ":"
    if(! grammar::parse(
        it, end, ec,
            t.scheme_part))
        return;

    // hier-part
    if(! grammar::parse(
        it, end, ec,
            t.hier_part))
        return;

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
