//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_USERINFO_BNF_IPP
#define BOOST_URL_IMPL_USERINFO_BNF_IPP

#include <boost/url/rfc/userinfo_bnf.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/literal.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <boost/url/rfc/pct_encoding.hpp>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    userinfo_bnf& t)
{
    using bnf::parse;
    auto const start = it;
    pct_encoded_str user;
    pct_encoded_str pass;
    optional<
        bnf::literal<':'>> colon;
    if(! parse(it, end, ec,
        pct_encoded_bnf<
            unsub_char_mask>{user},
        colon,
        pct_encoded_bnf<
            unsub_char_mask |
            colon_char_mask>{pass}))
        return false;
    t.str = string_view(
        start, it - start);
    t.username = user;
    if(colon.has_value())
        t.password = pass;
    else
        t.password.reset();
    return true;
}

} // urls
} // boost

#endif
