//
// Copyright (c) 2016-2019 Damian Jarek (damian dot jarek93 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_URL_GRAMMAR_TOKEN_RULE_HPP
#define BOOST_URL_GRAMMAR_TOKEN_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/result.hpp>
#include <boost/url/string_view.hpp>

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
struct token_rule_t
{
    using value_type = string_view;
    //using value_type = std::string;

    static_assert(
        is_charset<CharSet>::value,
        "CharSet requirements not met");

    auto
    parse(
        char const*& it,
        char const* end
            ) const noexcept ->
        result<value_type>;

private:
    template<class CharSet_>
    friend
    auto
    token_rule(
        CharSet_ const&) noexcept ->
            token_rule_t<CharSet_>;

    token_rule_t(
        CharSet const& cs) noexcept
        : cs_(cs)
    {
    }

    CharSet const cs_;
};

template<
    class CharSet>
auto
token_rule(
    CharSet const& cs) noexcept ->
        token_rule_t<CharSet>
{
    return {cs};
}

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/token_rule.hpp>

#endif
