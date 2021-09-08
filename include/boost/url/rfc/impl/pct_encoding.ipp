//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODING_IPP
#define BOOST_URL_IMPL_PCT_ENCODING_IPP

namespace boost {
namespace urls {

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

void
pct_decode_unchecked(
    char* out,
    char* const last,
    string_view s) noexcept
{
    auto it = s.data();
    auto const end =
        it + s.size();
    while(it != end)
    {
        BOOST_ASSERT(out != last);
        if(*it != '%')
        {
            *out++ = *it++;
            continue;
        }
        BOOST_ASSERT(end - it >= 3);
        BOOST_ASSERT(
            bnf::hexdig_value(it[1]) >= 0);
        BOOST_ASSERT(
            bnf::hexdig_value(it[2]) >= 0);
        *out++ = static_cast<char>(
            (static_cast<unsigned char>(
                bnf::hexdig_value(it[1])) << 4) +
            static_cast<unsigned char>(
                bnf::hexdig_value(it[2])));
        it += 3;
    }
}

} // urls
} // boost

#endif
