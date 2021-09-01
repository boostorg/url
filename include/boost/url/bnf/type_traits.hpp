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

    To satisfy _Element_, T must satisfy
    _DefaultConstructible_, _CopyConstructible_,
    _CopyAssignable_, and also meet these
    requirements:

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
    struct T
    {
        char const* parse( char const*, char const*, error_code& );
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
    //typename T::value_type,
    decltype(
    //std::declval<T const&>.operator->(),
    //std::declval<T const&>.operator*(),
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

/** Alias for std::true_type if T satisfies List

    To satisfy _List_, T must satisfy
    _DefaultConstructible_, _CopyConstructible_,
    _CopyAssignable_, and also meet these
    requirements:

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

    @par Exemplar
    @code
    struct T
    {
        char const* begin( char const*, char const*, error_code& );
        char const* increment( char const*, char const*, error_code& );
    };
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

/** Alias for std::true_type if T is a bnf with a value

    @par Valid expressions
    @code
        auto
        T::operator->();

        auto
        T::operator*();
    @endcode
*/
#if BOOST_URL_DOCS
template<class T>
using has_value = __see_below__;

#else
template<class T, class = void>
struct has_value : std::false_type {};

template<class T>
struct has_value<T, boost::void_t<decltype(
    std::declval<T&>().operator->(),
    std::declval<T&>().operator*()
        )>> : std::integral_constant<bool,
    is_bnf<T>::value>
{
};

#endif

} // bnf
} // urls
} // boost

#endif
