//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_ERROR_HPP
#define BOOST_URL_GRAMMAR_ERROR_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/result.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Error codes returned by rules
*/
enum class error
{
    // VFALCO 3 space indent or
    // else Doxygen malfunctions

    //
    // (informational)
    //

    /**
     * The operation completed successfully.
    */
    success = 0,

    /**
     * A rule reached the end of a range
     *
     * This indicates that the input was consumed
     * when parsing a @ref range. The @ref range_rule
     * will avoid rewinding the input buffer when
     * this error is returned. Thus the consumed
     * characters will be considered part of the
     * range without contributing additional
     * elements.
    */
    end,

    //
    // (recoverable)
    //

    /**
     * Incomplete input for grammar.

       This happens when the end of the input
       string is reached without fully matching
       the grammar. Parsing elements which
       support streaming will return this error
       to indicate additional input is necessary
       for completely matching the grammar.
    */
    incomplete,

    /**
     * Leftover input remaining after match.
    */
    leftover,

    /**
     * An empty string matched the rule.
    */
    empty,

    //
    // condition::fatal
    //

    /** An integer overflowed during parsing.
    */
    overflow,

    /**
     * An unspecified syntax error was found.
    */
    syntax
};

//------------------------------------------------

/** Error conditions returned by the library.
*/
enum class condition
{
    /**
     * A fatal error in syntax was encountered.

       This indicates that parsing cannot continue.
    */
    fatal = 1
};

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/error.hpp>

#endif
