//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_ERROR_HPP
#define BOOST_URL_ERROR_HPP

#include <boost/url/detail/config.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <stdexcept>

namespace boost {
namespace urls {

/// The type of error code used by the library
using error_code = boost::system::error_code;

/// The type of system error thrown by the library
using system_error = boost::system::system_error;

/// The type of error category used by the library
using error_category = boost::system::error_category;

/// The type of error condition used by the library
using error_condition = boost::system::error_condition;

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

/** Error codes returned the library
*/
enum class error
{
    success = 0,

    syntax,

    /** Bad alphabetic character
    */
    bad_alpha,

    /** Character is not a digit
    */
    bad_digit,

    /** A required element was empty
    */
    bad_empty_element,

    /** Bad HEXDIG
    */
    bad_hexdig,

    /** Syntax error in IPv6 address
    */
    bad_ipv6,

    /** Bad leading zero in number
    */
    bad_leading_zero,

    /** The octet is out of range
    */
    bad_octet,

    /** Bad schemeless path segment
    */
    bad_schemeless_path_segment,

    /** Bad empty element
    */
    empty,

    /** Illegal empty path segment
    */
    empty_path_segment,

    /** A list parser reached the end
    */
    end,

    /** Illegal reserved character in encoded string.
    */
    illegal_reserved_char,

    /** Incomplete input for grammar

        This happens when the end of the input
        string is reached without fully matching
        the grammar.
    */
    incomplete,

    /** Validation failed
    */
    invalid,

    /** Leftover input remaining after match
    */
    leftover_input,

    /** Missing character literal
    */
    missing_char_literal,

    /** Missing path segment
    */
    missing_path_segment,

    /** A slash '/' was expected in the path segment
    */
    missing_path_separator,

    /** Missing words in IPv6 address
    */
    missing_words,

    /** Wrong character literal
    */
    wrong_char_literal,

    //--------------------------------------------

    /** Bad hexadecimal digit

        This error condition is fatal.
    */
    bad_pct_hexdig,

    /** The percent-encoded sequence is incomplete.

        This error condition is fatal.
    */
    incomplete_pct_encoding,

    /** Missing hexadecimal digit

        This error condition is fatal.
    */
    missing_pct_hexdig,

};

enum class condition
{
    /** A fatal error in syntax was encountered

        This indicates that parsing cannot continue.
    */
    fatal = 1
};

} // urls
} // boost

#include <boost/url/impl/error.hpp>

#endif
