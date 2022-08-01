//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_TYPE_TRAITS_HPP
#define BOOST_URL_GRAMMAR_TYPE_TRAITS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/result.hpp>
#include <boost/type_traits/make_void.hpp>
#include <type_traits>

namespace boost {
namespace urls {
namespace grammar {

/** Alias for `std::true_type` if `T` satisfies __MutableString__
    for the iterator type `I`
*/
#ifdef BOOST_URL_DOCS
template<class T, class I>
using is_mutable_string = __see_below__;
#else
template<class T, class I, class = void>
struct is_mutable_string : std::false_type {};

template<class T, class I>
struct is_mutable_string<T, I, boost::void_t<
    // T::append(I, I)
    decltype(
        std::declval<T&>()
            .append(
                std::declval<I>(),
                std::declval<I>())
                ),
    // T::assign(I, I)
    decltype(
        std::declval<T&>()
            .assign(
                std::declval<I>(),
                std::declval<I>())
                ),
    // std::is_same_v<T::value_type, char>
    decltype(
        std::declval<std::true_type&>() =
            typename std::integral_constant<
                bool, std::is_same<
                    typename T::value_type, char >::value>{}
                )>>
    : std::true_type
{};
#endif

//------------------------------------------------

/** Determine if T meets the requirements of Rule

    This is an alias for `std::true_type` if
    `T` meets the requirements, otherwise it
    is an alias for `std::false_type`.

    @par Example
    @code
    struct U
    {
        struct value_type;

        auto
        parse(
            char const*& it,
            char const* end) const ->
                result<value_type>
    };

    static_assert( is_rule<U>::value, "Requirements not met" );
    @endcode

    @see
        @ref parse.
*/
#ifdef BOOST_URL_DOCS
template<class T>
using is_rule = __see_below__;
#else
template<class T, class = void>
struct is_rule : std::false_type {};

template<class T>
struct is_rule<T, boost::void_t<decltype(
    std::declval<result<typename T::value_type>&>() =
        std::declval<T const&>().parse(
            std::declval<char const*&>(),
            std::declval<char const*>())
    )>> : std::true_type
{
};
#endif

} // grammar
} // urls
} // boost

#endif
