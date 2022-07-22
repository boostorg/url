//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODED_RULE_HPP
#define BOOST_URL_IMPL_PCT_ENCODED_RULE_HPP

#include <boost/url/pct_encoding.hpp>
#include <boost/url/grammar/charset.hpp>

namespace boost {
namespace urls {

template<class CharSet>
auto
pct_encoded_rule_t<CharSet>::
parse(
    char const*& it,
    char const* end) const noexcept ->
        result<value_type>
{
    auto const start = it;
    // VFALCO TODO
    // opt.plus_to_space?
    std::size_t n = 0;
    char const* it0;
skip:
    it0 = it;
    it = grammar::find_if_not(
        it0, end, cs_);
    n += it - it0;
    if(it == end)
        goto finish;
    if(*it != '%')
        goto finish;
    for(;;)
    {
        ++it;
        if(it == end)
        {
            // missing HEXDIG
            return grammar::error::syntax;
        }
        char r;
        if(!grammar::hexdig_value(*it, r))
        {
            // expected HEXDIG
            return grammar::error::syntax;
        }
        ++it;
        if(it == end)
        {
            // missing HEXDIG
            return grammar::error::syntax;
        }
        if(!grammar::hexdig_value(*it, r))
        {
            // expected HEXDIG
            return grammar::error::syntax;
        }
        ++n;
        ++it;
        if(it == end)
            break;
        if(*it != '%')
            goto skip;
    }
finish:
    return detail::access::construct(
        string_view(start, it - start), n);
}

} // urls
} // boost

#endif
