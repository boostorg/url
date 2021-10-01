//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_VIEW_HPP
#define BOOST_URL_SEGMENTS_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <cstddef>
#include <iosfwd>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DECL
class segments_encoded_view;
#endif

/** A BidirectionalRange view of read-only path segments with percent-decoding applied
*/
class segments_view
{
    string_view s_;
    std::size_t n_;
    string_value::allocator a_;

    friend class url;
    friend class url_view;
    friend class segments_encoded_view;

    template<class Allocator>
    segments_view(
        string_view s,
        std::size_t n,
        Allocator const& a)
        : s_(s)
        , n_(n)
        , a_(a)
    {
    }

public:
#ifdef BOOST_URL_DOCS
    /** An iterator to a read-only decoded path segment.
    */
    using iterator = __see_below__;
#else
    class iterator;
#endif
    /** The type of value returned when dereferencing an iterator.
    */
    using value_type = string_value;

    /** The type of value returned when dereferencing an iterator.
    */
    using reference = string_value;

    /** The type of value returned when dereferencing an iterator.
    */
    using const_reference = string_value;

    /** The unsigned integer type used to represent size.
    */
    using size_type = std::size_t;

    /** The signed integer type used to represent differences.
    */
    using difference_type = std::ptrdiff_t;

    //--------------------------------------------
    //
    // Members
    //
    //--------------------------------------------

    BOOST_URL_DECL
    segments_view(
        segments_view const&) noexcept;

    segments_view& operator=(
        segments_view const&) noexcept = delete;

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

    /** Returns true if this contains an absolute path.

        Absolute paths always start with a
        forward slash ('/').
    */
    inline
    bool
    is_absolute() const noexcept;

    //--------------------------------------------
    //
    // Element Access
    //
    //--------------------------------------------

    /** Return the first element.
    */
    inline
    string_value
    front() const noexcept;

    /** Return the last element.
    */
    inline
    string_value
    back() const noexcept;

    //--------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------

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
};

} // urls
} // boost

#include <boost/url/impl/segments_view.hpp>

#endif
