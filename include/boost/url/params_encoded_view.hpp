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
#include <boost/url/ignore_case.hpp>
#include <boost/url/param.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <initializer_list>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_base;
#endif

//------------------------------------------------

/** A view representing query parameters in a URL

    Objects of this type are used to interpret
    the query parameters as a bidirectional view
    of key value pairs.

    The view does not retain ownership of the
    elements and instead references the original
    url. The caller is responsible for ensuring
    that the lifetime of the referenced url
    extends until it is no longer referenced.

    The view is modifiable; calling non-const
    members will cause changes to the referenced
    url.

    @par Example
    @code
    url u( "?first=John&last=Doe" );

    params_encoded_view p = u.encoded_encoded_params();
    @endcode

    Strings produced when elements are returned
    have type @ref param_view and represent
    encoded strings. Member functions which
    accept strings expect encoded strings and
    throw exceptions on invalid inputs.

    @par Iterator Invalidation
    @li @ref append : Only `end()`.
    @li @ref assign, @ref clear,
        `operator=` : All elements.
    @li @ref erase : Erased elements and all
        elements after (including `end()`).
    @li @ref insert : All elements at or after
        the insertion point (including `end()`).
    @li @ref replace, @ref set : Modified
        elements and all elements
        after (including `end()`).
*/
class params_encoded_view
    : private detail::parts_base
{
    url_base* u_ = nullptr;

    friend class url_base;

    explicit
    params_encoded_view(
        url_base& u) noexcept
        : u_(&u)
    {
    }

public:
    /** The iterator type

        Iterators returned by the container
        are bidirectional and return constant
        values of type @ref param_view
        when dereferenced.
    */
#ifdef BOOST_URL_DOCS
    using iterator = __see_below__;
#else
    class iterator;
#endif

    /// @copydoc iterator
    using const_iterator = iterator;

    /** The value type

        Values of this type represent parameters
        whose strings retain unique ownership
        by making a copy.

        @par Example
        @code
        url u( "?first=John&last=Doe" );

        params_encoded_view::value_type p( *u.encoded_encoded_params().find( "first" ) );
        @endcode
    */
    using value_type = param;

    /** The reference type

        This is the type of value returned when
        iterators of the view are dereferenced.
    */
    using reference = param_view;

    /// @copydoc reference
    using const_reference = reference;

    /** The unsigned integer type
    */
    using size_type = std::size_t;

    /** The signed integer type
    */
    using difference_type = std::ptrdiff_t;

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
    params_encoded_view(
        params_encoded_view const& other) = default;

    /** Assignment

        After assignment, both views will
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
    params_encoded_view&
    operator=(params_encoded_view const& other) & = default;

    /** Assignment

        After assignment, the previous contents
        of the query parameters are replaced by
        the contents of the initializer-list.

        @par Preconditions
        None of character buffers referenced by
        `init` may overlap the character buffer of
        the underlying url, or else the behavior
        is undefined.

        @par Effects
        @code
        this->assign( init );
        @endcode

        @par Complexity
        Linear in `init.size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param init The list of params to assign.
    */
    params_encoded_view&
    operator=(std::initializer_list<
        reference> init);

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
        url u( "?key=value" );

        assert( &u.encoded_segments().url() == &u );
        @endcode

        @par Exception Safety
        @code
        Throws nothing.
        @endcode
    */
    auto
    url() const noexcept ->
        url_base&
    {
        return *u_;
    }

    /** Return true if there are no elements

        When the url has no query, the view is
        always empty. Otherwise, there will be
        at least one element.

        @par Example
        @code
        assert( ! url( "?key=value" ).encoded_encoded_params().empty() );
        @endcode

        @par Effects
        @code
        return ! this->url().has_query();
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    bool
    empty() const noexcept;

    /** Return the number of elements

        When the url has no query, the view is
        always empty. Otherwise, there will be
        at least one element.

        @par Example
        @code
        assert( url( "?key=value").encoded_encoded_params().size() == 1 ) );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    std::size_t
    size() const noexcept;

    /** Return an iterator to the beginning

        @par Complexity
        Linear in the size of the first element.

        @par Exception Safety
        Throws nothing.
    */
    iterator
    begin() const noexcept;

    /** Return an iterator to the end

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    iterator
    end() const noexcept;

    /** Return true if a matching key exists

        @par Example
        @code
        assert( url( "?first=John&last=Doe" ).contains( "first" ) );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Exception thrown when `key` contains
        an invalid percent-encoding.

        @param key The key to match.
        By default, a case-sensitive
        comparison is used.

        @param ic An optional parameter. If
        the value @ref ignore_case is passed
        here, the comparison will be
        case-insensitive.
    */
    bool
    contains(
        string_view key,
        ignore_case_param ic = {}) const;

    /** Return the number of matching keys

        @par Example
        @code
        assert( url( "?first=John&last=Doe" ).count( "first" ) == 1 );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Exception thrown when `key` contains
        an invalid percent-encoding.

        @param key The key to match.
        By default, a case-sensitive
        comparison is used.

        @param ic An optional parameter. If
        the value @ref ignore_case is passed
        here, the comparison will be
        case-insensitive.
    */
    BOOST_URL_DECL
    std::size_t
    count(
        string_view key,
        ignore_case_param ic = {}) const;

    /** Find a matching key

        @par Example
        @code
        url u( "?first=John&last=Doe" );

        assert( u.encoded_encoded_params().find( "First", ignore_case )->value == "John" );
        @endcode

        @par Effects
        @code
        return this->find( this->begin(), key, ic );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Exception thrown when `key` contains
        an invalid percent-encoding.

        @return an iterator to the element

        @param key The key to match.
        By default, a case-sensitive
        comparison is used.

        @param ic An optional parameter. If
        the value @ref ignore_case is passed
        here, the comparison will be
        case-insensitive.
    */
    iterator
    find(
        string_view key,
        ignore_case_param ic = {}) const;

    /** Find a matching key

        This function searches for the key
        starting at `before` and continuing
        until either the key is found, or
        the end of the range is reached in
        which case `end()` is returned.

        @par Example
        @code
        url u( "?First=John&Last=Doe" );

        assert( u.encoded_encoded_params().find( "first" ) != u.encoded_encoded_params().find( "first", ignore_case ) );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Exception thrown when `key` contains
        an invalid percent-encoding.

        @return an iterator to the element

        @param from The position to begin the
            search from. This can be `end()`.

        @param key The key to match.
        By default, a case-sensitive
        comparison is used.

        @param ic An optional parameter. If
        the value @ref ignore_case is passed
        here, the comparison will be
        case-insensitive.
    */
    BOOST_URL_DECL
    iterator
    find(
        iterator from,
        string_view key,
        ignore_case_param ic = {}) const;

    /** Find a matching key

        This function searches for the key
        starting just prior to `before` and
        continuing until either the key is found,
        or the beginning of the range is reached
        in which case `end()` is returned.

        @par Example
        @code
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Exception thrown when `key` contains
        an invalid percent-encoding.

        @return an iterator to the element

        @param before One past the position
        to begin the search from. This can
        be `end()`.

        @param key The key to match.
        By default, a case-sensitive
        comparison is used.

        @param ic An optional parameter. If
        the value @ref ignore_case is passed
        here, the comparison will be
        case-insensitive.
    */
    BOOST_URL_DECL
    iterator
    find_prev(
        iterator before,
        string_view key,
        ignore_case_param ic = {}) const;

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
        this->url().remove_query();
        @endcode

        @par Postconditions
        @code
        this->empty() == true && this->url().has_query() == false
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    void
    clear() noexcept;

    //--------------------------------------------

    /** Assign elements

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
        Exception thrown when params contain
        an invalid percent-encoding.

        @param init The list of params to assign.
    */
    void
    assign(std::initializer_list<
        param_view> init);

    /** Assign elements

        This function replaces the entire
        contents of the view with the params
        in the range.

        <br>
        All iterators are invalidated.

        @note
        The strings referenced by the params
        must not come from the underlying url,
        or else the behavior is undefined.

        @par Mandates
        @code
        std::is_convertible< std::iterator_traits< FwdIt >::value_type, param_view >::value == true
        @endcode

        @par Complexity
        Linear in the size of the range.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exception thrown when params contain
        an invalid percent-encoding.

        @param first, last The range of params
        to assign.
    */
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last);

    //--------------------------------------------

    /** Append elements

        This function appends a param to the view.

        <br>
        The `end()` iterator is invalidated.

        @par Example
        @code
        url u;

        u.encoded_params().append( { "first", "John" } );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exception thrown when param contains
        an invalid percent-encoding.

        @return An iterator to the new element.

        @param p The param to append.
    */
    iterator
    append(
        param_view const& p);

    /** Append elements

        This function appends the params in
        an <em>initializer-list</em> to the view.

        <br>
        The `end()` iterator is invalidated.

        @par Example
        @code
        url u;

        u.encoded_params().append( {{ "first", "John" }, {{ "last", "Doe" }});
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exception thrown when params contain
        an invalid percent-encoding.

        @return An iterator to the first new element.

        @param init The list of params to append.
    */
    iterator
    append(std::initializer_list<
        param_view> init);

    /** Append elements

        This function appends a range of params
        to the view.

        <br>
        The `end()` iterator is invalidated.

        @par Example
        @code
        @endcode

        @par Mandates
        @code
        std::is_convertible< std::iterator_traits< FwdIt >::value_type, param_view >::value == true
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exception thrown when params contain
        an invalid percent-encoding.

        @return An iterator to the first new element.

        @param first, last The range of params
        to append.
    */
    template<class FwdIt>
    iterator
    append(
        FwdIt first, FwdIt last);

    //--------------------------------------------

    /** Insert elements

        This function inserts a param
        before the specified position.

        <br>
        All iterators that are equal to
        `before` or come after are invalidated.

        @par Example
        @code
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exception thrown when param contains
        an invalid percent-encoding.

        @return An iterator to the inserted
        element.

        @param before An iterator before which
        the param will be inserted. This may
        be equal to `end()`.

        @param p The param to insert.
    */
    iterator
    insert(
        iterator before,
        param_view const& p);

    /** Insert elements

        This function inserts the params in
        an <em>initializer-list</em> before
        the specified position.

        <br>
        All iterators that are equal to
        `before` or come after are invalidated.

        @note
        The strings referenced by the params
        must not come from the underlying url,
        or else the behavior is undefined.

        @par Example
        @code
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exception thrown when params contain
        an invalid percent-encoding.

        @return An iterator to the first
        element inserted, or `before` if
        `init.size() == 0`.

        @param before An iterator before which
        the element will be inserted. This may
        be equal to `end()`.

        @param init The list of params to insert.
    */
    iterator
    insert(
        iterator before,
        std::initializer_list<
            param_view> init);

    /** Insert elements

        This function inserts a range of
        params before the specified position.

        <br>
        All iterators that are equal to
        `before` or come after are invalidated.

        @note
        The strings referenced by the params
        must not come from the underlying url,
        or else the behavior is undefined.

        @par Example
        @code
        @endcode

        @par Mandates
        @code
        std::is_convertible< std::iterator_traits< FwdIt >::value_type, param_view >::value == true
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exception thrown when params contain
        an invalid percent-encoding.

        @return An iterator to the first
        element inserted, or `before` if
        `first == last`.

        @param before An iterator before which
        the element will be inserted. This may
        be equal to `end()`.

        @param first, last The range of params
        to insert.
    */
    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last);

    //--------------------------------------------

    /** Erase elements

        This function removes an element from
        the container.

        <br>
        All iterators that are equal to
        `pos` or come after are invalidated.

        @par Example
        @code
        url u( "?first=John&last=Doe" );

        params_encoded_view::iterator it = u.encoded_params().erase( u.encoded_params().begin() );

        assert( u.encoded_query() == "last=Doe" );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Throws nothing.

        @return An iterator to one past
        the removed element.

        @param pos An iterator to the element.
    */
    iterator
    erase(iterator pos) noexcept;

    /** Erase elements

        This function removes a range of elements
        from the container.

        <br>
        All iterators that are equal to
        `first` or come after are invalidated.

        @par Example
        @code
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Throws nothing.

        @return An iterator to one past
        the removed range.

        @param first, last The range of
        elements to erase.
    */
    iterator
    erase(
        iterator first,
        iterator last) noexcept;

    /** Erase elements

        <br>
        All iterators are invalidated.

        @par Example
        @code
        @endcode

        @par Postconditions
        @code
        this->count( key, ic ) == 0
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Exception thrown when key contain
        an invalid percent-encoding.

        @return The number of elements removed
        from the container.

        @param key The key to match.
        By default, a case-sensitive
        comparison is used.

        @param ic An optional parameter. If
        the value @ref ignore_case is passed
        here, the comparison will be
        case-insensitive.
    */
    BOOST_URL_DECL
    std::size_t
    erase(
        string_view key,
        ignore_case_param ic = {});

    //--------------------------------------------

    /** Replace elements

        This function replaces the contents
        of the element at `pos` with the
        specified param.

        <br>
        All iterators that are equal to
        `pos` or come after are invalidated.

        @par Example
        @code
        url u( "?first=John&last=Doe" );

        u.encoded_params().replace( u.encoded_params().begin(), { "title", "Mr" });

        assert( u.encoded_query() == "title=Mr&last=Doe" );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exception thrown when param contains
        an invalid percent-encoding.

        @return An iterator to the element.

        @param pos An iterator to the element.

        @param p The param to assign.
    */
    iterator
    replace(
        iterator pos,
        param_view const& p);

    /** Replace elements

        This function replaces a range of
        elements with the params in an
        <em>initializer-list</em>.

        <br>
        All iterators that are equal to
        `from` or come after are invalidated.

        @note
        The strings referenced by the params
        must not come from the underlying url,
        or else the behavior is undefined.

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exception thrown when params contain
        an invalid percent-encoding.

        @return An iterator to the first
        element inserted, or one past `to` if
        `init.size() == 0`.

        @param from,to The range of elements
        to replace.

        @param init The list of params to assign.
    */
    iterator
    replace(
        iterator from,
        iterator to,
        std::initializer_list<
            param_view> init);

    /** Replace elements

        This function replaces a range of
        elements with a range of params.

        <br>
        All iterators that are equal to
        `from` or come after are invalidated.

        @note
        The strings referenced by the params
        must not come from the underlying url,
        or else the behavior is undefined.

        @par Mandates
        @code
        std::is_convertible< std::iterator_traits< FwdIt >::value_type, param_view >::value == true
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.
        Exception thrown when params contain
        an invalid percent-encoding.

        @return An iterator to the first
        element inserted, or one past `to` if
        `first == last`.

        @param from,to The range of elements to
        replace.

        @param first, last The range of params
        to assign.
    */
    template<class FwdIt>
    iterator
    replace(
        iterator from,
        iterator to,
        FwdIt first,
        FwdIt last);

    //--------------------------------------------

    /** Remove the value on an element

        This function removes the value of
        an element at the specified position.
        After the call returns, `has_value`
        for the element will be false.

        <br>
        All iterators that are equal to
        `pos` or come after are invalidated.

        @par Example
        @code
        url u( "?first=John&last=Doe" );

        u.encoded_params().reset( u.encoded_params().begin() );

        assert( u.encoded_query() == "first&last=Doe" );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Throws nothing.

        @return An iterator to the element.

        @param pos An iterator to the element.
    */
    BOOST_URL_DECL
    iterator
    reset(
        iterator pos) noexcept;

    /** Set a value

        This function replaces the value of an
        element at the specified position.

        <br>
        All iterators that are equal to
        `pos` or come after are invalidated.

        @par Example
        @code
        url u( "?id=42&id=69" );

        u.encoded_params().set( u.encoded_params().begin(), "none" );

        assert( u.encoded_query() == "id=none&id=69" );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @return An iterator to the element.

        @param pos An iterator to the element.

        @param value The value to assign. The
        empty string still counts as a value.
        That is, `has_value` for the element
        will be true.
    */
    BOOST_URL_DECL
    iterator
    set(
        iterator pos,
        string_view value);

    /** Set a value

        This function performs one of two
        actions depending on the value of
        `this->contains( key, ic )`.

        @li If key is contained in the view
        then one of the matching elements has
        its value changed to the specified value.
        The remaining elements with a matching
        key are erased. Otherwise,

        @li If `key` is not contained in the
        view, then the function apppends the
        param `{ key, value }`.

        <br>
        All iterators are invalidated.

        @par Example
        @code
        url u( "?id=42&id=69" );

        u.encoded_params().set( "id", "none" );

        assert( u.encoded_params().count( "id" ) == 1 );
        @endcode

        @par Postconditions
        @code
        this->count( key, ic ) == 1 && this->find( key, ic )->value == value
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @return An iterator to the appended
        or modified element.

        @param key The key to match.
        By default, a case-sensitive
        comparison is used.

        @param value The value to assign. The
        empty string still counts as a value.
        That is, `has_value` for the element
        will be true.

        @param ic An optional parameter. If
        the value @ref ignore_case is passed
        here, the comparison will be
        case-insensitive.
    */
    BOOST_URL_DECL
    iterator
    set(
        string_view key,
        string_view value,
        ignore_case_param ic = {});

    //--------------------------------------------

private:
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last,
        std::forward_iterator_tag);

    // Doxygen cannot render ` = delete`
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last,
           std::input_iterator_tag) = delete;

    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last,
        std::forward_iterator_tag);

    // Doxygen cannot render ` = delete`
    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last,
        std::input_iterator_tag) = delete;
};

} // urls
} // boost

// This is in <boost/url/url_base.hpp>
//
// #include <boost/url/impl/params_encoded_view.hpp>

#endif
