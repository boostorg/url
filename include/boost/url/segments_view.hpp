//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_VIEW_HPP
#define BOOST_URL_SEGMENTS_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <cstddef>
#include <iosfwd>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
// VFALCO is this needed?
class url_view;
class segments_encoded_view;
#endif

/** A bidirectional range of read-only path segment strings with percent-decoding applied.

    @see
        @ref segments_encoded_view.
*/
class segments_view
{
    string_view s_;
    std::size_t n_ = 0;

    friend class url_view;
    friend class segments_encoded_view;

    segments_view(
        string_view s,
        std::size_t n)
        : s_(s)
        , n_(n)
    {}

public:
    /** A read-only bidirectional iterator to a decoded segment.

        This is a read-only bidirectional iterator to
        the decoded segments.

     */
#ifdef BOOST_URL_DOCS
    using iterator = __see_below__;
#else
    class iterator;
#endif

    /// @copydoc iterator
    using const_iterator = iterator;

    /** A type which can represent a segment as a value

        This type allows for making a copy of
        a segment where ownership is retained
        in the copy.
    */
    using value_type = std::string;

    /** A type which can represent a segment as a const reference

        This type does not make a copy of a segment
        and ownership is retained by the container.

    */
    using reference = pct_encoded_view;

    /// @copydoc reference
    using const_reference = pct_encoded_view;

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

    /** Constructor

        Default constructed views represent an
        empty path.
    */
    segments_view() noexcept = default;

    /** Constructor

        Copy constructor
    */
    segments_view(segments_view const& other) = default;

    /** Assignment

        After the assignment, both views will point to
        the same underlying object.
    */
    segments_view&
    operator=(segments_view const& other) & = default;


    /** Returns true if this contains an absolute path.

        Absolute paths always start with a
        forward slash ('/').
    */
    bool
    is_absolute() const noexcept;

    //--------------------------------------------
    //
    // Element Access
    //
    //--------------------------------------------

    /** Return the first element.
    */
    pct_encoded_view
    front() const noexcept;

    /** Return the last element.
    */
    pct_encoded_view
    back() const noexcept;

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

    /** Return true if the range contains no elements
    */
    bool
    empty() const noexcept;

    /** Return the number of elements in the range
    */
    std::size_t
    size() const noexcept;

    //--------------------------------------------
    //
    // Friends
    //
    //--------------------------------------------

    /** Format the object to an output stream
    */
    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        segments_view const& vw)
    {
        vw.write(os);
        return os;
    }

private:
    BOOST_URL_DECL
    void
    write(std::ostream& os) const;

};

//----------------------------------------------------------


} // urls
} // boost

#include <boost/url/impl/segments_view.hpp>

#endif
