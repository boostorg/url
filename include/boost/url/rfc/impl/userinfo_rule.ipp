//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_USERINFO_RULE_IPP
#define BOOST_URL_IMPL_USERINFO_RULE_IPP

#include <boost/url/rfc/userinfo_rule.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/grammar/lut_chars.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

void
userinfo_rule::
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    userinfo_rule& t) noexcept
{
    struct uchars
        : grammar::lut_chars
    {
        constexpr
        uchars() noexcept
            : lut_chars(
                unreserved_chars +
                subdelim_chars)
        {
        }
    };

    struct pwchars
        : grammar::lut_chars
    {
        constexpr
        pwchars() noexcept
            : lut_chars(
                unreserved_chars +
                subdelim_chars + ':')
        {
        }
    };

    pct_encoded_rule<uchars> t0;
    pct_encoded_rule<pwchars> t1;

    if(! grammar::parse(
        it, end, ec, t0))
        return;
    t.user = t0.s;
    if( it == end ||
        *it != ':')
    {
        t.has_password = false;
        t.password = {};
        return;
    }
    ++it;
    if(! grammar::parse(
        it, end, ec, t1))
        return;
    t.has_password = true;
    t.password = t1.s;
}

} // urls
} // boost

#endif
