//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
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
#include <boost/url/grammar/detail/type_traits.hpp>
#include <boost/type_traits/make_void.hpp>
#include <type_traits>

namespace boost {
namespace urls {
namespace grammar {

/** Alias for `std::true_type` if `T` satisfies __MutableString__
*/
#ifdef BOOST_URL_DOCS
template<class T, class I>
using is_mutable_string = __see_below__;
#else
template<class T, class = void>
struct is_mutable_string : std::false_type {};

template<class T>
struct is_mutable_string<T, boost::void_t<
    // T::append(I, I)
    decltype(
        std::declval<T&>()
            .append(
                std::declval<detail::input_it_ex>(),
                std::declval<detail::input_it_ex>())
                ),
    // T::assign(I, I)
    decltype(
        std::declval<T&>()
            .assign(
                std::declval<detail::input_it_ex>(),
                std::declval<detail::input_it_ex>())
                ),
    // std::is_same_v<T::value_type, char>
    decltype(
        std::declval<std::true_type&>() =
            typename std::integral_constant<
                bool, std::is_same<
                    typename T::value_type, char >::value>{}
                )>>
    : std::true_type
{
};
#endif

} // grammar
} // urls
} // boost

#endif
