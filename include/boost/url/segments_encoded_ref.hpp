//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_ENCODED_REF_HPP
#define BOOST_URL_SEGMENTS_ENCODED_REF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/segments_encoded_base.hpp>
#include <initializer_list>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_base;
class segments_encoded_view;
#endif

/** A view representing path segments in a URL

    Objects of this type are used to interpret
    the path as a bidirectional view of segment
    strings. 

    The view does not retain ownership of the
    elements and instead references the original
    character buffer. The caller is responsible
    for ensuring that the lifetime of the buffer
    extends until it is no longer referenced.

    The view is modifiable; calling non-const
    members will cause changes to the referenced
    url.

    @par Example
    @code
    url u( "/path/to/file.txt" );

    segments_encoded_ref ps = u.segments();
    @endcode

    Strings produced when elements are returned
    have type @ref param_pct_view and represent
    encoded strings. Strings passed to member
    functions may contain percent escapes, and
    throw exceptions on invalid inputs.

    @par Iterator Invalidation
    Changes to the underlying character buffer
    can invalidate iterators which reference it.
    Modifications made through the container will
    invalidate some iterators to the underlying
    character buffer:
    @li @ref push_back : Only `end()`.
    @li @ref assign, @ref clear,
        `operator=` : All elements.
    @li @ref erase : Erased elements and all
        elements after (including `end()`).
    @li @ref insert : All elements at or after
        the insertion point (including `end()`).
    @li @ref replace : Modified
        elements and all elements
        after (including `end()`).

    @see
        @ref segments_encoded_view,
        @ref segments_view,
        @ref segments_ref.
*/
class segments_encoded_ref
    : public segments_encoded_base
{
    friend class url_base;

    url_base* u_ = nullptr;

    segments_encoded_ref(
        url_base& u) noexcept;

public:
    //--------------------------------------------
    //
    // Special Members
    //
    //--------------------------------------------

    /** Constructor

        After construction, both views will
        reference the same url. Ownership is not
        transferred; the caller is responsible
        for ensuring the lifetime of the url
        extends until it is no longer
        referenced.

        @par Postconditions
        @code
        &this->url() == &other.url();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.

        @param other The other view.
    */
    segments_encoded_ref(
        segments_encoded_ref const& other) = default;

    /** Assignment

        The previous contents of this are
        replaced by a copy of the other segments.

        <br>
        All iterators are invalidated.

        @note
        The strings referenced by `other`
        must not come from the underlying url,
        or else the behavior is undefined.

        @par Effects
        @code
        this->assign( other.begin(), other.end() );
        @endcode

        @par Complexity
        Linear in `other.buffer().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param other The segments to assign.
    */
    /** @{ */
    segments_encoded_ref&
    operator=(segments_encoded_ref const& other);

    segments_encoded_ref&
    operator=(segments_encoded_view const& other);
    /** @} */

    /** Assignment

        The previous contents of this are
        replaced by a copy of the segments in
        the initializer-list, whose strings may
        contain percent-escapes.

        <br>
        All iterators are invalidated.

        @par Preconditions
        None of character buffers referenced by
        `init` may overlap the character buffer of
        the underlying url, or else the behavior
        is undefined.

        @par Effects
        @code
        this->assign( init.begin(), init.end() );
        @endcode

        @par Complexity
        Linear in `init.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @throw system_error
        `init` contains an invalid percent-encoding.

        @param init The list of segments to assign.
    */
    segments_encoded_ref&
    operator=(std::initializer_list<
        pct_string_view> init);

    /** Conversion
    */
    operator
    segments_encoded_view() const noexcept;

    //--------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------

    /** Return the referenced url

        This function returns the url referenced
        by the view.

        @par Example
        @code
        url u( "/path/to/file.txt" );

        assert( &u.encoded_segments().url() == &u );
        @endcode

        @par Exception Safety
        @code
        Throws nothing.
        @endcode
    */
    url_base&
    url() const noexcept
    {
        return *u_;
    }

    //--------------------------------------------
    //
    // Modifiers
    //
    //--------------------------------------------

    /** Clear the contents of the container

        <br>
        All iterators are invalidated.

        @par Effects
        @code
        this->url().set_encoded_path( "" );
        @endcode

        @par Postconditions
        @code
        this->empty() == true
        @endcode

        @par Complexity
        Linear in `this->url().size()`.

        @par Exception Safety
        Throws nothing.
    */
    void
    clear() noexcept;

    /** Assign params

        This function replaces the entire
        contents of the view with the params
        in the <em>initializer-list</em>.

        <br>
        All iterators are invalidated.

        @note
        The strings referenced by the params
        must not come from the underlying url,
        or else the behavior is undefined.

        @par Example
        @code
        url u;

        u.encoded_params().assign( {{ "first", "John" }, { "last", "Doe" }} );
        @endcode

        @par Complexity
        Linear in `init.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @throw system_error
        `init` contains an invalid percent-encoding.

        @param init The list of params to assign.
    */
    segments_encoded_ref&
    assign(std::initializer_list<pct_string_view> init);

    /** Assign segments

        This function replaces the entire
        contents of the view with the segments
        in the range.

        <br>
        All iterators are invalidated.

        @note
        The strings referenced by the segments
        must not come from the underlying url,
        or else the behavior is undefined.

        @par Mandates
        @code
        std::is_convertible< std::iterator_traits< FwdIt >::reference_type, param_pct_view >::value == true
        @endcode

        @par Complexity
        Linear in the size of the range.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @throw system_error
        The range contains an invalid percent-encoding.

        @param first, last The range of segments
        to assign.
    */
    template<class FwdIt>
#ifdef BOOST_URL_DOCS
    void
#else
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            pct_string_view>::value>::type
#endif
    assign(FwdIt first, FwdIt last);

    //--------------------------------------------

    /** Insert segments

        This function inserts a string as a
        segment, before the specified position.
        Escapes in the string are preserved,
        and reserved characters in the string
        are percent-escaped in the result.

        <br>
        All iterators that are equal to
        `before` or come after are invalidated.

        @par Example
        @code
        @endcode

        @par Complexity
        Linear in `this->url().size() + s.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @throw system_error
        `s` contains an invalid percent-encoding.

        @return An iterator to the inserted
        segment.

        @param before An iterator before which
        the segment will be inserted. This may
        be equal to `end()`.

        @param s The segment to insert.
    */
    BOOST_URL_DECL
    iterator
    insert(
        iterator before,
        pct_string_view s);

    /** Insert segments

        This function inserts a range of
        segment strings before the specified
        position.
        Escapes in the strings are preserved,
        and reserved characters in the string
        are percent-escaped in the result.

        <br>
        All iterators that are equal to
        `before` or come after are invalidated.

        @note
        The strings referenced by the segments
        must not come from the underlying url,
        or else the behavior is undefined.

        @par Example
        @code
        @endcode

        @par Mandates
        @code
        std::is_convertible< std::iterator_traits< FwdIt >::reference_type, pct_string_view >::value == true
        @endcode

        @par Complexity
        Linear in `this->url().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @throw system_error
        The range contains an invalid percent-encoding.

        @return An iterator to the first
        element inserted, or `before` if
        `init.empty()`.

        @param before An iterator before which
        the element will be inserted. This may
        be equal to `end()`.

        @param init The list of segments
        to insert.
    */
    iterator
    insert(
        iterator before,
        std::initializer_list<
            pct_string_view> init);

    /** Insert segments

        This function inserts the strings in
        an <em>initializer-list</em> as segments
        before the specified position.
        Escapes in the strings are preserved,
        and reserved characters in the string
        are percent-escaped in the result.

        <br>
        All iterators that are equal to
        `before` or come after are invalidated.

        @note
        The strings referenced by the segments
        must not come from the underlying url,
        or else the behavior is undefined.

        @par Example
        @code
        @endcode

        @par Complexity
        Linear in `this->url().size`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exceptions thrown on invalid input.

        @throw system_error
        The range contains an invalid percent-encoding.

        @return An iterator to the first
        element inserted, or `before` if
        `init.size() == 0`.

        @param before An iterator before which
        the element will be inserted. This may
        be equal to `end()`.

        @param first, last The range of segments to insert.
    */
    template<class FwdIt>
#ifdef BOOST_URL_DOCS
    iterator
#else
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            pct_string_view>::value,
        iterator>::type
#endif
    insert(
        iterator before,
        FwdIt first,
        FwdIt last);

    //--------------------------------------------

    /** Erase segments

        This function removes a segment from
        the container.

        <br>
        All iterators that are equal to
        `pos` or come after are invalidated.

        @par Example
        @code
        @endcode

        @par Complexity
        Linear in `this->url().size()`.

        @par Exception Safety
        Throws nothing.

        @return An iterator to one past
        the removed segment.

        @param pos An iterator to the element.
    */
    iterator
    erase(
        iterator pos) noexcept;

    /** Erase segments

        This function removes a range of segments
        from the container.

        <br>
        All iterators that are equal to
        `first` or come after are invalidated.

        @par Example
        @code
        @endcode

        @par Complexity
        Linear in `this->url().size()`.

        @par Exception Safety
        Throws nothing.

        @return An iterator to one past
        the removed range.

        @param first, last The range of
        params to erase.
    */
    BOOST_URL_DECL
    iterator
    erase(
        iterator first,
        iterator last) noexcept;

    //--------------------------------------------

    iterator
    replace(
        iterator pos,
        pct_string_view s);

    iterator
    replace(
        iterator from,
        iterator to,
        pct_string_view s);

    iterator
    replace(
        iterator from,
        iterator to,
        std::initializer_list<
            pct_string_view> init);

    template<class FwdIt>
#ifdef BOOST_URL_DOCS
    iterator
#else
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            pct_string_view>::value,
        iterator>::type
#endif
    replace(
        iterator from,
        iterator to,
        FwdIt first,
        FwdIt last);

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

        u.segments_encoded_ref().push_back( "file.txt" );

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
        pct_string_view s);

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

        u.segments_encoded_ref().pop_back();

        assert( u.encoded_path() == "/path/to" );
        @endcode

        @par Exception Safety
        Throws nothing.
    */
    void
    pop_back() noexcept;

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
};

} // urls
} // boost

// This is in <boost/url/url_base.hpp>
//
// #include <boost/url/impl/segments_encoded_ref.hpp>

#endif
