//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_RFC_DETAIL_IMPL_HIER_PART_RULE_IPP
#define BOOST_URL_RFC_DETAIL_IMPL_HIER_PART_RULE_IPP

#include <boost/url/rfc/detail/hier_part_rule.hpp>
#include <boost/url/rfc/detail/path_rules.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {
namespace detail {

auto
hier_part_rule_t::
parse(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<value_type>
{
    value_type t;

    if(it == end)
    {
        // path-empty
        BOOST_URL_RETURN(t);
    }
    if(it[0] != '/')
    {
        // path-rootless
        auto const it0 = it;
        auto rv = grammar::parse(
            it, end,
            path_rootless_rule);
        if( rv )
        {
            t.path = std::move(*rv);
            BOOST_URL_RETURN(t);
        }
        it = it0;

        // path-empty
        BOOST_URL_RETURN(t);
    }
    if( end - it == 1 ||
        it[1] != '/')
    {
        // path-absolute
        auto rv = grammar::parse(
            it, end,
            path_absolute_rule);
        if(! rv)
            return rv.error();
        t.path = std::move(*rv);
        t.has_authority = false;
        BOOST_URL_RETURN(t);
    }

    // "//" authority path-abempty
    it += 2;
    // authority
    {
        auto rv = grammar::parse(
            it, end, authority_rule);
        if(! rv)
            return rv.error();
        t.authority = *rv;
    }

    // path-abempty
    {
        auto rv = grammar::parse(
            it, end, path_abempty_rule);
        if(! rv)
            return rv.error();
        t.path = std::move(*rv);
        t.has_authority = true;
    }

    BOOST_URL_RETURN(t);
}

} // detail
} // urls
} // boost

#endif
