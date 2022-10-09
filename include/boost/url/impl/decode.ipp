//
// Copyright (c) 2019 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_IMPL_DECODE_IPP
#define BOOST_URL_IMPL_DECODE_IPP

#include <boost/url/decode.hpp>

namespace boost {
namespace urls {

std::size_t
decode(
    char* dest,
    char const* end,
    BOOST_URL_PCT_STRING_VIEW s,
    decode_opts const& opt)
{
    auto const n =
        detail::decode_unchecked(
            dest, end, s, opt);
    if(n < s.decoded_size())
        return 0;
    return n;
}

} // urls
} // boost

#endif
