//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_SEQUENCE_RULE_HPP
#define BOOST_URL_GRAMMAR_SEQUENCE_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/detail/tuple.hpp>
#include <tuple>

namespace boost {
namespace urls {
namespace grammar {

/** A rule for parsing elements in a sequence
*/
template<
    class R0,
    class... Rn>
class sequence_rule_t
{
public:
    using value_type = std::tuple<
        typename R0::value_type,
        typename Rn::value_type...>;

    template<
        class R0_,
        class... Rn_>
    friend
    constexpr
    auto
    sequence_rule(
        R0_ const& r0,
        Rn_ const&... rn) noexcept ->
            sequence_rule_t<R0_, Rn_...>;

    result<value_type>
    parse(
        char const*& it,
        char const* end) const;

private:
    constexpr
    sequence_rule_t(
        R0 const& r0,
        Rn const&... rn) noexcept
        : rn_(r0, rn...)
    {
    }

    detail::tuple<R0, Rn...> const rn_;
};

//------------------------------------------------

/** Return a rule to parse a sequence
*/
template<
    class R0,
    class... Rn>
constexpr
auto
sequence_rule(
    R0 const& r0,
    Rn const&... rn) noexcept ->
        sequence_rule_t<
            R0, Rn...>
{
    return { r0, rn... };
}

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/sequence_rule.hpp>

#endif
