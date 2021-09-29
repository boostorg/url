//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_ENCODED_VIEW_HPP
#define BOOST_URL_PARAMS_ENCODED_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <boost/url/value_types.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view;
#endif

class params_encoded_view
    : private detail::parts_base
{
    friend class url_view;

    string_view s_;
    std::size_t n_;

    params_encoded_view(
        string_view s,
        std::size_t n) noexcept
        : s_(s)
        , n_(n)
    {
    }

public:
    class iterator;

    using value_type = params_value_type;
    using reference = value_type;
    using const_reference = value_type;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    //--------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------

    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    BOOST_URL_DECL
    iterator
    end() const noexcept;

    //--------------------------------------------
    //
    // Capacity
    //
    //--------------------------------------------

    bool
    empty() const noexcept
    {
        return n_ == 0;
    }

    std::size_t
    size() const noexcept
    {
        return n_;
    }

    //--------------------------------------------
    //
    // Lookup
    //
    //--------------------------------------------

    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    template<class Key>
#ifdef BOOST_URL_DOCS
    std::size_t
#else
    typename std::enable_if<
        is_stringlike<Key>::value,
        std::size_t>::type
#endif
    count(Key const& key) const noexcept
    {
        return count(to_string_view(key));
    }

    inline
    iterator
    find(string_view key) const noexcept;

    template<class Key>
#ifdef BOOST_URL_DOCS
    iterator
#else
    typename std::enable_if<
        is_stringlike<Key>::value,
        iterator>::type
#endif
    find(Key const& key) const noexcept;

    /** Search [from, end), from==end is valid
    */
    BOOST_URL_DECL
    iterator
    find(
        iterator from,
        string_view key) const noexcept;

    template<class Key>
#ifdef BOOST_URL_DOCS
    iterator
#else
    typename std::enable_if<
        is_stringlike<Key>::value,
        iterator>::type
#endif
    find(
        iterator from,
        Key const& key) const noexcept;

    inline
    bool
    contains(string_view key) const noexcept;

    template<class Key>
#ifdef BOOST_URL_DOCS
    bool
#else
    typename std::enable_if<
        is_stringlike<Key>::value,
        bool>::type
#endif
    contains(Key const& key) const noexcept
    {
        return contains(to_string_view(key));
    }

    BOOST_URL_DECL
    friend
    params_encoded_view
    parse_query_params_(
        string_view s,
        error_code& ec) noexcept;

    BOOST_URL_DECL
    friend
    params_encoded_view
    parse_query_params_(
        string_view s);
};

} // urls
} // boost

/** Return a query params view from a parsed string, using query-params bnf

    This function parses the string and returns the
    corresponding query params object if the string
    is valid, otherwise sets the error and returns
    an empty range. The query string should not
    include the leading question mark.

    @par BNF
    @code
    query-params    = [ query-param ] *( "&" [ query-param ] )
    query-param     = key [ "=" value ]
    @endcode

    @par Exception Safety
    No-throw guarantee.

    @param s The string to parse
    @param ec Set to the error, if any occurred

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4">
        3.4. Query (rfc3986)</a>

    @see @ref query_params_view
*/
BOOST_URL_DECL
params_encoded_view
parse_query_params_(
    string_view s,
    error_code& ec) noexcept;

/** Return a query params view from a parsed string, using query-params bnf

    This function parses the string and returns the
    corresponding query params object if the string
    is valid, otherwise throws an exception.
    The query string should not include the
    leading question mark.

    @par BNF
    @code
    query-params    = [ query-param ] *( "&" [ query-param ] )
    query-param     = key [ "=" value ]
    @endcode

    @throw system_error Thrown on error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4">
        3.4. Query (rfc3986)</a>

    @see @ref query_params_view
*/
BOOST_URL_DECL
params_encoded_view
parse_query_params_(
    string_view s);

// VFALCO This include is at the bottom of
// url_view.hpp because of a circular dependency
//#include <boost/url/impl/params_encoded_view.hpp>

#endif
