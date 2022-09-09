//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_PARAMS_BASE_HPP
#define BOOST_URL_PARAMS_BASE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/ignore_case.hpp>
#include <boost/url/param.hpp>
#include <boost/url/detail/params_iter_impl.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <boost/url/detail/url_impl.hpp>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view_base;
#endif

//------------------------------------------------

/** Common functionality for containers

    This base class is used by the library
    to provide common member functions for
    containers. This cannot be instantiated
    directly; Instead, use one of the
    containers or functions:

    @par Containers
    @li @ref params_view
    @li @ref params_const_view
    @li @ref params_encoded_view
    @li @ref params_const_encoded_view
*/
class params_base
    : private detail::parts_base
{
    friend class params_view;
    friend class params_const_view;

    detail::url_impl const* impl_ = nullptr;

    params_base(
        detail::url_impl const& impl) noexcept
        : impl_(&impl)
    {
    }

public:
    /** A Bidirectional iterator to a query parameter

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
        params_const_view::value_type qp( *url_view( "?first=John&last=Doe" ).params().find( "first" ) );
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

    /** Return the referenced character buffer.

        This function returns the character
        buffer referenced by the view.
        The returned string may contain
        percent escapes.

        @par Example
        @code
        assert( url_view( "?first=John&last=Doe" ).params().buffer() == "?first=John&last=Doe" );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    BOOST_URL_DECL
    pct_string_view
    buffer() const noexcept;

    /** Return true if there are no params

        @par Example
        @code
        assert( ! url_view( "?key=value" ).params().empty() );
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    bool
    empty() const noexcept;

    /** Return the number of params

        @par Example
        @code
        assert( url_view( "?key=value").params().size() == 1 );
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
        Linear in the size of the first param.

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
        assert( url_view( "?first=John&last=Doe" ).params().contains( "first" ) );
        @endcode

        @par Complexity
        Linear in `this->buffer().size()`.

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
        assert( url_view( "?first=John&last=Doe" ).params().count( "first" ) == 1 );
        @endcode

        @par Complexity
        Linear in `this->buffer().size()`.

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

        The search starts from the first param
        and proceeds forward until either the
        key is found or the end of the range is
        reached, in which case `end()` is
        returned.

        @par Example
        @code
        assert( url_view( "?first=John&last=Doe" ).params().find( "First", ignore_case )->value == "John" );
        @endcode

        @par Effects
        @code
        return this->find( this->begin(), key, ic );
        @endcode

        @par Complexity
        Linear in `this->buffer().size()`.

        @return an iterator to the param

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
        url_view u( "?First=John&Last=Doe" );

        assert( u.params().find( "first" ) != u.params().find( "first", ignore_case ) );
        @endcode

        @par Complexity
        Linear in `this->buffer().size()`.

        @return an iterator to the param

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

        The search starts from the last param
        and proceeds backwards until either the
        key is found or the beginning of the
        range is reached, in which case `end()`
        is returned.

        @par Example
        @code
        assert( url_view( "?first=John&last=Doe" ).params().find_last( "last" )->value == "Doe" );
        @endcode

        @par Complexity
        Linear in `this->buffer().size()`.

        @return an iterator to the param

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
        url_view u( "?First=John&Last=Doe" );

        assert( u.params().find_last( "last" ) != u.params().find_last( "last", ignore_case ) );
        @endcode

        @par Complexity
        Linear in `this->buffer().size()`.

        @return an iterator to the param

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
