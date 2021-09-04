//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PCT_ENCODING_IPP
#define BOOST_URL_RFC_IMPL_PCT_ENCODING_IPP

namespace boost {
namespace urls {
namespace rfc {

std::size_t
pct_decoded_size_unchecked(
    string_view s) noexcept
{
    std::size_t n = 0;
    auto it = s.data();
    auto const end =
        it + s.size();
    while(it != end)
    {
        auto c = *it;
        if(c != '%')
        {
            ++it;
            ++n;
            continue;
        }
        it += 3;
        ++n;
    }
    return n;
}

char*
pct_decode_unchecked(
    char* dest,
    string_view s) noexcept
{
    auto it = s.data();
    auto const end =
        it + s.size();
    while(it != end)
    {
        if(*it != '%')
        {
            *dest++ = *it++;
            continue;
        }
        *dest++ = static_cast<char>(
            (static_cast<unsigned char>(
                bnf::hexdig_value(it[1])) << 4) +
            static_cast<unsigned char>(
                bnf::hexdig_value(it[2])));
        it += 3;
    }
    return dest;
}

} // rfc
} // urls
} // boost

#endif
