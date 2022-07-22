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

auto
relative_ref_rule_t::
parse(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<value_type>
{
    value_type t;

    // relative-part
    {
        auto rv = grammar::parse(
            it, end, relative_part_rule);
        if(! rv)
            return rv.error();
        t.relative_part = *rv;
    }

    // [ "?" query ]
    {
        auto rv = grammar::parse(
            it, end, query_part_rule);
        if(! rv)
            return rv.error();
        t.query_part = *rv;
    }

    // [ "#" fragment ]
    {
        auto rv = grammar::parse(
            it, end, fragment_part_rule);
        if(! rv)
            return rv.error();
        t.fragment_part = *rv;
    }

    return t;
}

} // urls
} // boost

#endif
