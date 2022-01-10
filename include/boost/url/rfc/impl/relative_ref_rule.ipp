//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_RELATIVE_REF_RULE_IPP
#define BOOST_URL_IMPL_RELATIVE_REF_RULE_IPP

#include <boost/url/rfc/relative_ref_rule.hpp>
#include <boost/url/rfc/fragment_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/rfc/relative_part_rule.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

void
relative_ref_rule::
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    relative_ref_rule& t) noexcept
{
    // relative-part
    if(! grammar::parse(
        it, end, ec,
            t.relative_part))
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
