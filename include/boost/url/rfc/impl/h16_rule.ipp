//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_H16_RULE_HPP
#define BOOST_URL_RFC_IMPL_H16_RULE_HPP

#include <boost/url/rfc/h16_rule.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

auto
h16_rule_t::
parse(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<value_type>
{
    // VFALCO Why was this here?
    //BOOST_ASSERT(it != end);
    if(it == end)
        return grammar::error::syntax;

    std::uint16_t v;
    for(;;)
    {
        char d;
        if(!grammar::hexdig_value(*it, d))
        {
            // not a HEXDIG
            return grammar::error::syntax;
        }
        v = d;
        ++it;
        if(it == end)
            break;
        if(!grammar::hexdig_value(*it, d))
            break;
        v = (16 * v) + d;
        ++it;
        if(it == end)
            break;
        if(!grammar::hexdig_value(*it, d))
            break;
        v = (16 * v) + d;
        ++it;
        if(it == end)
            break;
        if(!grammar::hexdig_value(*it, d))
            break;
        v = (16 * v) + d;
        ++it;
        break;
    }
    return value_type{
        static_cast<
            unsigned char>(v / 256),
        static_cast<
            unsigned char>(v % 256)};
}

} // urls
} // boost

#endif
