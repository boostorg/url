//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_QUERY_PARAMS_VIEW_HPP
#define BOOST_URL_QUERY_PARAMS_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/pct_encoding.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <iosfwd>
#include <utility>

namespace boost {
namespace urls {

/** A ForwardRange view of read-only query parameters

    Objects of this type provide an iterable,
    bidirectional range which references a string
    without taking ownership. The caller is
    responsible for ensuring that the lifetime of
    the underlying string extends until the path
    view is destroyed. This is the same ownership
    model as @ref string_view.

    The query parameters in a URL are arranged
    as a series of key/value pairs separated by
    ampersands as shown in the BNF below:

    @par BNF
    @code
    query-params    = [ query-param ] *( "&" [ query-param ] )
    query-param     = key [ "=" value ]
    @endcode

    @see @ref parse_query_params
*/
class query_params_view
{
    string_view s_;
    std::size_t n_;

    friend class url;
    friend class url_view;

    query_params_view(
        string_view s,
        std::size_t n)
        : s_(s)
        , n_(n)
    {
    }

public:
    class value_type;
    class iterator;

    query_params_view(
        query_params_view const&) = default;
    query_params_view& operator=(
        query_params_view const&) = default;

    BOOST_URL_DECL
    query_params_view() noexcept;

    /** Return true if the range contains no elements
    */
    bool
    empty() const noexcept
    {
        return n_ == 0;
    }

    /** Return the number of elements in the range
    */
    std::size_t
    size() const noexcept
    {
        return n_;
    }

    /** Return the full encoded query string
    */
    string_view
    encoded_query() const noexcept
    {
        return s_;
    }

    /** Return an iterator to the beginning of the range
    */
    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    /** Return an iterator to the end of the range
    */
    BOOST_URL_DECL
    iterator
    end() const noexcept;

    /** Return true if the key exists
    */
    BOOST_URL_DECL
    bool
    contains(string_view key) const noexcept;

    /** Return the number of matching keys

        @par Exception Safety
        Does not throw.

        @return The number of matching keys.

        @param key The key to search for. This
        should not be percent-encoded.
    */
    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    /** Find the first occurrence of a key

        @par Exception Safety
        Does not throw.

        @return An iterator to the matching
        element, or @ref end() if no element
        is found.

        @param key The key to search for. This
        should not be percent-encoded.
    */
    BOOST_URL_DECL
    iterator
    find(string_view key) const noexcept;

    /** Find the next occurrence of a key

        @par Exception Safety
        Does not throw.

        @return An iterator to the matching
        element, or @ref end() if no element
        is found.

        @param after The iterator after which
        the search will start from.
        
        @param key The key to search for. This
        should not be percent-encoded.
    */
    BOOST_URL_DECL
    iterator
    find(
        iterator after,
        string_view key) const noexcept;

    /** Return the value for a key, or the empty string

        @return The percent-decoded value if the
        key exists, otherwise an empty string.

        @param key The key to search for. This
        should not be percent-encoded.
    */
    BOOST_URL_DECL
    std::string
    operator[](string_view key) const;

    /** Return the value for the first matching key if it exists, otherwise throw

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @return The percent-decoded value

        @param key The key to search for. This
        should not be percent-encoded.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.
    */
    template<class Allocator =
        std::allocator<char>>
    string_type<Allocator>
    at( string_view key,
        Allocator const& a = {}) const;

    BOOST_URL_DECL
    friend
    query_params_view
    parse_query_params(
        string_view s,
        error_code& ec) noexcept;

    BOOST_URL_DECL
    friend
    query_params_view
    parse_query_params(
        string_view s);
};

//----------------------------------------------------------

/** The value type for query parameters
*/
class query_params_view::value_type
{
    pct_encoded_str k_;
    pct_encoded_str v_;
    bool has_value_ = false;

    friend class iterator;
    friend class query_params_view;

public:
    value_type() = default;
    value_type& operator=(
        value_type const&) = default;
    value_type(
        value_type const&) = default;

    /** Return the key

        This function returns the key as
        a percent-encoded string.

        @see key
    */
    string_view
    encoded_key() const noexcept
    {
        return k_.str;
    }

    /** Return the key

        This function returns the key as a
        string with percent-decoding applied.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    key(Allocator const& a = {}) const
    {
        return detail::pct_decode_unchecked(
            k_.str, k_.decoded_size, {}, a);
    }

    /** Return the key

        This function returns the key as a
        string with percent-decoding applied,
        optionally disabling the conversion
        of plus signs to spaces.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param plus_to_space A bool indicating
        whether or not plus signs in the encoded
        string are converted to spaces.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    key(bool plus_to_space,
        Allocator const& a = {}) const
    {
        pct_decode_opts opt;
        opt.plus_to_space = plus_to_space;
        return detail::pct_decode_unchecked(
            k_.str, k_.decoded_size, opt, a);
    }

    /** Return true if a value exists for this element
    */
    bool
    has_value() const noexcept
    {
        return has_value_;
    }

    /** Return the value if it exists, or an empty string
    */
    string_view
    encoded_value() const noexcept
    {
        return v_.str;
    }

    /** Return the value if it exists, or an empty string

        This function returns the value as a
        string with percent-decoding applied.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    value(Allocator const& a = {}) const
    {
        return detail::pct_decode_unchecked(
            v_.str, v_.decoded_size, {}, a);
    }

    /** Return the value if it exists, or an empty string

        This function returns the value as a
        string with percent-decoding applied,
        optionally disabling the conversion
        of plus signs to spaces.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param plus_to_space A bool indicating
        whether or not plus signs in the encoded
        string are converted to spaces.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    value(
        bool plus_to_space,
        Allocator const& a = {}) const
    {
        pct_decode_opts opt;
        opt.plus_to_space = plus_to_space;
        return detail::pct_decode_unchecked(
            v_.str, v_.decoded_size, opt, a);
    }

    value_type const*
    operator->() const noexcept
    {
        return this;
    }

    /** Return the decoded key and value as a pair

        This function returns the key and value
        as a pair, using the default allocator.
        It is designed to allow easy copying
        of the range into another container, as
        shown below:

        @par Example
        @code
        query_params_view qp = parse_query_params( "a=1&b=2&c=3&d=%34" );

        std::map< std::string, std::string > m( qp.begin(), qp.end() );

        // m now holds the key value pairs from the query params
        @endcode
    */
    operator
    std::pair<
        std::string const,
        std::string>() const
    {
        return { key(), value() };
    }
};

//------------------------------------------------

/** Format the encoded query to an output stream
*/
BOOST_URL_DECL
std::ostream&
operator<<(
    std::ostream& os,
    query_params_view const& qpv);

//------------------------------------------------

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
query_params_view
parse_query_params(
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
    @param ec Set to the error, if any occurred

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4">
        3.4. Query (rfc3986)</a>

    @see @ref query_params_view
*/
BOOST_URL_DECL
query_params_view
parse_query_params(
    string_view s);

} // urls
} // boost

#include <boost/url/impl/query_params_view.hpp>

#endif
