//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_HPP
#define BOOST_URL_SEGMENTS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <initializer_list>
#include <iterator>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_base;
class segments_encoded;
#endif

/** A container referencing a random-access range of modifiable, percent-decoded path segments.

    This class implements a <em>RandomAccessContainer</em>
    representing the path segments in a @ref url as
    percent-encoded strings. Ownership of the segments
    is not transferred; the container references the
    buffer in the url. Therefore, the lifetime of the
    url must remain valid until this container no
    longer exists.

    Objects of this type are not constructed directly;
    Instead, call the corresponding non-const member
    function of @ref url to obtain an instance of
    the container:

    @par Example
    @code
    url u = parse_relative_ref( "/path/to/file.txt" );

    segments se = u.segments();

    for( segments::value_type s : se )
        std::cout << s << std::endl;
    @endcode

    The @ref reference and @ref const_reference
    nested types are defined as publicly accessible
    nested classes. They proxy the behavior of a
    reference to a percent-encoded string in the
    underlying URL. The primary use of these
    references is to provide l-values that can be
    returned from element-accessing operations.
    Any reads or writes which happen through a
    @ref reference or @ref const_reference
    potentially read or write the underlying
    @ref url.

    @see
        @ref url.
*/
class segments
    : private detail::parts_base
{
    url_base* u_ = nullptr;

    friend class url_base;
    friend class segments_encoded;

    segments(url_base& u)
        : u_(&u)
    {
    }

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

    /** A type which can represent a segment as a reference

        This type does not make a copy of a segment
        and ownership is retained by the container.
    */
    using reference = decode_view;

    /// @copydoc reference
    using const_reference = decode_view;

    /** An unsigned integer type
    */
    using size_type = std::size_t;

    /** A signed integer type
    */
    using difference_type = std::ptrdiff_t;

    //--------------------------------------------
    //
    // Members
    //
    //--------------------------------------------

    /** Returns true if this contains an absolute path.

        Absolute paths always start with a
        forward slash ('/').
    */
    bool
    is_absolute() const noexcept;

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
    segments(segments const&) = default;

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
    segments&
    operator=(segments const&) & = default;

    /** Replace the contents of the container

        This function replaces the contents with
        an initializer list of unencoded strings.

        The behavior is undefined if any
        string refers to the contents of `*this`.

        All iterators and references to elements
        of the container are invalidated,
        including the @ref end iterator.

        @par Example
        @code
        url u = parse_relative_uri( "/path/to/file.txt" );
        u.segments() = { "etc", "init.rc" };
        assert( u.encoded_path() == "/etc/init.rc") );
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @param init An initializer list of strings.
    */
    segments&
    operator=(std::initializer_list<string_view> init);

    /** Replace the contents of the container

        This function replaces the contents
        with a range of percent-encoded
        strings.
        Each string must contain a valid
        percent-encoding or else an
        exception is thrown.
        The behavior is undefined if either
        argument is an iterator into `*this`.
        All iterators and references to elements
        of the container are invalidated,
        including the @ref end iterator.

        @par Requires
        @code
        std::is_convertible< std::iterator_traits< FwdIt >::value_type, string_view >::value == true
        @endcode

        @par Example
        @code
        url u = parse_relative_uri( "/path/to/file.txt" );

        segments se = u.segments();

        std::vector< std::string > v = { "etc", "init.rc" };

        se.insert( u.end() - 1, v.begin(), v.end() );

        assert( u.encoded_path() == "/etc/init.rc") );
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @param first An iterator to the first
        element in the range

        @param last An iterator to one past the
        last element in the range

        @throw std::invalid_argument invalid percent-encoding
    */
    template<class FwdIt>
#ifdef BOOST_URL_DOCS
    void
#else
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            string_view>::value>::type
#endif
    assign(FwdIt first, FwdIt last);

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
    decode_view
    front() const;

    /** Access the last element.

        Returns a reference to the last element.

        @par Precondition
        `not empty()`

        @par Complexity
        Constant.
    */
    decode_view
    back() const;

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

    /** Return the number of elements in the array.

        This returns the number of segments in the path.

        @par Complexity
        Constant.

        @par Exception Safety
        No-throw guarantee.
    */
    std::size_t
    size() const noexcept;

    //--------------------------------------------
    //
    // Modifiers
    //
    //--------------------------------------------

    /** Remove the contents of the container

        This function removes all the segments
        from the path, leaving the
        underlying URL with an empty path.

        @par Postconditions
        @code
        empty() == true
        @endcode

        @par Exception Safety
        Throws nothing.
    */
    void
    clear() noexcept;

    //--------------------------------------------

    /** Insert an element

        This function inserts a segment specified
        by the percent-encoded string `s`, at the
        position preceding `before`.
        The string must contain a valid
        percent-encoding, or else an exception
        is thrown.
        All references and iterators starting
        from the newly inserted element and
        up to and including the last element
        and @ref end iterators are invalidated.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @return An iterator pointing to the
        inserted value.

        @param before An iterator before which the
        new element should be inserted.

        @param s A valid percent-encoded string
        to be inserted.

        @throw std::invalid_argument invalid percent-encoding
    */
    BOOST_URL_DECL
    iterator
    insert(
        iterator before,
        string_view s);

    /** Insert a range of segments

        This function inserts a range of unencoded
        strings passed as an initializer-list.
        All references and iterators starting
        from the newly inserted elements and
        up to and including the last element
        and @ref end iterators are invalidated.

        @par Example
        @code
        url u = parse_relative_uri( "/path/file.txt" );
        segments se = u.segments();
        se.insert( u.end() - 1, { "to", "the" } );
        assert( u.encoded_path() == "/path/to/the/file.txt") );
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @note Behavior is undefined if any elements of
        initializer_list belong to the container

        @return An iterator to one past the last
        newly inserted element or `before` if
        the range is empty.

        @param before An iterator before which the
        new elements should be inserted.

        @param init The initializer list containing
        unencoded segments to insert.
    */
    iterator
    insert(
        iterator before,
        std::initializer_list<string_view> init);

    /** Insert a range of segments

        This function inserts a range
        of percent-encoded strings.
        Each string must contain a valid
        percent-encoding or else an
        exception is thrown.
        The behavior is undefined if either
        argument is an iterator into `this`.
        All references and iterators starting
        from the newly inserted elements and
        up to and including the last element
        and @ref end iterators are invalidated.

        @par Requires
        @code
        std::is_convertible< std::iterator_traits< FwdIt >::value_type, string_view >::value == true
        @endcode

        @par Example
        @code
        url u = parse_relative_uri( "/path/file.txt" );

        segments se = u.segments();

        std::vector< std::string > v = { "to", "the" };

        se.insert( u.end() - 1, v.begin(), v.end() );

        assert( u.encoded_path() == "/path/to/the/file.txt") );
        @endcode

        @note Behavior is undefined if any elements of the range
        belong to the container

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @return An iterator to one past the last
        newly inserted element or `before` if
        the range is empty.

        @param before An iterator before which the
        new element should be inserted.

        @param first An iterator to the first
        element to insert.

        @param last An iterator to one past the
        last element to insert.

        @throw std::invalid_argument invalid percent-encoding
    */
    template<class FwdIt>
#ifdef BOOST_URL_DOCS
    iterator
#else
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            string_view>::value,
        iterator>::type
#endif
    insert(
        iterator before,
        FwdIt first,
        FwdIt last);

private:
    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last,
        std::input_iterator_tag) = delete;

    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last,
        std::forward_iterator_tag);
