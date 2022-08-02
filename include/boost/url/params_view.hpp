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

        Default constructor
    */
    params_view() = default;

    /** Constructor

        Copy constructor
    */
    params_view(params_view const&) = default;

    /** Assignment

        After the assignment, both views will point to
        the same underlying object.
    */
    params_view&
    operator=(params_view const&) & = default;

    //--------------------------------------------
    //
    // Element Access
    //
    //--------------------------------------------

    BOOST_URL_DECL
    pct_encoded_view
    at(string_view key) const;

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
    empty() const noexcept;

    std::size_t
    size() const noexcept;

    //--------------------------------------------
    //
    // Lookup
    //
    //--------------------------------------------

    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    iterator
    find(string_view key) const noexcept;

    /** Search [from, end), from==end is valid
    */
    BOOST_URL_DECL
    iterator
    find(
        iterator from,
        string_view key) const noexcept;

    bool
    contains(string_view key) const noexcept;
};

} // urls
} // boost

#include <boost/url/impl/params_view.hpp>

#endif
