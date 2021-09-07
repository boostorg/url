//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODING_HPP
#define BOOST_URL_IMPL_PCT_ENCODING_HPP

#include <boost/url/rfc/detail/pct_encoding.hpp>

namespace boost {
namespace urls {

#if 0

template<class CharSet>
bool
parse_pct_encoded(
    char const* const start,
    char const* const end,
    error_code& ec,
    CharSet const& cs) noexcept
{
    std::size_t needed;
    if(! detail::parse_pct_encoded_impl(
        start, end, ec, cs, needed))
        return false;
    return true;
}

template<class CharSet>
std::size_t
pct_decoded_size(
    string_view s,
    error_code& ec,
    CharSet const& cs) noexcept
{
    std::size_t needed;
    auto it = s.data();
    auto const end = it + s.size();
    if(! detail::parse_pct_encoded_impl(
        s.data(), end, ec, cs, needed))
        return 0;
    if(it != end)
    {
        // char must be pct-encoded
        ec = error::illegal_reserved_char;
        return 0;
    }
    return needed;
}

#endif

} // urls
} // boost

#endif
