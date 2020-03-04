//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_ERROR_HPP
#define BOOST_URL_ERROR_HPP

#include <boost/url/config.hpp>
#include <stdexcept>

namespace boost {
namespace urls {

struct BOOST_SYMBOL_VISIBLE
    parse_error : std::invalid_argument
{
    BOOST_URL_DECL
    parse_error();

#ifndef BOOST_URL_DOCS
    BOOST_URL_DECL
    static BOOST_NORETURN void raise();
#endif

};

struct BOOST_SYMBOL_VISIBLE
    invalid_part : std::invalid_argument
{
    BOOST_URL_DECL
    invalid_part();

#ifndef BOOST_URL_DOCS
    BOOST_URL_DECL
    static BOOST_NORETURN void raise();
#endif

};

struct BOOST_SYMBOL_VISIBLE
    too_large : std::length_error
{
    BOOST_URL_DECL
    too_large();

#ifndef BOOST_URL_DOCS
    BOOST_URL_DECL
    static BOOST_NORETURN void raise();
#endif
};

struct BOOST_SYMBOL_VISIBLE
    out_of_range : std::out_of_range
{
    BOOST_URL_DECL
    out_of_range();

#ifndef BOOST_URL_DOCS
    BOOST_URL_DECL
    static BOOST_NORETURN void raise();
#endif
};

//----------------------------------------------------------

/** Error codes returned by URL operations
*/
enum class error
{
    /// An input did not match a structural element (soft error)
    no_match = 1,

    /// A syntax error occurred
    syntax,

    /// The parts encountered an invalid input
    invalid,

    //---

    /// The scheme is missing.
    missing_scheme,

    /// The scheme does not start with a letter.
    bad_scheme_start_char,

    /// The scheme contains an invalid character.
    bad_scheme_char,

    /** The user contains an invalid character.

        This error is returned when attempting to set
        the user with a string containing a colon (':').
    */
    bad_username_char,

    /// The userinfo contains an invalid character.
    bad_userinfo_char,

    /// The port contains an invalid character.
    bad_port_char,

    /// The port number is too large.
    port_overflow,

    /// The hostname is missing
    missing_hostname,

    /// The port is missing
    missing_port,

    /// The percent-encoding digit is invalid.
    bad_pct_encoding_digit,

    /// The percent-encoded sequence is incomplete.
    incomplete_pct_encoding,

    /// Illegal reserved character in encoded string.
    illegal_reserved_char
};

enum class condition
{
    parse_error = 1
};

} // urls
} // boost

#include <boost/url/impl/error.hpp>
#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/error.ipp>
#endif

#endif
