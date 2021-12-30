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

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    hier_part_rule& t)
{
    using grammar::parse;
    if(it == end)
    {
        // path-empty
        t.path = {};
        t.has_authority = false;
        ec = {};
        return true;
    }
    if(it[0] != '/')
    {
        // path-rootless
        path_rootless_rule t0;
        if(! parse(it, end, ec, t0))
            return false;
        t.path.path = t0.str;
        t.path.count = t0.count;
        t.has_authority = false;
        return true;
    }
    if( end - it == 1 ||
        it[1] != '/')
    {
        // path-absolute
        path_absolute_rule t0;
        if(! parse(it, end, ec, t0))
            return false;
        t.path.path = t0.str;
        t.path.count = t0.count;
        t.has_authority = false;
        return true;
    }
    // "//" authority path-abempty
    it += 2;
    // authority
    if(! parse(it, end, ec,
            t.authority))
        return false;
    // path-abempty
    path_abempty_rule t0;
    if(! parse(it, end, ec, t0))
        return false;
    t.path.path = t0.str;
    t.path.count = t0.count;
    t.has_authority = true;
    return true;
}

} // urls
} // boost

#endif