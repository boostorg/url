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
#include <boost/url/error_code.hpp>
#include <boost/url/result.hpp>
#include <stdexcept>

namespace boost {
namespace urls {

/** Error codes returned the library
*/
enum class error
{
    // VFALCO 3 space indent or
    // else Doxygen malfunctions

    /**
     * The operation completed successfully.
    */
    success = 0,

    /**
     * Bad alphabetic character.
    */
    bad_alpha,

    /**
     * Character is not a digit.
    */
    bad_digit,

    /**
     * A required element was empty.
    */
    bad_empty_element,

    /**
     * Bad HEXDIG
    */
    bad_hexdig,

    /**
     * Syntax error in IPv6 address.
    */
    bad_ipv6,

    /**
     * Bad leading zero in number.
    */
    bad_leading_zero,

    /**
     * The octet is out of range.
    */
    bad_octet,

    /** Bad empty element.
    */
    empty,

    /**
     * Illegal empty path segment.
    */
    empty_path_segment,

    /**
     * Null encountered in pct-encoded.
    */
    illegal_null,

    /**
     * Illegal reserved character in encoded string.
    */
    illegal_reserved_char,

    /**
     * Validation failed
    */
    invalid,

    /**
     * Missing character literal.
    */
    missing_char_literal,

    /**
     * Missing path segment.
    */
    missing_path_segment,

    /**
     * A slash '/' was expected in the path segment.
    */
    missing_path_separator,

    /**
     * Missing words in IPv6 address.
    */
    missing_words,

    /**
     * A grammar element was not in canonical form.
    */
    non_canonical,

    /**
     * Wrong character literal.
    */
    wrong_char_literal,

    //--------------------------------------------

    /**
     * Bad hexadecimal digit.

       This error condition is fatal.
    */
    bad_pct_hexdig,

    /**
     * The percent-encoded sequence is incomplete.

       This error condition is fatal.
    */
    incomplete_pct_encoding,

    /**
     * Missing hexadecimal digit.

       This error condition is fatal.
    */
    missing_pct_hexdig,

    /**
     * No space in output buffer

       This error is returned when a provided
       output buffer was too small to hold
       the complete result of an algorithm.
    */
    no_space,

    /**
     * The URL is not a base URL
    */
    not_a_base
};

} // urls
} // boost

#include <boost/url/impl/error.hpp>

#endif
