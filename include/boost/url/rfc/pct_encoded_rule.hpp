//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PCT_ENCODED_RULE_HPP
#define BOOST_URL_RFC_PCT_ENCODED_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/pct_encoding.hpp>
#include <boost/url/result.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/detail/empty_value.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

/** Rule for a string with percent-encoded escapes

    This function returns a rule which, when used
    with the @ref grammar::parse function, parses
    its input as a percent-encoded string.
    The percent-encoding mechanism is used to
    represent data characters which would otherwise
    be considered context-dependent delimiters in
    a syntax. For example the string

    @code
    "Program Files"
    @endcode
    
    contains a space, which is often undestood to
    be a delimiter. It can be expressed as a
    percent-encoded escape sequence in hexadecimal
    as follows:
    
    @code
    "Program%20Files"
    @endcode

    Given a production rule which includes
    <em>pct-encoded</em> escape sequences, such as:
    @code
    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    @endcode
    
    This example shows how to parse a string
    of pchars:

    @par Example
    @code
    result< pct_encoded_view > rv = grammar::parse( s, pct_encoded_rule( pchars ) );
    @endcode

    @par BNF
    @code
    pct-encoded   = "%" HEXDIG HEXDIG
    @endcode

    @param cs The character set indicating
    which characters are allowed without escapes.
    Any character which is not in this set must be
    escaped, or else parsing returns an error.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref grammar::parse,
        @ref pct_encoded_view
*/
#ifdef BOOST_URL_DOCS
/**@{*/
template<class CharSet>
constexpr
__implementation_defined__
pct_encoded_rule( CharSet const& cs ) noexcept;
/**@}*/
#else
template<class CharSet>
struct pct_encoded_rule_t
    : private detail::empty_value<CharSet>
{
    using value_type = pct_encoded_view;

    template<class CharSet_>
    friend
    constexpr
    auto
    pct_encoded_rule(
        CharSet_ const& cs) noexcept ->
            pct_encoded_rule_t<CharSet_>;

    result<value_type>
    parse(
        char const*& it,
        char const* end) const noexcept;

private:
    constexpr
    pct_encoded_rule_t(
        CharSet const& cs) noexcept
        : detail::empty_value<CharSet>(
            detail::empty_init, cs)
    {
    }
};

template<class CharSet>
constexpr
auto
pct_encoded_rule(
    CharSet const& cs) noexcept ->
        pct_encoded_rule_t<CharSet>
{
    // If an error occurs here it means that
    // the value of your type does not meet
    // the requirements. Please check the
    // documentation!
    static_assert(
        grammar::is_charset<CharSet>::value,
        "CharSet requirements not met");

    return pct_encoded_rule_t<CharSet>(cs);
}

#endif

} // urls
} // boost

#include <boost/url/rfc/impl/pct_encoded_rule.hpp>

#endif
