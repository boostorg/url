//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_PCT_ENCODING_IPP
#define BOOST_URL_DETAIL_IMPL_PCT_ENCODING_IPP

#include <boost/url/detail/pct_encoding.hpp>
#include <boost/assert.hpp>
#include <cstring>

namespace boost {
namespace urls {
namespace detail {

// returns true if plain_key == decode(encoded)
bool
key_equal_encoded(
    string_view plain_key,
    pct_encoded_str encoded) noexcept
{
    if(plain_key.size() !=
        encoded.decoded_size)
        return false; // trivial reject
    if(encoded.str.size() ==
        encoded.decoded_size)
    {
        // plain comparison
        return std::memcmp(
            plain_key.data(),
            encoded.str.data(),
            plain_key.size()) == 0;
    }
    auto it0 = plain_key.data();
    auto it1 = encoded.str.data();
    auto const end0 = it0 + plain_key.size();
    auto const end1 = it1 + encoded.str.size();
    for(;;)
    {
        if(it1 == end1)
            return it0 == end0;
        if(it0 == end0)
            return false;
        if(*it1 != '%')
        {
            if(*it1++ != *it0++)
                return false;
            continue;
        }
        // VFALCO Was
        // BOOST_ASSERT(end1 - it1 >= 3);
        if(end1 - it1 < 3)
            return false;
        auto const ch = static_cast<char>(
            (static_cast<unsigned char>(
                bnf::hexdig_value(it1[1])) << 4) +
            static_cast<unsigned char>(
                bnf::hexdig_value(it1[2])));
        if(ch != *it0++)
            return false;
        it1 += 3;
    }
}

//------------------------------------------------

// VFALCO This is here for the cases
// where we don't store the decoded size
std::size_t
pct_decode_size_unchecked(
    string_view s) noexcept
{
    auto it = s.data();
    auto const end =
        it + s.size();
    std::size_t n = 0;
    while(it != end)
    {
        if(*it != '%')
        {
            // unescaped
            ++it;
            ++n;
            continue;
        }
        BOOST_ASSERT(
            end - it >= 3);
        it += 3;
        ++n;
    }
    return n;
}

// Preconditions:
// * valid encoding for options
// * [first, last) is exactly big enough
void
pct_decode_unchecked(
    char* dest,
    char const* last,
    string_view s,
    pct_decode_opts const& opt) noexcept
{
    auto it = s.data();
    auto const end = it + s.size();
    error_code ec;
    if(! opt.plus_to_space)
    {
        while(it != end)
        {
            BOOST_ASSERT(dest < last);
            if(*it == '%')
            {
                // escaped
                ++it;
                *dest++ = decode_hex_octet(
                    it, end, ec);
                BOOST_ASSERT(! ec.failed());
                continue;
            }
            // unescaped
            *dest++ = *it++;
        }
        return;
    }

    // plus decodes to space
    while(it != end)
    {
        BOOST_ASSERT(dest < last);
        if(*it == '+')
        {
            // plus to space
            *dest++ = ' ';
            ++it;
            continue;
        }
        if(*it == '%')
        {
            // escaped
            ++it;
            *dest++ = decode_hex_octet(
                it, end, ec);
            BOOST_ASSERT(! ec.failed());
            continue;
        }
        // unescaped
        *dest++ = *it++;
    }
}

} // detail
} // urls
} // boost

#endif
