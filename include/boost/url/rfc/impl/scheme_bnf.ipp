//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SCHEME_BNF_IPP
#define BOOST_URL_IMPL_SCHEME_BNF_IPP

#include <boost/url/rfc/scheme_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    scheme_bnf& t)
{
    auto const start = it;
    if(it == end)
    {
        // expected alpha
        ec = error::incomplete;
        return false;
    }
    if(! bnf::alpha_chars{}(*it))
    {
        // expected alpha
        ec = error::bad_alpha;
        return false;
    }
    struct scheme_chars
    {
        constexpr
        bool
        operator()(char c) const noexcept
        {
            return
                (c >= '0' && c <= '9') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                c == '+' ||
                c == '-' ||
                c == '.';
        }
    };
    it = bnf::find_if_not(
        it + 1, end,
            scheme_chars{});
    t.scheme = string_view(
        start, it - start);
    t.scheme_id = string_to_scheme(
        t.scheme);
    return true;
}

} // urls
} // boost

#endif
