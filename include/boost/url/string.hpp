//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_STRING_HPP
#define BOOST_URL_STRING_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <boost/type_traits/make_void.hpp>
#include <memory>
#include <string>
#include <type_traits>

namespace boost {
namespace urls {

/** A const string with shared ownership

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
class string_value : public string_view
{
    /** Type for string_value reference counter and custom destructor

        This struct stores the number of references to this read-only string.
        As the string is read-only, we can create a copy by simply updating
        the reference counter.

        As the underlying string is constructed with a custom allocator, the
        struct also includes a custom destructor to deallocate the
        string whenever the reference counter reaches zero.

     */
    struct base;

    /** Instance of reference counter for @ref string_value instances pointing to this string

        Pointer to the reference counter for all string_value instances
        pointing to the same string.

     */
    base* p_ = nullptr;

    /** Allocate a string copy and return its initial reference counter

        This function allocates a string of input size `n` starting at
        output address `dest` with the allocator `a`.

        It returns its initial reference counter and the string address as
        `dest`.

        @note As an optimization, the initial reference counter is `nullptr` for
        empty strings. For other strings, the reference counter starts as `1`
        and contains a pointer to a `deallocate` function, which might be used
        later on by @ref ~string_value to deallocate this string value.

        @note If `n == 0`, `dest` is set to `nullptr` to logically represent
        an empty string. This is unlike string_view, which differentiates
        empty strings of size 0 and no string pointing to nullptr.

        @tparam Allocator Allocator used to create allocate the new string

        @return Pointer to the initial reference counter for this new string

        @param n Size of the new string

        @param a Allocator used to allocate the new string

        @param dest Address where the string should be constructed


     */
    template<class Allocator>
    base*
    construct(
        std::size_t n,
        Allocator const& a,
        char*& dest);

public:
    /** The default allocator type to construct read-only instances of @ref string_value

        An allocator of this class can be instantiated to create
        multiple read-only string values.

        @note This custom allocator is not an instance of the C++ Allocator named requirement

        @par References
        @li <a href="https://en.cppreference.com/w/cpp/named_req/Allocator">Allocator (cppreference)</a>

     */
    class allocator;

    /** Destructor

        Upon destruction, ownership of the shared string is released. If
        there are no more owners, the resources for the string are released.

     */
    inline
    ~string_value();

    /** Default constructor

        This function creates the default base @ref string_view, which points to
        no string / nullptr.

        @note As an optimization, the reference counter is `nullptr`.

     */
    string_value() = default;

    /** Construct an initial string_value

        This function creates a string of the specified size at the
        destination with the a custom allocator.

        @note Although this function returns sets the destination pointer
        to the new string address, this string_value owns the buffer.
        The buffer is destroyed after the last string_value points to it.

        @tparam Allocator Allocator type

        @param n Number of chars in the string `dest`

        @param a Allocator used to create a copy of `dest`

        @param dest Destination

     */
    template<class Allocator>
    string_value(
        std::size_t n,
        Allocator const& a,
        char*& dest);

    /** Construct a copy of a @ref string_value

        This function constructs a copy of a @ref string_value with the
        specified allocator.

        This function allocates a @ref string_value the specified allocator
        and copies its contents to this new string.

        @tparam Allocator Allocator type

        @param s string_view whose contents will be copied to this string_value

        @param a allocator used to create this string_value

     */
    template< class Allocator =
        std::allocator<char> >
    explicit
    string_value(
        string_view s,
        Allocator const& a = {});

    /** Copy constructor

        This function constructs a copy of a string_value. As string_value
        is read-only, this function can only copy and increment the string
        reference counter.

        @param other The other string_value

     */
    inline
    string_value(
        string_value const& other) noexcept;

    /** Copy assignment

        This function copy assigns a @ref string_value.

        @note As @ref string_value is read-only, this function can only copy and increment
        the string reference counter, which allows this operation to execute in constant time.

        @param other The other @ref string_value

     */
    inline
    string_value&
    operator=(string_value const& other) & noexcept;
};


} // urls
} // boost


#include <boost/url/impl/string.hpp>

#endif
