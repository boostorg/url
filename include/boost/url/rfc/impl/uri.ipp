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
#include <boost/url/rfc/fragment.hpp>
#include <boost/url/rfc/hier_part.hpp>
#include <boost/url/rfc/query.hpp>
#include <boost/url/rfc/scheme.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    uri& t)
{
    using bnf::parse;
    auto it = start;

    // scheme ":"
    it = parse(it, end, ec,
        scheme{t.scheme}, ':');
    if(ec)
        return start;

    // hier-part
    hier_part h;
    it = parse(
        it, end, ec, h);
    if(ec)
        return start;
    t.authority = h.authority;
    t.path = h.path;

    // [ "?" query ]
    it = parse(
        it, end, ec, '?');
    if(! ec.failed())
    {
        t.query.emplace();
        it = parse(it, end, ec,
            query{*t.query});
        if(ec)
            return start;
    }
    else
    {
        ec = {};
        t.query.reset();
    }

    // [ "#" fragment ]
    it = parse(
        it, end, ec, '#');
    if(! ec.failed())
    {
        t.fragment.emplace();
        it = parse(it, end, ec,
            fragment{*t.fragment});
        if(ec)
            return start;
    }
    else
    {
        ec = {};
        t.fragment.reset();
    }

    return it;
}

} // rfc
} // urls
} // boost

#endif
