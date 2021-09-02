//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PCT_ENCODED_HPP
#define BOOST_URL_RFC_IMPL_PCT_ENCODED_HPP

#include <boost/url/rfc/char_sets.hpp>

namespace boost {
namespace urls {
namespace rfc {

template<std::uint8_t CharMask>
char const*
pct_encoded<CharMask>::
parse(
    char const* const start,
    char const* const end,
    error_code& ec) noexcept
{
    masked_char_set<CharMask> cs;
    auto it = parse_pct_encoded(
        cs, start, end, ec);
    if(ec)
        return start;
    v_.s_ = string_view(
        start, it - start);
    return it;
}

} // rfc
} // urls
} // boost

#endif
