//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_BASE_HPP
#define BOOST_URL_PARAMS_BASE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/ignore_case.hpp>
#include <boost/url/param.hpp>
#include <boost/url/detail/params_iter_impl.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view_base;
#endif

//------------------------------------------------

/** Provides common functionality for param views
*/
class params_base
    : private detail::parts_base
{
    friend class params_view;
    friend class params_const_view;

    url_view_base const* cu_ = nullptr;

    params_base(
        url_view_base const& u) noexcept
        : cu_(&u)
    {
    }

public:
    /** A bidirectional InputIterator to a query parameter

        Objects of this type allow iteration
        through the parameters in the query.
        Any percent-escapes in returned strings
        are decoded first.
        The values returned are read-only;
        changes to parameters must be made
        through the container instead, if the
        container supports modification.

        <br>

        The strings produced when iterators are
        dereferenced belong to the iterator and
        become invalidated when that particular
        iterator is incremented, decremented,
        or destroyed.
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
        whose strings retain unique ownership by
        making a copy.

        @par Example
        @code
        url u( "?first=John&last=Doe" );

        params_const_view::value_type p( *u.params().find( "first" ) );
        @endcode

        @see
            @ref param.
    */
    using value_type = param;

    /** The reference type

        This is the type of value returned when
        iterators of the view are dereferenced.

        @see
            @ref param_view.
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

        assert( &u.segments().url() == &u );
        @endcode

        @par Exception Safety
        @code
        Throws nothing.
        @endcode
    */
    url_view_base const&
    url() const noexcept
    {
        return *cu_;
    }

    /** Return true if there are no elements

        When the url has no query, the view is
        always empty. Otherwise, there will be
        at least one element.

        @par Example
        @code
        assert( ! url_view( "?key=value" ).params().empty() );
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
        assert( url_view( "?key=value").params().size() == 1 ) );
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

    //--------------------------------------------

    /** Return true if a matching key exists

        This function examines the
        parameters in the container to
        find a match for the specified key.
        The comparison is performed as if all
        escaped characters were decoded first.

        @par Example
        @code
        assert( url( "?first=John&last=Doe" ).contains( "first" ) );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Throws nothing.

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
        ignore_case_param ic = {}) const noexcept;

    /** Return the number of matching keys

        This function examines the
        parameters in the container to
        find the number of matches for
        the specified key.
        The comparison is performed as if all
        escaped characters were decoded first.

        @par Example
        @code
        assert( url( "?first=John&last=Doe" ).count( "first" ) == 1 );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

        @par Exception Safety
        Throws nothing.

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
        ignore_case_param ic = {}) const noexcept;

    /** Find a matching key

        This function examines the
        parameters in the container to
        find a match for the specified key.
        The comparison is performed as if all
        escaped characters were decoded first.

        <br>

        The search starts from the first element
        and proceeds forward until either the
        key is found or the end of the range is
        reached, in which case `end()` is
        returned.

        @par Example
        @code
        url u( "?first=John&last=Doe" );

        assert( u.params().find( "First", ignore_case )->value == "John" );
        @endcode

        @par Effects
        @code
        return this->find( this->begin(), key, ic );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

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
        ignore_case_param ic = {}) const noexcept;

    /** Find a matching key

        This function examines the
        parameters in the container to
        find a match for the specified key.
        The comparison is performed as if all
        escaped characters were decoded first.

        <br>

        The search starts at `from`
        and proceeds forward until either the
        key is found or the end of the range is
        reached, in which case `end()` is
        returned.

        @par Example
        @code
        url u( "?First=John&Last=Doe" );

        assert( u.params().find( "first" ) != u.params().find( "first", ignore_case ) );
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

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
    iterator
    find(
        iterator from,
        string_view key,
        ignore_case_param ic = {}) const noexcept;

    /** Find a matching key
    
        This function examines the
        parameters in the container to
        find a match for the specified key.
        The comparison is performed as if all
        escaped characters were decoded first.

        <br>

        The search starts from the last element
        and proceeds backwards until either the
        key is found or the beginning of the
        range is reached, in which case `end()`
        is returned.

        @par Example
        @code
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

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
    find_last(
        string_view key,
        ignore_case_param ic = {}) const noexcept;

    /** Find a matching key
    
        This function examines the
        parameters in the container to
        find a match for the specified key.
        The comparison is performed as if all
        escaped characters were decoded first.

        <br>

        The search starts prior to `before`
        and proceeds backwards until either the
        key is found or the beginning of the
        range is reached, in which case `end()`
        is returned.

        @par Example
        @code
        @endcode

        @par Complexity
        Linear in `this->url().encoded_query().size()`.

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
    iterator
    find_last(
        iterator before,
        string_view key,
        ignore_case_param ic = {}) const noexcept;

private:
    BOOST_URL_DECL
    detail::params_iter_impl
    find_impl(
        detail::params_iter_impl,
        string_view,
        ignore_case_param) const noexcept;

    BOOST_URL_DECL
    detail::params_iter_impl
    find_last_impl(
        detail::params_iter_impl,
        string_view,
        ignore_case_param) const noexcept;
};

} // urls
} // boost

// This is in <boost/url/url_view_base.hpp>
//
// #include <boost/url/impl/params_base.hpp>

#endif
