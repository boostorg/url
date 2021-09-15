//
// Copyright (c) 2016-2019 Damian Jarek (damian dot jarek93 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_URL_BNF_TOKEN_HPP
#define BOOST_URL_BNF_TOKEN_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/detail/char_set.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {
namespace bnf {

/** BNF for a series of characters in a char set
*/
template<class CharSet>
struct token
{
    BOOST_STATIC_ASSERT(
        is_char_set<
            CharSet>::value);

    string_view& v;

    template<class CharSet_>
    friend
    bool
    parse(
        char const*& it,
        char const* end,
        error_code& ec,
        token<CharSet_> const& t);
};

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/token.hpp>

#endif