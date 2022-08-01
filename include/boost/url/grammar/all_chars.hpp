//
// Copyright (c) 2021 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_ALL_CHARS_HPP
#define BOOST_URL_GRAMMAR_ALL_CHARS_HPP

#include <boost/url/detail/config.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** The set of all characters

    This function object is invocable with
    the following equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    It returns `true` when `ch` is a member
    of the character set, and `false` otherwise.

    @par BNF
    @code
    ALL         = %x00-FF
                ; all ASCII and high-ASCII
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#appendix-B.1"
        >B.1. Core Rules (rfc5234)</a>

    @see
        @ref find_if,
        @ref find_if_not.
*/
#ifdef BOOST_URL_DOCS
constexpr __implementation_defined__ all_chars;
#else
struct all_chars_t
{
    constexpr
    bool
    operator()(char) const noexcept
    {
        return true;
    }

    constexpr
    char const*
    find_if(
        char const* first,
        char const*) const noexcept
    {
        return first;
    }

    constexpr
    char const*
    find_if_not(
        char const*,
        char const* last) const noexcept
    {
        return last;
    }
};
constexpr all_chars_t all_chars{};
#endif

} // grammar
} // urls
} // boost

#endif
