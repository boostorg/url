//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_RELATIVE_REF_BNF_IPP
#define BOOST_URL_RFC_IMPL_RELATIVE_REF_BNF_IPP

#include <boost/url/rfc/relative_ref_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/fragment_bnf.hpp>
#include <boost/url/rfc/query_bnf.hpp>
#include <boost/url/rfc/relative_part_bnf.hpp>

namespace boost {
namespace urls {
namespace rfc {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    relative_ref_bnf& t)
{
    using bnf::parse;

    // relative-part
    relative_part_bnf rp;
    if(! parse(it, end, ec, rp))
        return false;
    t.authority = rp.authority;
    t.path = rp.path;

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
