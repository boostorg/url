//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_ENCODED_CONST_VIEW_HPP
#define BOOST_URL_PARAMS_ENCODED_CONST_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/param.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view_base;
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
    encoded params_view, then each parameter is
    printed to standard output:

    @code
    params_encoded_const_view pev = parse_query_params( "cust=John&id=42&last_invoice=1001" ).value();

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
class params_encoded_const_view
    : private detail::parts_base
{
    friend class url_view_base;

    url_view_base const* u_ = nullptr;

    params_encoded_const_view(
        url_view_base const& u) noexcept
        : u_(&u)
    {
    }

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
    using value_type = param;

    /** A type which can represent a parameter as a const reference

        This type does not make a copy of a parameter
        and ownership is retained by the container.
    */
    using reference = param_view;

    /// @copydoc reference
    using const_reference = param_view;

    /** An unsigned integer type to represent sizes.
    */
    using size_type = std::size_t;

    /** A signed integer type used to represent differences.
    */
    using difference_type = std::ptrdiff_t;

    //--------------------------------------------

    /** Constructor

        A default-constructed instance will be
        an empty range.
    */
    params_encoded_const_view() = default;

    /** Constructor

        After the copy both views will point to
        the same character buffer.

        Ownership is not transferred; the caller
        is responsible for ensuring the lifetime
        of the character buffer extends until
        it is no longer referenced.

        @par Complexity
            Constant

        @par Exception Safety
            Throws nothing

    */
    params_encoded_const_view(
        params_encoded_const_view const&) = default;

    /** Assignment

        After the assignment, both views will point to
        the same underlying object.

        Ownership is not transferred; the caller
        is responsible for ensuring the lifetime
        of the character buffer extends until
        it is no longer referenced.

        @par Complexity
            Constant

        @par Exception Safety
            Throws nothing

    */
    params_encoded_const_view&
    operator=(params_encoded_const_view const&) & = default;

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
};

} // urls
} // boost

// This is in <boost/url/url_view_base.hpp>
//
// #include <boost/url/impl/params_encoded_const_view.hpp>

#endif
