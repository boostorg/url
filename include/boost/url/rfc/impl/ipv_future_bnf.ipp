//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_IPV_FUTURE_BNF_IPP
#define BOOST_URL_IMPL_IPV_FUTURE_BNF_IPP

#include <boost/url/rfc/ipv_future_bnf.hpp>
#include <boost/url/bnf/charset.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/bnf/token.hpp>
#include <boost/url/rfc/charsets.hpp>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    ipv_future_bnf& t)
{
    auto const start = it;
    struct minor_chars_t : bnf::lut_chars
    {
        constexpr
        minor_chars_t() noexcept
            : bnf::lut_chars(
                unreserved_chars +
                subdelim_chars + ':')
        {
        }
    };
    bnf::token<bnf::hexdig_chars_t> major;
    bnf::token<minor_chars_t> minor;
    if(! parse(it, end, ec,
        'v',
        major,
        '.',
        minor))
        return false;
    t.major = *major;
    t.minor = *minor;
    if(t.major.empty())
    {
        // can't be empty
        ec = BOOST_URL_ERR(
            error::bad_empty_element);
        return false;
    }
    if(t.minor.empty())
    {
        // can't be empty
        ec = BOOST_URL_ERR(
            error::bad_empty_element);
        return false;
    }
    t.str = string_view(
        start, it - start);
    return true;
}

} // urls
} // boost

#endif