public:

    //--------------------------------------------

    iterator
    replace(
        iterator pos,
        string_view s);

    iterator
    replace(
        iterator from,
        iterator to,
        std::initializer_list<
            string_view> init);

    template<class FwdIt>
#ifdef BOOST_URL_DOCS
    iterator
#else
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            string_view>::value,
        iterator>::type
#endif
    replace(
        iterator from,
        iterator to,
        FwdIt first,
        FwdIt last);

    //--------------------------------------------

    /** Erase an element

        This function erases the element pointed
        to by `pos`, which must be a valid
        iterator for the container.
        All references and iterators starting
        from pos and up to and including
        the last element and @ref end iterators
        are invalidated.

        @par Preconditions
        `pos` points to a valid element in
        this container.

        @par Example
        @code
        url u = parse_relative_uri( "/path/to/file.txt" );

        segments se = u.segments();

        se.erase( se.begin() + 1 );

        assert( u.encoded_path() == "/path/file.txt" );
        @endcode

        @par Exception Safety
        Throws nothing.

        @return An iterator following
        the last element erased.

        @param pos An iterator to the
        element to erase.
    */
    iterator
    erase(
        iterator pos) noexcept;

    /** Erase a range of elements

        This function erases the elements
        in the range `[first, last)`, which
        must be a valid range in the container.
        All references and iterators starting
        from `first` and up to and including
        the last element and @ref end iterators
        are invalidated.

        @par Preconditions
        `[first, last)` is a valid range in
        this container.

        @par Example
        @code
        url u = parse_relative_uri( "/path/to/the/file.txt" );

        segments se = u.segments();

        se.erase( se.begin() + 1, se.begin() + 3 );

        assert( u.encoded_path() == "/path/file.txt" );
        @endcode

        @return An iterator following
        the last element erased.

        @param first The beginning of the
        range to erase.

        @param last The end of the range
        to erase.

        @throw std::invalid_argument invalid percent-encoding
    */
    BOOST_URL_DECL
    iterator
    erase(
        iterator first,
        iterator last) noexcept;

    //--------------------------------------------

    /** Add an element to the end

        This function appends a segment
        containing the percent-encoded string
        `s` to the end of the container.
        The percent-encoding must be valid or
        else an exception is thrown.
        All @ref end iterators are invalidated.

        @par Example
        @code
        url u = parse_relative_uri( "/path/to" );

        u.segments().push_back( "file.txt" );

        assert( u.encoded_path() == "/path/to/file.txt" );
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @param s The string to add

        @throw std::invalid_argument invalid percent-encoding
    */
    void
    push_back(
        string_view s);

    /** Remove the last element

        This function removes the last element
        from the container, which must not be
        empty or else undefined behavior occurs.
        Iterators and references to
        the last element, as well as the
        @ref end iterator, are invalidated.

        @par Preconditions
        @code
        not empty()
        @endcode

        @par Example
        @code
        url u = parse_relative_uri( "/path/to/file.txt" );

        u.segments().pop_back();

        assert( u.encoded_path() == "/path/to" );
        @endcode

        @par Exception Safety
        Throws nothing.
    */
    void
    pop_back() noexcept;
};

} // urls
} // boost

// VFALCO This include is at the bottom of
// url.hpp because of a circular dependency
//#include <boost/url/impl/segments.hpp>

#endif
