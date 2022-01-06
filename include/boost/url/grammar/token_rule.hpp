//
// Copyright (c) 2016-2019 Damian Jarek (damian dot jarek93 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_URL_GRAMMAR_TOKEN_RULE_HPP
#define BOOST_URL_GRAMMAR_TOKEN_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string_view.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Rule for 1 or more characters from a character set

    @par BNF
    @code
    token     = 1*( ch )
    @endcode
*/
template<class CharSet>
struct token_rule
{
    using value_type = string_view;

    value_type&
    operator*() noexcept
    {
        return s_;
    }

    value_type*
    operator->() noexcept
    {
        return &**this;
    }

    friend
    bool
    parse(
        char const*& it,
        char const* end,
        error_code& ec,
        token_rule& t) noexcept
    {
        ec = {};
        auto const start = it;
        static constexpr CharSet cs{};
        if(it == end)
        {
            ec = error::incomplete;
            return false;
        }
        it = find_if_not(it, end, cs);
        if(it == start)
        {
            ec = error::syntax;
            return false;
        }
        t.s_ = string_view(start, it - start);
        return true;
    }

private:
    BOOST_STATIC_ASSERT(
        is_charset<
            CharSet>::value);

    value_type s_;
};

} // bnf
} // urls
} // boost

#endif
