//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_SEGMENT_IPP
#define BOOST_URL_RFC_IMPL_SEGMENT_IPP

#include <boost/url/rfc/segment.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    segment& t)
{
    pct_encoded<
        unreserved_char_mask +
        sub_delims_char_mask +
        colon_char_mask +
        at_char_mask> ts;
    auto it = parse(
        start, end, ec, ts);
    if(ec)
        return start;
    t.v_ = ts.value();
    return it;
}

char const*
segment::
parse(
    char const* start,
    char const* end,
    error_code& ec)
{
    using bnf::parse;
    return rfc::parse(
        start, end, ec, *this);
}

char const*
segment_nz::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    pct_encoded<
        unreserved_char_mask +
        sub_delims_char_mask +
        colon_char_mask +
        at_char_mask> p;
    auto it = p.parse(
        start, end, ec);
    if(ec)
        return start;
    if(it == start)
    {
        // expected pchar
        ec = error::syntax;
        return start;
    }
    v_.s_ = p->str();
    return it;
}

char const*
segment_nz_nc::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    pct_encoded<
        unreserved_char_mask +
        sub_delims_char_mask +
        at_char_mask> p;
    auto it = p.parse(
        start, end, ec);
    if(ec)
        return start;
    if(it == start)
    {
        // expected pchar
        ec = error::syntax;
        return start;
    }
    v_.s_ = p->str();
    return it;
}

} // rfc
} // urls
} // boost

#endif
