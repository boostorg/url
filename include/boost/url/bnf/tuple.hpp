//
// Copyright (c) 2016-2019 Damian Jarek (damian dot jarek93 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_URL_BNF_TUPLE_HPP
#define BOOST_URL_BNF_TUPLE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/mp11/integer_sequence.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/copy_cv.hpp>
#include <cstdlib>
#include <utility>

namespace boost {
namespace urls {
namespace bnf {

template<std::size_t I, class T>
struct tuple_element_impl
{
    T t;

    tuple_element_impl() = default;

    tuple_element_impl(T const& t_)
        : t(t_)
    {
    }

    tuple_element_impl(T&& t_)
        : t(std::move(t_))
    {
    }
};

template<std::size_t I, class T>
struct tuple_element_impl<I, T&>
{
    T& t;

    tuple_element_impl() = default;

    tuple_element_impl(T& t_)
        : t(t_)
    {
    }
};

template<class... Ts>
struct tuple_impl;

template<class... Ts, std::size_t... Is>
struct tuple_impl<
    boost::mp11::index_sequence<Is...>, Ts...>
  : tuple_element_impl<Is, Ts>...
{
    tuple_impl() = default;

    template<class... Us>
    explicit tuple_impl(Us&&... us)
        : tuple_element_impl<Is, Ts>(
            std::forward<Us>(us))...
    {
    }
};

template<class... Ts>
struct tuple : tuple_impl<
    boost::mp11::index_sequence_for<Ts...>, Ts...>
{
    tuple() = default;

    template<class... Us>
    explicit tuple(Us&&... us)
      : tuple_impl<
            boost::mp11::index_sequence_for<Ts...>, Ts...>{
          std::forward<Us>(us)...}
    {
    }
};

template<std::size_t I, class T>
T&
get(tuple_element_impl<I, T>& te)
{
    return te.t;
}

template<std::size_t I, class T>
T const&
get(tuple_element_impl<I, T> const& te)
{
    return te.t;
}

template<std::size_t I, class T>
T&&
get(tuple_element_impl<I, T>&& te)
{
    return std::move(te.t);
}

template<std::size_t I, class T>
T&
get(tuple_element_impl<I, T&>&& te)
{
    return te.t;
}

template <std::size_t I, class T>
using tuple_element = typename boost::copy_cv<
    mp11::mp_at_c<typename remove_cv<T>::type, I>, T>::type;

namespace detail {

template<
    class... Args>
char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    string_view* ps,
    std::integral_constant<
        std::size_t, sizeof...(Args)-1>,
    tuple<Args...>& t)
{
    auto constexpr I =
        sizeof...(Args)-1;
    auto it = get<I>(t).parse(
        start, end, ec);
    if(ec)
        return start;
    *ps = string_view(
        start, it - start);
    return it;
}

template<
    std::size_t I,
    class... Args>
char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    string_view* ps,
    std::integral_constant<
        std::size_t, I>,
    tuple<Args...>& t)
{
    auto it = get<I>(t).parse(
        start, end, ec);
    if(ec)
        return start;
    *ps = string_view(
        start, it - start);
    return parse(
        it, end, ec, ps + 1,
        std::integral_constant<
            std::size_t, I+1>{},
        t);
}

template<
    class... Tn>
char const*
parse_tuple(
    char const* const start,
    char const* const end,
    error_code& ec,
    std::integral_constant<
        std::size_t, sizeof...(Tn)-1>,
    tuple<Tn...>& t)
{
    auto constexpr I =
        sizeof...(Tn)-1;
    auto it = parse(
        start, end, ec,
            get<I>(t));
    if(ec)
        return start;
    return it;
}

template<
    std::size_t I,
    class... Args>
char const*
parse_tuple(
    char const* const start,
    char const* const end,
    error_code& ec,
    std::integral_constant<
        std::size_t, I>,
    tuple<Args...>& t)
{
    auto it = parse(
        start, end, ec,
            get<I>(t));
    if(ec)
        return start;
    it = parse(
        it, end, ec,
        std::integral_constant<
            std::size_t, I+1>{},
        t);
    if(ec)
        return start;
    return it;
}

} // detail

template<class... Tn>
char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    tuple<Tn...>& t)
{
    auto it =
        detail::parse_tuple(
        start, end, ec,
        std::integral_constant<
            std::size_t, 0>{}, t);
    if(ec)
        return start;
    return it;
}

} // bnf
} // urls
} // boost

#endif