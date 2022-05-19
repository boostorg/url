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
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/parse_tag.hpp>
#include <boost/type_traits/make_void.hpp>
#include <type_traits>

namespace boost {
namespace urls {
namespace grammar {

/** Parse a literal character

    This function parses the first char in a
    substring as a literal character.

    @par Example

    @code
    std::string str = "Hello.cpp";
    char const* it = str.data();
    char const* end = it + str.size();
    if (parse(it, end, ec, 'H'))
    {
        std::cout << "str begins with 'H'\n";
    }
    @endcode

    @par Exception Safety
    Throws nothing.

    @param it An iterator to the first element in the range.
    At the end of this function, `it` points to one past the last
    element parsed in the range.

    @param end An iterator to the end of the range

    @param ec Set to the error, if any occurred

    @param ch A literal character to match
*/
void
tag_invoke(
    parse_tag const&,
    char const*& it,
    char const* end,
    error_code& ec,
    char ch) noexcept;

/** Parse a range of characters into two or more sequential grammar rules

    This function parses a range of characters into
    one or more grammar rules, where each rule
    defines its own syntax and storage for parsed
    results.

    The algorithm works by invoking a function called
    `parse` for each element using an unqualified call
    so that argument-dependent lookup applies, using
    the equivalent signature:

    @code
    template< class Rule >
    bool parse( char const*&, char const*, error_code&, Rule&& r );
    @endcode

    If this call succeeds, the algorithm proceeds by
    performing the same operation on the remaining
    arguments including the parameter pack.

    If an error occurs for any of the rules in the
    parameter pack, the entire parse operation fails
    but the chars from the substring are still consumed.

    @par Example
    @code
    if (parse(it, end, ec, r1, r2, r3))
    {
        std::cout << "Range [it, end) parsed successfully\n";
    }
    @endcode

    @par Exception Safety
    Defined by the types of the rule objects.

    @return `true` if the range initial chars match the elements successfully

    @param it An iterator to the first element in the range.
    At the end of this function, `it` points to one past the last
    element parsed in the range.

    @param end An iterator to the end of the range

    @param ec Set to the error, if any occurred

    @param rn Grammar rule objects

*/
template<class... Rn>
bool
parse(
    char const*& it,
    char const* end,
    error_code& ec,
    Rn&&... rn);

/** Parse all rules in sequence

    If the parse fails, `it` is unchanged.
*/
template<class... Rn>
bool
parse_all(
    char const*& it,
    char const* end,
    error_code& ec,
    Rn&&... rn);

/** Parse a sequence of grammar rules

   This function parses a complete string into the sequence of
   specified grammar rules. The function returns an error if
   some characters are left unparsed.

   @par Example

   @code
   if (parse(str, ec, r1, r2, r3)) {
     std::cout << "String " << str << " parsed successfully\n";
   }
   @endcode

   @par Exception Safety

     Defined by the types of the rule objects.

   @return `true` if the string matches the rules successfully and all
   chars are consumed

   @param s The input string

   @param ec Set to the error, if any occurred. If the string is not completely consumed,
   but the beginning of the string matches the elements, `ec` is set to @ref error::leftover.

   @param rn Grammar rule objects

*/
template<class... Rn>
bool
parse_string(
    string_view s,
    error_code& ec,
    Rn&&... rn);

/** Parse a sequence of grammar rules and throw on failure

   This function parses a complete string into the specified sequence
   of grammar rules. If the string is not completely consumed, an
   error is thrown.

   @par Example

   @code
   try {
      parse(str, ec, r1, r2, r3);
   } catch (boost::urls::system_error& e) {
      std::cout << e.what() << '\n';
   }
   @endcode

   @par Exception Safety

     Exceptions thrown on invalid input.

   @param s The input string

   @param rn Grammar rule objects

   @throws boost::system::system_error Thrown on failure.

 */
template<class... Rn>
void
parse_string(
    string_view s,
    Rn&&... rn);

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/parse.hpp>

#endif
