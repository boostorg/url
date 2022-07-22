//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_HIER_PART_RULE_IPP
#define BOOST_URL_IMPL_HIER_PART_RULE_IPP

#include <boost/url/rfc/hier_part_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/paths_rule.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

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
        return t;
    }
    if(it[0] != '/')
    {
        // path-rootless
        auto const it0 = it;
        auto rv = grammar::parse(
            it, end,
            path_rootless_rule{});
        if(rv.has_value())
        {
            auto const& v = *rv;
            t.path.path = v.string();
            t.path.count = v.size();
            return t;
        }
        it = it0;

        // path-empty
        return t;
    }
    if( end - it == 1 ||
        it[1] != '/')
    {
        // path-absolute
        auto rv = grammar::parse(
            it, end,
            path_absolute_rule{});
        if(! rv)
            return rv.error();
        auto const& p = *rv;
        t.path.path = p.string();
        t.path.count = p.size();
        t.has_authority = false;
        return t;
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
            it, end, path_abempty_rule{});
        if(! rv)
            return rv.error();

        auto const& v = *rv;
        t.path.path = v.string();
        t.path.count = v.size();
        t.has_authority = true;
        return t;
    }
}

} // urls
} // boost

#endif
