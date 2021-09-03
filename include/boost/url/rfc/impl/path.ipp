//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PATH_IPP
#define BOOST_URL_RFC_IMPL_PATH_IPP

#include <boost/url/rfc/path.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
path::
begin(
    char const* const start,
    char const* const end,
    error_code& ec,
    value_type& t)
{
    if(start == end)
    {
        ec = error::end;
        return start;
    }
    auto it = start;
    if(*it == '/')
        ++it;
    pct_encoded<
        unreserved_char_mask |
        sub_delims_char_mask |
        colon_char_mask |
        at_char_mask> p;
    it = parse(
        it, end, ec, p);
    if(ec)
        return start;
    t = p.value();
    return it;
}

char const*
path::
increment(
    char const* const start,
    char const* const end,
    error_code& ec,
    value_type& t)
{
    if(start == end)
    {
        ec = error::end;
        return start;
    }
    auto it = start;
    if(*it != '/')
    {
        // VFALCO should
        // never get here
        ec = error::end;
        return start;
    }
    ++it;
    pct_encoded<
        unreserved_char_mask |
        sub_delims_char_mask |
        colon_char_mask |
        at_char_mask> p;
    it = parse(
        it, end, ec, p);
    if(ec)
        return start;
    t = p.value();
    return it;
}

} // rfc
} // urls
} // boost

#endif
