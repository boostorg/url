//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_ENCODED_VIEW_HPP
#define BOOST_URL_SEGMENTS_ENCODED_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/segments_view.hpp>
#include <boost/url/string_view.hpp>
#include <iosfwd>
#include <utility>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
// VFALCO is this needed?
class url_view;
#endif

/** A bidirectional range of read-only encoded path segment strings.

    Objects of this type represent an iterable
    range of path segments, where each segment
    is represented by a percent-encoded string.

    Dereferenced iterators return string views
    into the underlying character buffer.

    Ownership of the underlying characters is
    not transferred; the character buffer used
    to construct the container must remain
    valid for as long as the container exists.

    A view of encoded segments in a URL's path
    can be obtained by calling
        @ref url_view::encoded_segments.
    Alternatively, to obtain encoded segments
    from a path stored in a string call one of
    the parsing functions (see below).

    @par Examples

    A path string is parsed into encoded
    segments, then each segment is printed to
    standard output:

    @code
    segments_encoded_view sev = parse_path( "/path/to/file.txt" ).value();

    for( auto it = sev.begin(); it != sev.end(); ++it )
        std::cout << *it << std::endl;
    @endcode

    A URL containing a path is parsed, then a
    view to the encoded segments is obtained
    and formatted to standard output:

    @code
    url_view u = parse_uri( "http://example.com/path/to/file.txt" ).value();

    segments_encoded_view sev = u.encoded_segments();

    std::cout << sev << std::endl;
    @endcode

    @par Complexity

    Iterator increment or decrement runs in
    linear time on the size of the segment.
    All other operations run in constant time.
    No operations allocate memory.

    @see
        @ref parse_path,
        @ref segments_view.
*/
class segments_encoded_view
{
    string_view s_;
    std::size_t n_ = 0;

    friend class url_view_base;

    BOOST_URL_DECL
    segments_encoded_view(
        string_view s,
        std::size_t n) noexcept;

public:
    /** A read-only bidirectional iterator to an encoded segment.

        This is a read-only bidirectional iterator to
        the encoded segments.

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
    using reference = string_view;

    /// @copydoc reference
    using const_reference = string_view;

    /** An unsigned integer type used to represent size.
    */
    using size_type = std::size_t;

    /** A signed integer type used to represent differences.
    */
    using difference_type = std::ptrdiff_t;

    //--------------------------------------------
    //
    // Members
    //
    //--------------------------------------------

    /** Constructor

        A default-constructed instance will be
        an empty range.
    */
    segments_encoded_view() noexcept;

    /** Constructor

        After the copy, both views will point to
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
    segments_encoded_view(segments_encoded_view const&) noexcept = default;

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
    segments_encoded_view&
    operator=(segments_encoded_view const&) & = default;

    /** Return a view of this container as percent-decoded segments

        This function returns a new view over the
        same underlying character buffer where each
        segment is returned as a @ref string_view
        with percent-decoding applied using the
        optionally specified allocator.

        The decoded view does not take ownership of
        the underlying character buffer; the caller
        is still responsible for ensuring that the
        buffer remains valid until all views which
        reference it are destroyed.

        @par Example
        @code
        segments_encoded_view sev = parse_path( "/%70%61%74%68/%74%6f/%66%69%6c%65%2e%74%78%74" ).value();

        segments_view sv = sev.decoded();

        std::stringstream ss;

        ss << sv.front() << "/../" << sv.back();

        assert( ss.string() == "path/../file.txt" );
        @endcode

        @par Exceptions
        Calls to allocate may throw.

        @return A view to decoded path segments.

    */
    segments_view
    decoded() const
    {
        return {s_, n_};
    }

    /// @copydoc decoded()
    operator segments_view() const
    {
        return decoded();
    }

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

    /** Access the first element.

        Returns a reference to the first element.

        @par Precondition
        `not empty()`

        @par Complexity
        Constant.
    */
    string_view
    front() const noexcept;

    /** Access the last element.

        Returns a reference to the last element.

        @par Precondition
        `not empty()`

        @par Complexity
        Constant.
    */
    string_view
    back() const noexcept;

    //--------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------

    /** Return an iterator to the first element.

        If the path is empty, @ref end() is returned.

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

    /** Check if the path has no segments.

        Returns `true` if there are no segments in the
        path, i.e. @ref size() returns 0.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    bool
    empty() const noexcept;

    /** Return the number of segments in the path.

        This returns the number of segments in the path.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::size_t
    size() const noexcept;

    //--------------------------------------------

    // hidden friend
    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        segments_encoded_view const& pv)
    {
        return os << pv.s_;
    }

    BOOST_URL_DECL friend
        result<segments_encoded_view>
        parse_path(string_view s) noexcept;
};

/** Format the encoded segments to an output stream.

    @param os The output stream.

    @param pv The encoded segments.
*/
std::ostream&
operator<<(
    std::ostream& os,
    segments_encoded_view const& pv);

//----------------------------------------------------------

/** Parse a string and return an encoded segment view

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise returns an error.

    @par BNF
    @code
    path          = [ "/" ] segment *( "/" segment )
    @endcode

    @par Exception Safety
    No-throw guarantee.

    @return A valid view on success, otherwise an
    error code.

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path,
        @ref segments_encoded_view.
*/
BOOST_URL_DECL
result<segments_encoded_view>
parse_path(string_view s) noexcept;

} // urls
} // boost

#include <boost/url/impl/segments_encoded_view.hpp>

#endif
