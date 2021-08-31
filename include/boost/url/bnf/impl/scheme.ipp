//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_SCHEME_IPP
#define BOOST_URL_BNF_IMPL_SCHEME_IPP

#include <boost/url/bnf/scheme.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/char_sets.hpp>

namespace boost {
namespace urls {
namespace bnf {

bool
is_scheme_char(char c) noexcept
{
    return
        is_alpha(c) ||
        is_digit(c) ||
        c == '+' ||
        c == '-' ||
        c == '.';
}

char const*
scheme::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    auto it = start;
    if(it == end)
    {
        ec = error::need_more;
        return start;
    }
    if(! is_alpha(*it))
    {
        ec = error::syntax;
        return start;
    }
    scheme_char_set cs;
    it = cs.skip(it + 1, end);
    v_ = string_view(
        start, it - start);
    return it;
}

} // bnf
} // urls
} // boost

#endif
