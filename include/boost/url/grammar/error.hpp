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

namespace boost {
namespace urls {
namespace grammar {

/** Error codes returned by BNF algorithms
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
     * A list parser reached the end.
    */
    end,

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

} // bnf
} // urls
} // boost

#include <boost/url/grammar/impl/error.hpp>

#endif
