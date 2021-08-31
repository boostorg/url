//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_TYPE_TRAITS_HPP
#define BOOST_URL_BNF_TYPE_TRAITS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/type_traits/make_void.hpp>
#include <type_traits>

namespace boost {
namespace urls {
namespace bnf {

/** Alias for std::true_type if T satisfies Element

    @par Valid expressions
    @code
        char const*
        T::parse(
            char const* start,
            char const* end,
            error_code& ec);
    @endcode

    @par Exemplar
    @code
    struct element
    {
        using value_type = ...;
        value_type const& value() const noexcept;
        char const* parse(
            char const*, char const*, error_code& );
    };
    @endcode
*/
#if BOOST_URL_DOCS
template<class T>
using is_element = __see_below__;

#else
template<class T, class = void>
struct is_element : std::false_type {};

template<class T>
struct is_element<T, boost::void_t<
    typename T::value_type,
    decltype(
    std::declval<char const*&>() = std::declval<T&>().parse(
        std::declval<char const*>(),
        std::declval<char const*>(),
        std::declval<error_code&>())
            )>> : std::integral_constant<bool,
    std::is_default_constructible<T>::value &&
    std::is_copy_constructible<T>::value &&
    std::is_copy_assignable<T>::value>
{
};

#endif

/** Alias for std::true_type if T satisfies Element

    @par Valid expressions
    @code
        char const*
        T::begin(
            char const* start,
            char const* end,
            error_code& ec);

        char const*
        T::increment(
            char const* start,
            char const* end,
            error_code& ec);
    @endcode
*/
#if BOOST_URL_DOCS
template<class T>
using is_list = __see_below__;

#else
template<class T, class = void>
struct is_list : std::false_type {};

template<class T>
struct is_list<T, boost::void_t<decltype(
    std::declval<char const*&>() = std::declval<T&>().begin(
        std::declval<char const*>(),
        std::declval<char const*>(),
        std::declval<error_code&>()),
    std::declval<char const*&>() = std::declval<T&>().increment(
        std::declval<char const*>(),
        std::declval<char const*>(),
        std::declval<error_code&>())
            )>> : std::integral_constant<bool,
    std::is_default_constructible<T>::value &&
    std::is_copy_constructible<T>::value &&
    std::is_copy_assignable<T>::value>
{
};

#endif

/** Alias for true_type if T is an element or list
*/
template<class T>
using is_bnf = std::integral_constant<bool,
    is_element<T>::value || is_list<T>::value>;

} // bnf
} // urls
} // boost

#endif
