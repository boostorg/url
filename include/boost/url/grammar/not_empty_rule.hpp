//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_NOT_EMPTY_RULE_HPP
#define BOOST_URL_GRAMMAR_NOT_EMPTY_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/result.hpp>
#include <boost/url/grammar/type_traits.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Rule for an element which may not be empty

    This rule adapts another rule such that
    when an empty string is successfully
    parsed, the result will be an error.
*/
template<class R>
struct not_empty_rule_t
{
    using value_type =
        typename R::value_type;

    auto
    parse(
        char const*& it,
        char const* end) const ->
            result<value_type>;

    template<class R_>
    friend
    constexpr
    auto
    not_empty_rule(
        R_ const& r) ->
            not_empty_rule_t<R_>;

private:
    constexpr
    not_empty_rule_t(
        R const& r) noexcept
        : r_(r)
    {
    }

    R const r_;
};

//------------------------------------------------

/** Rule for an element which may not be empty

    This rule adapts another rule such that
    when an empty string is successfully
    parsed, the result will be an error.
*/
template<class R>
auto
constexpr
not_empty_rule(
    R const& r) ->
        not_empty_rule_t<R>
{
    return { r };
}

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/not_empty_rule.hpp>

#endif
