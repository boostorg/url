//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_CONST_STRING_HPP
#define BOOST_URL_CONST_STRING_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <memory>
#include <type_traits>

namespace boost {
namespace urls {

/** A constant string with shared ownership

    Objects of this type represent read-only
    strings with ownership of the character
    buffer.

    They are reference counted to allow cheap copies.

    The type is derived from `string_view`,
    which provides compatibility with strings
    in terms of comparisons and conversions.

    However, care must be exercised; undefined
    behavior results if the string_view
    portion of the object is modified
    directly, for example by calling
    `remove_suffix` or `operator=`.

    Slicing, however, is supported, as
    copies of the `string_view` portion
    of the object are valid and remain
    valid for the lifetime of the original
    object.
*/
class const_string : public string_view
{
    struct base;

    base* p_ = nullptr;

    template<class Allocator>
    base*
    construct(
        std::size_t n,
        Allocator const& a,
        char*& dest);

public:
#ifdef BOOST_URL_DOCS
    using allocator = __see_below__;
#else
    class allocator;
#endif

    /** Destructor

        Upon destruction, ownership of the shared string is released. If
        there are no more owners, the resources for the string are released.

     */
    inline
    ~const_string();

    /** Constructor

        Default-constructed strings are empty

     */
    const_string() = default;

    /** Constructor

        This function creates a const_string of the specified size at the
        destination with the a custom allocator.

        This function sets the destination pointer to the new string address,
        where the user can fill its contents while the const_string still
        owns the buffer.

        The buffer is destroyed after the last const_string that refers to it
        is destroyed.

        @tparam Allocator Allocator type

        @param n Number of chars in the string `dest`

        @param a Allocator to create a string in `dest`

        @param dest Destination

     */
    template<class Allocator>
    const_string(
        std::size_t n,
        Allocator const& a,
        char*& dest);

    /** Constructor

        This function constructs a copy of a @ref const_string with the
        specified allocator.

        This function allocates a @ref const_string the specified allocator
        and copies its contents to this new string.

        @tparam Allocator Allocator type

        @param s The sting to copy

        @param a allocator used to create this const_string

     */
    template< class Allocator =
        std::allocator<char> >
    explicit
    const_string(
        string_view s,
        Allocator const& a = {});

    /** Copy constructor

        This function constructs a copy of a const_string. As const_string
        is read-only, this function can only copy and increment the string
        reference counter.

        @param other The string to copy

     */
    inline
    const_string(
        const_string const& other) noexcept;

    /** Copy assignment

        This function copy assigns a @ref const_string.

        Objects of this type are cheap to copy.

        @param other The string to copy

     */
    inline
    const_string&
    operator=(const_string const& other) & noexcept;
};


} // urls
} // boost


#include <boost/url/impl/const_string.hpp>

#endif
