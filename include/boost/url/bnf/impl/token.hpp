//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_TOKEN_HPP
#define BOOST_URL_BNF_IMPL_TOKEN_HPP

#include <boost/url/bnf/char_set.hpp>

namespace boost {
namespace urls {
namespace bnf {

template<class CharSet>
char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    token<CharSet> const& t)
{
    auto it = find_if_not(
        start, end, CharSet{});
    t.v = string_view(
        start, it - start);
    ec = {};
    return it;
}

} // bnf
} // urls
} // boost

#endif
