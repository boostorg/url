//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_GRAMMAR_DEC_OCTET_RULE_IPP
#define BOOST_URL_IMPL_GRAMMAR_DEC_OCTET_RULE_IPP

#include <boost/url/grammar/dec_octet_rule.hpp>
#include <boost/url/grammar/charset.hpp>

namespace boost {
namespace urls {
namespace grammar {

auto
dec_octet_rule_t::
parse(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<value_type>
{
    if(it == end)
    {
        // expected DIGIT
        return error::incomplete;
    }
    if(! digit_chars(*it))
    {
        // not a digit
        return error::syntax;
    }
    unsigned v = *it - '0';
    ++it;
    if( it == end ||
        ! digit_chars(*it))
        return static_cast<
            value_type>(v);
    if(v == 0)
    {
        // bad leading '0'
        return error::syntax;
    }
    v = (10 * v) + *it - '0';
    ++it;
    if( it == end ||
        ! digit_chars(*it))
        return static_cast<
            value_type>(v);
    if(v > 25)
    {
        // out of range
        return error::syntax;
    }
    v = (10 * v) + *it - '0';
    if(v > 255)
    {
        // out of range
        return error::syntax;
    }
    ++it;
    return static_cast<
        value_type>(v);
}

} // grammar
} // urls
} // boost

#endif
