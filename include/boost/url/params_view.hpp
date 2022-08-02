//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_VIEW_HPP
#define BOOST_URL_PARAMS_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <boost/url/query_param.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view;
class params_encoded_view;
#endif

class params_view
    : private detail::parts_base
{
    friend class url_view;
    friend class params_encoded_view;

    string_view s_;
    std::size_t n_ = 0;

    params_view(
        string_view s,
        std::size_t n)
        : s_(s)
        , n_(n)
    {}

public:
    /** A read-only forward iterator to a decoded query parameter.

        This is a read-only forward iterator to
        the decoded query parameters.

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
    using reference = query_param_view;

    /// @copydoc reference
    using const_reference = reference;

    /** An unsigned integer type to represent sizes.
    */
    using size_type = std::size_t;

    /** A signed integer type used to represent differences.
    */
    using difference_type = std::ptrdiff_t;

    /** Constructor

        A default-constructed instance will be
        an empty range.

    */
    params_view() = default;

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
    params_view(params_view const&) = default;

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
    params_view&
    operator=(params_view const&) & = default;

    //--------------------------------------------
    //
    // Element Access
    //
    //--------------------------------------------

    /** Access the specified element, with bounds checking.

        Returns a constant reference to the mapped value of
        the element that matches `key`, otherwise throws.

        @par Complexity
        Linear @ref size().

        @par Exception Safety
        Strong guarantee.

        @return A reference to the mapped value.

        @param key The key of the element to find.

        @throw std::out_of_range if no such element exists.
    */
    BOOST_URL_DECL
    pct_encoded_view
    at(string_view key) const;

    //--------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------

    /** Return a const iterator to the first element.

        If the container is empty, @ref end() is returned.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    /** Return an iterator to the element following the last element.

        The element acts as a placeholder; attempting
        to access it results in undefined behavior.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_URL_DECL
    iterator
    end() const noexcept;

    //--------------------------------------------
    //
    // Capacity
    //
    //--------------------------------------------

    /** Return whether there are no elements.

        Returns `true` if there are no elements in
        the container, i.e. @ref size() returns 0.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.

    */
    bool
    empty() const noexcept;

    /** Return the number of elements.

        This returns the number of elements in the container.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::size_t
    size() const noexcept;

    //--------------------------------------------
    //
    // Lookup
    //
    //--------------------------------------------

    /** Count the number of elements with a specific key

        This function returns the count of the number of
        elements match `key`. The only possible return values
        are 0 and 1.

        @par Complexity
        Linear in @ref size().

        @par Exception Safety
        No-throw guarantee.

        @param key The key of the element to find.
    */
    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    /** Find an element with a specific key

        This function returns an iterator to the element
        matching `key` if it exists, otherwise returns
        @ref end().

        @par Complexity
        Linear in @ref size().

        @par Exception Safety
        No-throw guarantee.

        @param key The key of the element to find.
    */
    iterator
    find(string_view key) const noexcept;

    /**
    */

    /** Find an element with a specific key

        This function searches `[from, end)`,
        where `from==end` is valid.

        It returns a constant iterator to
        the element matching `key` if it exists,
        otherwise returns @ref end().

        @par Complexity
        Linear in @ref size().

        @par Exception Safety
        No-throw guarantee.

        @param key The key of the element to find.
    */
    BOOST_URL_DECL
    iterator
    find(
        iterator from,
        string_view key) const noexcept;

    /** Return `true` if the key is found

        This function returns `true` if a key with the
        specified string is found.

        @par Effects
        @code
        return this->find(key) != this->end();
        @endcode

        @par Complexity
        Linear in @ref size().

        @par Exception Safety
        No-throw guarantee.

        @param key The key of the element to find.

        @see @ref find
    */
    bool
    contains(string_view key) const noexcept;
};

} // urls
} // boost

#include <boost/url/impl/params_view.hpp>

#endif
