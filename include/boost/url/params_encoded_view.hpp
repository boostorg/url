//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_ENCODED_VIEW_HPP
#define BOOST_URL_PARAMS_ENCODED_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/params_view.hpp>
#include <boost/url/query_param.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view;
#endif

/** A forward range of read-only encoded query parameters.

    Objects of this type represent an iterable
    range of query parameters, where each parameter
    is represented by percent-encoded key and value
    strings.

    Dereferenced iterators return a structure of
    string views into the underlying character
    buffer.

    Ownership of the underlying characters is
    not transferred; the character buffer used
    to construct the container must remain
    valid for as long as the container exists.

    The container models a multi-map. That is,
    duplicate keys are possible.

    A view of encoded parameters in a URL's query
    can be obtained by calling
        @ref url_view::encoded_params.
    Alternatively, to obtain encoded parameters
    from a query stored in a string call the
    parsing function (see below).

    @par Example

    A query parameter string is parsed into
    encoded params, then each parameter is
    printed to standard output:

    @code
    params_encoded_view pev = parse_query_params( "cust=John&id=42&last_invoice=1001" ).value();

    for(auto e : pev)
        std::cout << "key = " << e.key << ", value = " << e.value << std::endl;
    @endcode

    @par Complexity
    Iterator increment runs in linear time on
    the size of the parameter.
    All other operations run in constant time.
    No operations allocate memory.

    @see
        @ref parse_query_params.
*/
class params_encoded_view
    : private detail::parts_base
{
    friend class url_view;

    string_view s_;
    std::size_t n_ = 0;

    params_encoded_view(
        string_view s,
        std::size_t n) noexcept;

public:
    /** A read-only forward iterator to an encoded query parameter.

        This is a read-only forward iterator to
        the encoded query parameters.

     */
#ifdef BOOST_URL_DOCS
    using iterator = __see_below__;
#else
    class iterator;
#endif

    /// @copydoc iterator
    using const_iterator = iterator;

    /** A type which can represent a parameter as a value

        This type allows for making a copy of
        a parameter where ownership is retained
        in the copy.

    */
    using value_type = query_param;

    /** A type which can represent a parameter as a const reference

        This type does not make a copy of a parameter
        and ownership is retained by the container.
    */
    using reference = query_param_encoded_view;

    /// @copydoc reference
    using const_reference = reference;

    /** An unsigned integer type to represent sizes.
    */
    using size_type = std::size_t;

    /** A signed integer type used to represent differences.
    */
    using difference_type = std::ptrdiff_t;

    /** Return a view of this container as percent-decoded query parameters

        This function returns a new view over the
        same underlying character buffer where each
        segment is returned as a read-only string
        with percent-decoding applied using the
        optionally specified allocator.

        The decoded view does not take ownership of
        the underlying character buffer; the caller
        is still responsible for ensuring that the
        buffer remains valid until all views which
        reference it are destroyed.

        @par Exceptions
        Calls to allocate may throw.

        @return A view to decoded path segments.

        @param alloc The allocator the returned
        view will use for all string storage. If
        this parameter is ommitted, the default
        allocator will be used.
    */
    params_view
    decoded() const
    {
        return {s_, n_};
    }

    /** Constructor

        Copy constructor
    */
    params_encoded_view(params_encoded_view const&) = default;

    /** Assignment

        After the assignment, both views will point to
        the same underlying object.
    */
    params_encoded_view&
    operator=(params_encoded_view const&) & = default;

    //--------------------------------------------
    //
    // Element Access
    //
    //--------------------------------------------

    /** Return the first element matching the key.

        This function searches the container for
        the specified encoded key. If the key
        is found, the corresponding value is
        returned. If the value does not exist,
        an empty string is returned. Otherwise,
        if the key does not exist, an exception
        is thrown.

        If multiple parameters match the key,
        only the first match is returned.

        @par Example
        @code
        params_encoded_view pev = parse_query_params( "cust=John&id=42&last_invoice=1001" ).value();

        std::cout << pev.at( "cust" ); // prints "John"
        @endcode

        @return The value as an encoded string.

        @param key The encoded key.

        @throws std::out_of_range Key not found.
    */
    BOOST_URL_DECL
    string_view
    at(string_view key) const;

    //--------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------

    /** Return an iterator to the beginning.
    */
    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    /** Return an iterator to the end.
    */
    BOOST_URL_DECL
    iterator
    end() const noexcept;

    //--------------------------------------------
    //
    // Capacity
    //
    //--------------------------------------------

    /** Return true if the range contains no elements.
    */
    bool
    empty() const noexcept;

    /** Return the number of elements in the range.
    */
    std::size_t
    size() const noexcept;

    //--------------------------------------------
    //
    // Lookup
    //
    //--------------------------------------------

    /** Return the number of matching elements.

        This function returns the total number
        of elements whose key matches the
        specified encoded string.

        @par Exception Safety
        Throws nothing.

        @return The number of elements.

        @param key The encoded key.
    */
    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    /** Return the first element matching the key

        This function returns the first
        element which matches the specified
        percent-encoded key. If no element
        matches, then an iterator to the end
        is returned.

        @par Exception Safety
        Throws nothing.

        @return An iterator to the element.

        @param key The encoded key.
    */
    iterator
    find(string_view key) const noexcept;

    /** Return the next element matching the key

        This function returns the first
        element which matches the specified
        percent-encoded key, starting the
        search at `*from` inclusive.

        If no element matches the key, then an
        iterator to the end is returned.

        @par Exception Safety
        Throws nothing.

        @return An iterator to the element

        @param from An iterator to the element
        to start from. The range `[ from, end() )`
        is searched.

        @param key The encoded key.
    */
    BOOST_URL_DECL
    iterator
    find(
        iterator from,
        string_view key) const noexcept;

    /** Return true if at least one matching element exists.

        This function returns true if at least one
        element matches the specified percent-encoded
        key.

        @par Exception Safety
        Throws nothing.

        @return `true` if a matching element exists.

        @param key The encoded key.
    */
    bool
    contains(string_view key) const noexcept;

    //--------------------------------------------
    //
    // Parsing
    //
    //--------------------------------------------

    BOOST_URL_DECL friend
    result<params_encoded_view>
    parse_query_params(string_view s) noexcept;
};

//------------------------------------------------

/** Return a query params view from a parsed string, using query-params bnf

    This function parses the string and returns the
    corresponding query params object if the string
    is valid, otherwise sets the error and returns
    an empty range. The query string should not
    include the leading question mark.

    @par BNF
    @code
    query-params    = query-param *( "&" query-param )
    query-param     = key [ "=" value ]

    key             = *qpchar
    value           = *( qpchar / "=" )
    qpchar          = unreserved
                    / pct-encoded
                    / "!" / "$" / "'" / "(" / ")"
                    / "*" / "+" / "," / ";"
                    / ":" / "@" / "/" / "?"
    @endcode

    @par Exception Safety
    No-throw guarantee.

    @return The encoded parameter view, or an
    error if parsing failed.

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4">
        3.4. Query (rfc3986)</a>

    @see
        @ref params_encoded_view,
        @ref result.
*/
BOOST_URL_DECL
result<params_encoded_view>
parse_query_params(
    string_view s) noexcept;

} // urls
} // boost

// VFALCO This include is at the bottom of
// url_view.hpp because of a circular dependency
//#include <boost/url/impl/params_encoded_view.hpp>

#endif
