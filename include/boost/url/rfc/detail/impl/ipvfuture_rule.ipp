//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_IPVFUTURE_RULE_IPP
#define BOOST_URL_DETAIL_IMPL_IPVFUTURE_RULE_IPP

#include <boost/url/rfc/detail/ipvfuture_rule.hpp>
#include <boost/url/error.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/token_rule.hpp>
#include <boost/url/grammar/sequence_rule.hpp>

namespace boost {
namespace urls {
namespace detail {

auto
ipvfuture_rule_t::
parse(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<value_type>
{
    static constexpr auto
        minor_chars = 
            unreserved_chars +
            subdelim_chars + ':';
    auto const it0 = it;
    auto rv = grammar::parse(
        it, end,
        grammar::sequence_rule(
            grammar::char_rule('v'),
            grammar::token_rule(
                grammar::hexdig_chars),
            grammar::char_rule('.'),
            grammar::token_rule(minor_chars)));
    if(! rv)
        return rv.error();
    value_type t;
    t.major = std::get<1>(*rv);
    t.minor = std::get<3>(*rv);
    if(t.major.empty())
    {
        // can't be empty
        return urls::error::bad_empty_element;
    }
    if(t.minor.empty())
    {
        // can't be empty
        return urls::error::bad_empty_element;
    }
    t.str = string_view(
        it0, it - it0);
    return t;
}

} // detail
} // urls
} // boost

#endif
