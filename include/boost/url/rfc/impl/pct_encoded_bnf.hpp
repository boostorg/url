//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODED_BNF_HPP
#define BOOST_URL_IMPL_PCT_ENCODED_BNF_HPP

#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/detail/pct_encoding.hpp>

namespace boost {
namespace urls {

template<std::uint8_t CharMask>
bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_bnf<
        CharMask> const& t) noexcept
{
    auto const start = it;
    masked_char_set<CharMask> cs;
    if(! detail::parse_pct_encoded_impl(
        it, end, ec, cs,
            t.v.decoded_size))
        return false;
    t.v.str = string_view(
        start, it - start);
    return true;
}

} // urls
} // boost

#endif
