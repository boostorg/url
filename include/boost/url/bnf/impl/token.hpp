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
bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    token<CharSet> const& t)
{
    auto const start = it;
    it = find_if_not(
        it, end, CharSet{});
    t.v = string_view(
        start, it - start);
    ec = {};
    return true;
}

} // bnf
} // urls
} // boost

#endif
