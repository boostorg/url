//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_PARSE_HPP
#define BOOST_URL_BNF_PARSE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/type_traits/make_void.hpp>
#include <type_traits>

#include <boost/optional.hpp>

namespace boost {
namespace urls {
namespace bnf {

/** Alias for `std::true_type` if T satisfies __Element__
*/
#ifdef BOOST_URL_DOCS
template<class T>
using is_element = __see_below__;
#else
template<class T, class = void>
struct is_element : std::false_type {};

template<class T>
struct is_element<T, boost::void_t<
    decltype(
    std::declval<char const*&>() =
        T::parse(
            std::declval<char const*&>(),
            std::declval<char const*>(),
            std::declval<T&>())
        ) > >
    : std::is_default_constructible<T>
{
};
#endif

/** Parse a literal character
*/
inline
bool
parse(
    char const*& it,
    char const* end,
    error_code& ec,
    char ch) noexcept;

/** Parse an optional element

    @par BNF
    @code
    optional    = [ <T> ]
    @endcode
*/
template<class T>
bool
parse(
    char const*& it,
    char const* end,
    error_code& ec,
    optional<T>& t);

/** Parse a sequence of elements
*/
template<
    class T0,
    class T1,
    class... Tn>
bool
parse(
    char const*& it,
    char const* end,
    error_code& ec,
    T0&& t0,
    T1&& t1,
    Tn&&... tn);

/** Parse a complete string
*/
template<
    class T0,
    class... Tn>
bool
parse_string(
    string_view s,
    error_code& ec,
    T0&& t0,
    Tn&&... tn);

/** Parse a complete string

    @throw system_error Thrown on failure
*/
template<
    class T0,
    class... Tn>
void
parse_string(
    string_view s,
    T0&& t0,
    Tn&&... tn);

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/parse.hpp>

#endif
