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
#include <boost/url/grammar/parse_tag.hpp>

namespace boost {
namespace urls {

template<class CharSet>
void
pct_encoded_rule<CharSet>::
parse(
    char const*& it,
    char const* end,
    error_code& ec,
    pct_encoded_rule& t) noexcept
{
    auto const start = it;
    static constexpr CharSet cs{};
    // VFALCO TODO
    // opt.plus_to_space?
    std::size_t n = 0;
    char const* it0;
skip:
    it0 = it;
    it = grammar::find_if_not(
        it0, end, cs);
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
            ec = grammar::error::syntax;
            return;
        }
        char r;
        if(!grammar::hexdig_value(*it, r))
        {
            // expected HEXDIG
            ec = grammar::error::syntax;
            return;
        }
        ++it;
        if(it == end)
        {
            // missing HEXDIG
            ec = grammar::error::syntax;
            return;
        }
        if(!grammar::hexdig_value(*it, r))
        {
            // expected HEXDIG
            ec = grammar::error::syntax;
            return;
        }
        ++n;
        ++it;
        if(it == end)
            break;
        if(*it != '%')
            goto skip;
    }
finish:
    t.s.str = string_view(
        start, it - start);
    t.s.decoded_size = n;
}

} // urls
} // boost

#endif
