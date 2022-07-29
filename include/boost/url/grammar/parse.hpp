//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_PARSE_HPP
#define BOOST_URL_GRAMMAR_PARSE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/result.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/type_traits.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Parse a string using a rule

    @param it A pointer to the start. The
    caller's variable will be changed to
    reflect the amount of input consumed.

    @param end A pointer to the end.

    @param r The rule to use
*/
template<class R>
auto
parse(
    char const*& it,
    char const* end,
    R const& r) ->
        result<typename R::value_type>;

/** Parse a string using a rule

    This function parses a complete string into
    the specified sequence of grammar rules. If
    the string is not completely consumed, 
    an error is returned instead.

    @param s The input string

    @param r The rule to use
*/
template<class R>
auto
parse(
    string_view s,
    R const& r) ->
        result<typename R::value_type>;

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/parse.hpp>

#endif
