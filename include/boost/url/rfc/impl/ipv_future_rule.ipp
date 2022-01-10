//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_IPV_FUTURE_RULE_IPP
#define BOOST_URL_IMPL_IPV_FUTURE_RULE_IPP

#include <boost/url/rfc/ipv_future_rule.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/token_rule.hpp>

namespace boost {
namespace urls {

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    ipv_future_rule& t) noexcept
{
    auto const start = it;
    struct minor_chars_t
        : grammar::lut_chars
    {
        constexpr
        minor_chars_t() noexcept
            : grammar::lut_chars(
                unreserved_chars +
                subdelim_chars + ':')
        {
        }
    };
    grammar::token_rule<
        grammar::hexdig_chars_t> major;
    grammar::token_rule<
        minor_chars_t> minor;
    if(! grammar::parse(
        it, end, ec,
            'v',
            major,
            '.',
            minor))
        return;
    t.major = *major;
    t.minor = *minor;
    if(t.major.empty())
    {
        // can't be empty
        ec = error::bad_empty_element;
        return;
    }
    if(t.minor.empty())
    {
        // can't be empty
        ec = error::bad_empty_element;
        return;
    }
    t.str = string_view(
        start, it - start);
}

} // urls
} // boost

#endif
