//
// Copyright (c) 2016-2019 Damian Jarek (damian dot jarek93 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_URL_GRAMMAR_DETAIL_TUPLE_HPP
#define BOOST_URL_GRAMMAR_DETAIL_TUPLE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/mp11/integer_sequence.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/copy_cv.hpp>
#include <cstdlib>
#include <utility>

namespace boost {
namespace urls {
namespace grammar {
namespace detail {

template<std::size_t I, class T>
struct tuple_element_impl
{
    T t;

    constexpr
    tuple_element_impl(T const& t_)
        : t(t_)
    {
    }

    constexpr
    tuple_element_impl(T&& t_)
        : t(std::move(t_))
    {
    }
};

template<std::size_t I, class T>
struct tuple_element_impl<I, T&>
{
    T& t;

    constexpr
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
    template<class... Us>
    constexpr
    explicit
    tuple_impl(Us&&... us)
        : tuple_element_impl<Is, Ts>(
            std::forward<Us>(us))...
    {
    }
};

template<class... Ts>
struct tuple : tuple_impl<
    boost::mp11::index_sequence_for<
        Ts...>, Ts...>
{
    constexpr tuple() = default;

    constexpr
    tuple(tuple const&) noexcept = default;

    template<class U
    #if 0
        ,typename std::enable_if<
            ! std::is_same<
        U, error_code>::value>::type = 0
    #endif
    >
    constexpr
    explicit
    tuple(U&& u) noexcept
        : tuple_impl<
            boost::mp11::index_sequence_for<
            Ts...>, Ts...>{u}
    {
    }

    template<
        class U0,
        class U1,
        class... Us>
    constexpr
    explicit
    tuple(
        U0&& u0,
        U1&& u1,
        Us&&... us) noexcept
        : tuple_impl<
            boost::mp11::index_sequence_for<
                Ts...>, Ts...>{
            std::forward<U0>(u0),
            std::forward<U1>(u1),
            std::forward<Us>(us)...}
    {
    }
};

template<std::size_t I, class T>
constexpr
T&
get(tuple_element_impl<I, T>& te)
{
    return te.t;
}

template<std::size_t I, class T>
constexpr
T const&
get(tuple_element_impl<I, T> const& te)
{
    return te.t;
}

template<std::size_t I, class T>
constexpr
T&&
get(tuple_element_impl<I, T>&& te)
{
    return std::move(te.t);
}

template<std::size_t I, class T>
constexpr
T&
get(tuple_element_impl<I, T&>&& te)
{
    return te.t;
}

template <std::size_t I, class T>
using tuple_element = typename boost::copy_cv<
    mp11::mp_at_c<typename remove_cv<T>::type, I>, T>::type;

} // detail
} // grammar
} // urls
} // boost

#if 0
namespace std {
template<class T>
struct tuple_size;
template<class... Ts>
struct tuple_size<
    ::boost::urls::grammar::detail::tuple<Ts...>>
    : std::integral_constant<
        std::size_t, sizeof...(Ts)>
{
};
} // std
#endif

#endif
