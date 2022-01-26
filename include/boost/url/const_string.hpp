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
    struct result;

    base* p_;

    BOOST_URL_DECL
    explicit
    const_string(
        result const& r) noexcept;

public:
    class factory;

    /** Destructor

        Releases ownership of the string buffer.
        If this is the last owner, the storage
        is deallocated.
    */
    BOOST_URL_DECL
    ~const_string();

    /** Constructor

        Default-constructed objects represent
        the empty string.
    */
    BOOST_URL_DECL
    const_string() noexcept;

    /** Constructor

        Copies do not allocate. The newly
        constructed string acquires shared
        ownership of the underlying character
        buffer.

        @par Exception Safety
        Throws nothing.
    */
    BOOST_URL_DECL
    const_string(
        const_string const& other) noexcept;

    /** Assignment

        Assignment does not allocate. The newly
        assigned string acquires shared
        ownership of the underlying character
        buffer.
    */
    BOOST_URL_DECL
    const_string&
    operator=(
        const_string const& other) noexcept;

    /** Constructor

        Constructs a string with the specified
        size. The function object `init` is
        used to initialize the contents of
        the string, and will be invoked with
        this equivalent signature:

        @code
        void (std::size_t size, char* dest );
        @endcode

        @param size The size of the resulting string

        @param a The allocator to use.

        @param init The function object to invoke.
    */
    template<
        class Allocator,
        class InitFn>
    const_string(
        std::size_t size,
        Allocator const& a,
        InitFn const& init);

    /** Constructor

        Constructs a copy of the string `s`.
        If memory allocation is required, the
        specified allocator `a` will be used.

        @param s The string to copy

        @param a The allocator to use.
    */
    template<class Allocator>
    const_string(
        string_view s,
        Allocator const& a);
};

//------------------------------------------------

/** A producer for constant strings used a specified allocator.

    Objects of this type are used to produce
    constant strings which all use the specified
    allocator.
*/
class const_string::factory
{
    friend class const_string;

    class base;

    base* p_;

    template<class T>
    class impl;

public:
    /** Destructor
    */
    BOOST_URL_DECL
    ~factory();

    /** Constructor

        @par Exception Safety
        Throws nothing.
    */
    BOOST_URL_DECL
    factory(
        factory const& other) noexcept;

    /** Assignment

        @par Exception Safety
        Throws nothing.
    */
    BOOST_URL_DECL
    factory&
    operator=(
        factory const& other) noexcept;

    /** Constructor

        Default constructed factories use
        the standard allocator
        `std::allocator<char>`.

        @par Exception Safety
        Throws nothing.
    */
    BOOST_URL_DECL
    factory() noexcept;

    /** Constructor

        All factories constructed with this
        function will use the standard allocator
        `std::allocator<char>`.
    */
    template<class T>
    explicit
    factory(std::allocator<T> const&)
        : factory()
    {
    }

    /** Constructor

        Creates a factory using the specified
        allocator.
    */
    template<class Allocator>
    explicit
    factory(Allocator const& alloc);

    /** Return a newly constructed string

        Constructs a string with the specified
        size. The function object `init` is
        used to initialize the contents of
        the string, and will be invoked with
        this equivalent signature:

        @code
        void (std::size_t size, char* dest );
        @endcode

        @param size The size of the resulting string

        @param init The function object to invoke.
    */
    template<class InitFn>
    const_string
    operator()(
        std::size_t n,
        InitFn const& init) const;

    /** Return a newly constructed copy of a string

        Constructs a string with the specified
        size. The function object `init` is
        used to initialize the contents of
        the string, and will be invoked with
        this equivalent signature:

        @code
        void (std::size_t size, char* dest );
        @endcode

        @param s The string to copy
    */
    BOOST_URL_DECL
    const_string
    operator()(string_view s) const;
};

} // urls
} // boost

#include <boost/url/impl/const_string.hpp>

#endif
