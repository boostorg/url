//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_DECODE_HPP
#define BOOST_URL_DECODE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/decode_opts.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/error_types.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/pct_string_view.hpp>
#include <boost/url/detail/decode.hpp>
#include <boost/url/grammar/all_chars.hpp>
#include <boost/url/grammar/charset.hpp>

namespace boost {
namespace urls {

/** Validate and decode a string view

    This function returns a view that applies
    percent-decoding to the given percent-encoded
    string, by converting escape sequences into
    their character equivalent.

    The function validates the input string
    and returns a view of the decoded bytes.
    If the input string is invalid, the result
    contains an error.

    @par Example
    @code
    result< decode_view > r = decode( "Program%20Files" );
    assert( r.has_value() );

    decode_view v = *r;
    assert( v == "Program Files" );
    assert( v.size() == 13 );
    assert( v.encoded().size() == 15 );
    @endcode

    @par Exception Safety
    Throws nothing.

    @return The decoded string.

    @param s The string to decode.

    @param token A string token

    @param opt The options for decoding. If
    this parameter is omitted, the default
    options will be used.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1"
        >2.1. Percent-Encoding (rfc3986)</a>

    @see
        @ref decode_opts.
*/
template<BOOST_URL_STRTOK_TPARAM>
result<BOOST_URL_STRTOK_RETURN>
decode(
    BOOST_URL_PCT_STRING_VIEW s,
    decode_opts const& opt = {},
    BOOST_URL_STRTOK_ARG(token)) noexcept;

} // urls
} // boost

#include <boost/url/impl/decode.hpp>

#endif
