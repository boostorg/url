//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODED_BNF_IPP
#define BOOST_URL_IMPL_PCT_ENCODED_BNF_IPP

#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <boost/url/bnf/char_set.hpp>

namespace boost {
namespace urls {

// returns true if plain_key == decode(encoded)
bool
key_equal_encoded(
    string_view plain_key,
    pct_encoded_str encoded) noexcept
{
    // VFALCO should we validate the encoded string?
    if(plain_key.size() > encoded.str.size())
        return false; // trivial reject
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
        BOOST_ASSERT(end1 - it1 >= 3);
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

} // urls
} // boost

#endif
