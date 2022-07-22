//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_ABSOLUTE_URI_RULE_IPP
#define BOOST_URL_IMPL_ABSOLUTE_URI_RULE_IPP

#include <boost/url/rfc/absolute_uri_rule.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

auto
absolute_uri_rule_t::
parse(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<value_type>
{
    value_type t;

    // scheme
    {
        auto rv = grammar::parse(
            it, end, scheme_part_rule());
        if(! rv)
            return rv.error();
        t.scheme_part = *rv;
    }

    // hier_part
    {
        auto rv = grammar::parse(
            it, end, hier_part_rule);
        if(! rv)
            return rv.error();
        t.hier_part = *rv;
    }

    // [ "?" query ]
    {
        auto rv = grammar::parse(
            it, end, query_part_rule);
        if(! rv)
            return rv.error();
        t.query_part = *rv;
    }

    return t;
}

} // urls
} // boost

#endif
