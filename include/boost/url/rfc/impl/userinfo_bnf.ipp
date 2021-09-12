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
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <boost/url/pct_encoding.hpp>

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
    if(! parse(it, end, ec,
        pct_encoded_bnf<
            masked_char_set<
                unsub_char_mask>>{
                    t.user}))
        return false;
    if( it != end &&
        *it == ':')
    {
        ++it;
        t.password.emplace();
        if(! parse(it, end, ec,
            pct_encoded_bnf<
                masked_char_set<
                    unsub_char_mask |
                    colon_char_mask>>{
                        *t.password}))
            return false;
    }
    else
    {
        t.password.reset();
    }
    t.str = string_view(
        start, it - start);
    return true;
}

} // urls
} // boost

#endif
