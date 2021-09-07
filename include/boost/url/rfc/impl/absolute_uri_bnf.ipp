//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_ABSOLUTE_URI_BNF_IPP
#define BOOST_URL_IMPL_ABSOLUTE_URI_BNF_IPP

#include <boost/url/rfc/absolute_uri_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/fragment_bnf.hpp>
#include <boost/url/rfc/hier_part_bnf.hpp>
#include <boost/url/rfc/query_bnf.hpp>
#include <boost/url/rfc/scheme_bnf.hpp>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    absolute_uri_bnf& t)
{
    using bnf::parse;

    // scheme ":"
    if(! parse(it, end, ec,
        t.scheme, ':'))
        return false;

    // hier-part
    hier_part_bnf hp;
    if(! parse(it, end, ec, hp))
        return false;
    t.authority = hp.authority;
    t.path = hp.path;

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

    return true;
}

} // urls
} // boost

#endif
