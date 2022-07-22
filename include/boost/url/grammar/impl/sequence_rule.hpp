//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_IMPL_SEQUENCE_RULE_HPP
#define BOOST_URL_GRAMMAR_IMPL_SEQUENCE_RULE_HPP

#include <boost/url/grammar/parse.hpp>
#include <boost/mp11/tuple.hpp>

namespace boost {
namespace urls {
namespace grammar {

namespace detail {

// must come first
template<
    class R0,
    class... Rn,
    std::size_t I>
void
parse_element(
    char const*&,
    char const*,
    error_code&,
    detail::tuple<
        R0, Rn...> const&,
    std::tuple<
        result<typename
            R0::value_type>,
        result<typename
            Rn::value_type>...>&,
    std::integral_constant<
        std::size_t, I> const&,
    std::false_type const&)
{
    // end
}

template<
    class R0,
    class... Rn,
    std::size_t I>
void
parse_element(
    char const*& it,
    char const* end,
    error_code& ec,
    detail::tuple<
        R0, Rn...> const& rn,
    std::tuple<
        result<typename
            R0::value_type>,
        result<typename
            Rn::value_type>...>& tn,
    std::integral_constant<
        std::size_t, I> const&,
    std::true_type const&)
{
    auto& rv = get<I>(tn);
    rv = grammar::parse(
        it, end, get<I>(rn));
    if(rv.has_error())
    {
        ec = rv.error();
        return;
    }
    parse_element(
        it, end, ec, rn, tn,
        std::integral_constant<
            std::size_t, I + 1>{},
        std::integral_constant<bool,
            ((I + 1) < (
                1 + sizeof...(Rn)))>{});
}

struct deref
{
    template<class R>
    auto
    operator()(R const& r) const ->
        decltype(*r)
    {
        return *r;
    }
};

} // detail

template<
    class R0,
    class... Rn>
auto
sequence_rule_t<R0, Rn...>::
parse(
    char const*& it,
    char const* end) const ->
        result<value_type>
{
    error_code ec;
    std::tuple<
        result<typename
            R0::value_type>,
        result<typename
            Rn::value_type>...> tn(
        result<typename
               R0::value_type>(error_code{}),
        result<typename
               Rn::value_type>(error_code{})...
    );
    detail::parse_element(
        it, end, ec, rn_, tn,
        std::integral_constant<
            std::size_t, 0>{},
        std::true_type{});
    if(ec.failed())
        return ec;
    return mp11::tuple_transform(
        detail::deref{}, tn);
}

} // grammar
} // urls
} // boost

#endif
