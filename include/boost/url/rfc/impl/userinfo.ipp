//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_USERINFO_IPP
#define BOOST_URL_RFC_IMPL_USERINFO_IPP

#include <boost/url/rfc/userinfo.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/bnf/literal.hpp>
#include <boost/url/bnf/sequence.hpp>
#include <boost/url/bnf/zero_or_one.hpp>
#include <boost/url/rfc/pct_encoded.hpp>
#include <boost/url/rfc/pct_encoding.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
userinfo::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    // *( unreserved / pct-encoded / sub-delims )
    constexpr auto cs_nc_mask =
        unreserved_char_mask +
        sub_delims_char_mask;

    // *( unreserved / pct-encoded / sub-delims / ":" )
    constexpr auto cs_mask =
        unreserved_char_mask +
        sub_delims_char_mask +
        colon_char_mask;

    bnf::sequence<
        pct_encoded<cs_nc_mask>,
        bnf::zero_or_one<bnf::literal<':'>>,
        bnf::zero_or_one<pct_encoded<cs_mask>>> p;
    auto it = p.parse(
        start, end, ec);
    if(ec)
        return start;

    v_.userinfo_ = string_view(start, it - start);
    v_.user_ = get<0>(p)->str();
    v_.password_ = get<2>(p)->value_or(
        pct_encoded_base::value_type()).str();
    return it;
}

} // rfc
} // urls
} // boost

#endif
