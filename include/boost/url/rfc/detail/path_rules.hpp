//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_RFC_DETAIL_PATH_RULES_HPP
#define BOOST_URL_RFC_DETAIL_PATH_RULES_HPP

#include <boost/url/rfc/pchars.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/grammar/delim_rule.hpp>
#include <boost/url/grammar/range_rule.hpp>
#include <boost/url/grammar/tuple_rule.hpp>

namespace boost {
namespace urls {
namespace detail {

/** Rule for segment

    @par BNF
    @code
    segment       = *pchar
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref grammar::parse.
*/
constexpr auto segment_rule =
    pct_encoded_rule(grammar::ref(pchars));

//------------------------------------------------

/** Rule for segment-nz

    @par BNF
    @code
    segment-nz    = 1*pchar
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>
*/
constexpr auto segment_nz_rule =
    grammar::not_empty_rule(
        pct_encoded_rule(grammar::ref(pchars)));

//------------------------------------------------

/** Rule for segment_nz_nc

    @par BNF
    @code
    segment-nz-nc   = 1*( unreserved / pct-encoded / sub-delims / "@" )
                    ; non-zero-length segment without any colon ":"
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref grammar::parse.
*/
constexpr auto segment_nz_nc_rule =
    grammar::not_empty_rule(
        pct_encoded_rule(pchars - ':'));

//------------------------------------------------

/** Rule for slash-segment

    @par BNF
    @code
    slash-segment   = "/" segment

    @endcode
*/
constexpr auto slash_segment_rule =
    grammar::tuple_rule(
        grammar::squelch(
            grammar::delim_rule('/')),
        pct_encoded_rule(grammar::ref(pchars)));

//------------------------------------------------

// VFALCO This is an alternate rule not found
// in the rfc3986, to reformulate the rfc path
// BNFs into something the range_rule can use.

/** Rule for segment-ns
*/
#ifdef BOOST_URL_DOCS
constexpr __implementation_defined__ segment_ns_rule;
#else
struct segment_ns_rule_t
{
    using value_type = pct_string_view;

    BOOST_URL_DECL
    result<value_type>
    parse(
        char const*& it,
        char const* end) const noexcept;
};

constexpr segment_ns_rule_t segment_ns_rule{};
#endif

//------------------------------------------------

/** Rule for path-abempty grammar

    @par BNF
    @code
    path-abempty  = *( "/" segment )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>
*/
constexpr auto path_abempty_rule =
    grammar::range_rule(
        grammar::tuple_rule(
            grammar::squelch(
                grammar::delim_rule('/')),
            segment_rule));

//------------------------------------------------

/** Rule for path-absolute grammar.

    @par BNF
    @code
    path-absolute = "/" [ segment-nz *( "/" segment ) ]
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>
*/
#ifdef BOOST_URL_DOCS
constexpr __implementation_defined__ path_absolute_rule;
#else
constexpr auto path_absolute_rule =
    grammar::range_rule(
        grammar::tuple_rule(
            grammar::squelch(
                grammar::delim_rule('/')),
            detail::segment_ns_rule),
        detail::slash_segment_rule,
        1);
#endif

//------------------------------------------------

/** Rule for path-noscheme grammar.

    @par BNF
    @code
    path-noscheme = segment-nz-nc *( "/" segment )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>
*/
#ifdef BOOST_URL_DOCS
constexpr __implementation_defined__ path_noscheme_rule;
#else
constexpr auto path_noscheme_rule =
    grammar::range_rule(
        segment_nz_nc_rule,
        slash_segment_rule,
        1);
#endif

//------------------------------------------------

/** Rule for path-rootless grammar.

    @par Example
    @code
    path_rootless_rule t;
    bool success = parse( it, end, ec, t);
    @endcode

    @par BNF
    @code
    path-rootless = segment-nz *( "/" segment )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>
*/
#ifdef BOOST_URL_DOCS
constexpr __implementation_defined__ path_rootless_rule;
#else
constexpr auto path_rootless_rule =
    grammar::range_rule(
        segment_nz_rule,
        slash_segment_rule,
        1);
#endif

} // detail
} // urls
} // boost

#endif
