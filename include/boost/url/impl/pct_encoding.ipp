//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODING_IPP
#define BOOST_URL_IMPL_PCT_ENCODING_IPP

#include <boost/url/pct_encoding.hpp>
#include <boost/url/bnf/charset.hpp>
#include <memory>

namespace boost {
namespace urls {

std::size_t
pct_decoded_bytes_unchecked(
    string_view s) noexcept
{
    auto it = s.data();
    auto const end =
        it + s.size();
    std::size_t n = 0;
    while(it < end)
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

void
pct_decode_unchecked(
    char* dest,
    char const* end,
    string_view s,
    pct_decode_opts const& opt) noexcept
{
    auto const decode_hex = [](
        char const* it)
    {
        auto const d0 =
            bnf::hexdig_value(it[0]);
        auto const d1 =
            bnf::hexdig_value(it[1]);
        return static_cast<char>(
            ((static_cast<
                unsigned char>(d0) << 4) +
            (static_cast<
                unsigned char>(d1))));
    };
    auto it = s.data();
    auto const last = it + s.size();

    if(opt.plus_to_space)
    {
        while(it != last)
        {
            // dest too small
            BOOST_ASSERT(dest != end);
            if(dest == end)
                return;
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
                // missing input
                BOOST_ASSERT(
                    last - it >= 2);
                if(last - it < 2)
                {
                    // initialize output
                    std::memset(dest,
                        0, end - dest);
                    return;
                }
                *dest++ = decode_hex(it);
                it += 2;
                continue;
            }
            // unescaped
            *dest++ = *it++;
        }
        return;
    }

    while(it != last)
    {
        // dest too small
        BOOST_ASSERT(dest != end);
        if(dest == end)
            return;
        if(*it == '%')
        {
            // escaped
            ++it;
            // missing input
            BOOST_ASSERT(
                last - it >= 2);
            if(last - it < 2)
            {
                // initialize output
                std::memset(dest,
                    0, end - dest);
                return;
            }
            *dest++ = decode_hex(it);
            it += 2;
            continue;
        }
        // unescaped
        *dest++ = *it++;
    }
}

} // urls
} // boost

#endif
