//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_URI_IPP
#define BOOST_URL_BNF_IMPL_URI_IPP

#include <boost/url/rfc/uri.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/fragment_part.hpp>
#include <boost/url/rfc/hier_part.hpp>
#include <boost/url/rfc/query_part.hpp>
#include <boost/url/rfc/scheme_part.hpp>

namespace boost {
namespace urls {
namespace rfc {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    uri& t)
{
    using bnf::parse;

    // scheme ":"
    if(! parse(it, end, ec,
        t.scheme, ':'))
        return false;

    // hier-part
    hier_part h;
    if(! parse(it, end, ec, h))
        return false;
    t.authority = h.authority;
    t.path = h.path;

    // [ "?" query ]
    if(! parse(it, end, ec,
        query_part{t.query}))
        return false;

    // [ "#" fragment ]
    if(! parse(it, end, ec,
        fragment_part{t.fragment}))
        return false;

    return true;
}

} // rfc
} // urls
} // boost

#endif
