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
#include <boost/url/optional.hpp>
#include <boost/url/string.hpp>

namespace boost {
namespace urls {
namespace bnf {

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
parse(
    string_view s,
    error_code& ec,
    T0&& t0,
    Tn&&... tn);

/** Return true if a string matches the grammar
*/
template<class T>
bool
is_valid(
    string_view s);

/** Throw an exception the string does not match the grammar
*/
template<class T>
void
validate(
    string_view s);

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/parse.hpp>

#endif
