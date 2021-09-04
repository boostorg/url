//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_QUERY_IPP
#define BOOST_URL_RFC_IMPL_QUERY_IPP

#include <boost/url/rfc/query.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/detail/paths.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
query::
begin(
    char const* const start,
    char const* const end,
    error_code& ec,
    query_param& t) noexcept
{
    using bnf::parse;
    pct_encoded<
        query_char_mask> k;
    auto it = parse(
        start, end, ec, k);
    if(ec)
        return start;
    it = parse(
        it, end, ec, '=');
    if(ec.failed())
    {
        if(it != start)
        {
            // key with no value
            ec = {};
            t.key = k.value();
            t.value.reset();
            return it;
        }
        // empty list
        ec = error::end;
        return start;
    }
    pct_encoded<
        query_char_mask> v;
    it = parse(it, end, ec, v);
    if(ec)
    {
        // VFALCO what about the key?
        return start;
    }
    t.key = k.value();
    t.value.emplace(v.value());
    return it;
}

char const*
query::
increment(
    char const* const start,
    char const* const end,
    error_code& ec,
    query_param& t) noexcept
{
    using bnf::parse;
    auto it = parse(
        start, end, ec, '&');
    if(ec)
    {
        // end of list
        ec = error::end;
        return start;
    }
    auto it0 = it;
    it = begin(it, end, ec, t);
    if(ec == error::end)
    {
        // empty param
        ec = {};
        t.key = {};
        t.value.reset();
        return it0;
    }
    if(ec)
        return start;
    return it;
}

} // rfc
} // urls
} // boost

#endif
