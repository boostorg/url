//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_VARIANT_RULE_HPP
#define BOOST_URL_GRAMMAR_VARIANT_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/result.hpp>
#include <boost/url/variant.hpp>
#include <boost/url/grammar/detail/tuple.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Rule for a set of alternative grammar elements
*/
template<
    class R0, class... Rn>
class variant_rule_t
{
public:
    using value_type = variant<
        typename R0::value_type,
        typename Rn::value_type...>;

    auto
    parse(
        char const*& it,
        char const* end) const ->
            result<value_type>;

    template<
        class R0_,
        class... Rn_>
    friend
    constexpr
    auto
    variant_rule(
        R0_ const& r0,
        Rn_ const&... rn) noexcept ->
            variant_rule_t<R0_, Rn_...>;

private:
    constexpr
    variant_rule_t(
        R0 const& r0,
        Rn const&... rn) noexcept
        : rn_(r0, rn...)
    {
    }

    detail::tuple<R0, Rn...> rn_;
};

/** Return a rule for parsing a set of alternatives
*/
template<
    class R0,
    class... Rn>
constexpr
auto
variant_rule(
    R0 const& r0,
    Rn const&... rn) noexcept ->
        variant_rule_t<R0, Rn...>;

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/variant_rule.hpp>

#endif
