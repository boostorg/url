//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_DETAIL_PCT_ENCODING_HPP
#define BOOST_URL_RFC_DETAIL_PCT_ENCODING_HPP

#include <boost/url/rfc/hexdig.hpp>
#include <boost/url/error.hpp>
#include <cstdlib>

namespace boost {
namespace urls {
namespace rfc {
namespace detail {

template<class CharSet>
char const*
parse_pct_encoded_impl(
    std::size_t& needed,
    CharSet const& cs,
    char const* const start,
    char const* const end,
    error_code& ec) noexcept
{
    std::size_t n = 0;
    auto it = start;
    char const* it0;
skip:
    it0 = it;
    it = cs.skip(it0, end);
    n += it - it0;
    if(it == end)
        goto finish;
    if(*it != '%')
        goto finish;
    for(;;)
    {
        ++it;
        if(it == end)
        {
            ec = error::need_more;
            return start;
        }
        if(hex_digit(*it) == -1)
        {
            // expected HEXDIG
            ec = error::bad_pct_encoding_digit;
            return start;
        }
        ++it;
        if(it == end)
        {
            ec = error::need_more;
            return start;
        }
        if(hex_digit(*it) == -1)
        {
            // expected HEXDIG
            ec = error::bad_pct_encoding_digit;
            return start;
        }
        ++n;
        ++it;
        if(it == end)
            break;
        if(*it != '%')
            goto skip;
    }
finish:
    needed = n;
    return it;
}

} // detail
} // rfc
} // urls
} // boost

#endif
