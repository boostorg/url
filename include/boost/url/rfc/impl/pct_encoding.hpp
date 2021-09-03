//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PCT_ENCODING_HPP
#define BOOST_URL_RFC_IMPL_PCT_ENCODING_HPP

#include <boost/url/rfc/detail/pct_encoding.hpp>

namespace boost {
namespace urls {
namespace rfc {

template<class CharSet>
char const*
parse_pct_encoded(
    CharSet const& cs,
    char const* const start,
    char const* const end,
    error_code& ec) noexcept
{
    std::size_t needed;
    auto it = detail::parse_pct_encoded_impl(
        needed, cs, start, end, ec);
    if(ec)
        return start;
    return it;
}

template<class CharSet>
std::size_t
pct_decoded_size(
    CharSet const& cs,
    string_view s,
    error_code& ec) noexcept
{
    std::size_t needed;
    auto const end = s.data() + s.size();
    auto it = detail::parse_pct_encoded_impl(
        needed, cs, s.data(), end, ec);
    if(ec)
        return 0;
    if(it != end)
    {
        // char must be pct-encoded
        ec = error::illegal_reserved_char;
        return 0;
    }
    return needed;
}

} // rfc
} // urls
} // boost

#endif
