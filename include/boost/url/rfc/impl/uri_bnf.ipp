//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_URI_BNF_IPP
#define BOOST_URL_RFC_IMPL_URI_BNF_IPP

#include <boost/url/rfc/uri_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/fragment_bnf.hpp>
#include <boost/url/rfc/hier_part_bnf.hpp>
#include <boost/url/rfc/query_bnf.hpp>
#include <boost/url/rfc/scheme_bnf.hpp>

namespace boost {
namespace urls {
namespace rfc {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    uri_bnf& t)
{
    using bnf::parse;

    // scheme ":"
    if(! parse(it, end, ec,
        t.scheme, ':'))
        return false;

    // hier-part
    hier_part_bnf h;
    if(! parse(it, end, ec, h))
        return false;
    t.authority = h.authority;
    t.path = h.path;

    // [ "?" query ]
    if( it != end &&
        *it == '?')
    {
        ++it;
        t.query.emplace();
        if(! parse(it, end, ec,
            query_bnf{*t.query}))
            return false;
    }
    else
    {
        t.query.reset();
    }

    // [ "#" fragment ]
    if( it != end &&
        *it == '#')
    {
        ++it;
        t.fragment.emplace();
        if(! parse(it, end, ec,
            fragment_bnf{
                *t.fragment}))
            return false;
    }
    else
    {
        t.fragment.reset();
    }

    return true;
}

} // rfc
} // urls
} // boost

#endif
