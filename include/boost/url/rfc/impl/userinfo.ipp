//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_USERINFO_IPP
#define BOOST_URL_RFC_IMPL_USERINFO_IPP

#include <boost/url/rfc/userinfo.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded.hpp>
#include <boost/url/rfc/pct_encoding.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
userinfo::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    char const* it;
    // *( unreserved / pct-encoded / sub-delims )
    {
        masked_char_set<
            unreserved_char_mask +
            sub_delims_char_mask> cs;
        it = parse_pct_encoded(
            cs, start, end, ec);
        if(ec)
            return start;
        v_.user_ = string_view(
            start, it - start);
    }
    if( it == end ||
        *it != ':')
    {
        // just user
        v_.userinfo_ = string_view(
            start, it - start);
        v_.password_ = {};
        return it;
    }
    ++it;
    if(it == end)
    {
        // just ( user ':' )
        v_.userinfo_ = string_view(
            start, it - start);
        v_.password_ = {};
        return it;
    }
    // *( unreserved / pct-encoded / sub-delims / ":" )
    {
        // ( user ":" pass )
        masked_char_set<
            unreserved_char_mask +
            sub_delims_char_mask +
            colon_char_mask> cs;
        auto it0 = it;
        it = parse_pct_encoded(
            cs, it0, end, ec);
        if(ec)
            return start;
        v_.userinfo_ = string_view(
            start, it - start);
        v_.password_ = string_view(
            it0, it - it0);
    }
    return it;
}

} // rfc
} // urls
} // boost

#endif
