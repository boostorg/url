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

    BOOST_URL_DECL
    ~const_string();

    BOOST_URL_DECL
    const_string() noexcept;

    BOOST_URL_DECL
    const_string(
        const_string const& other) noexcept;

    BOOST_URL_DECL
    const_string&
    operator=(
        const_string const& other) noexcept;

    template<
        class Allocator,
        class InitFn>
    const_string(
        std::size_t size,
        Allocator const& a,
        InitFn const& init);

    template<class Allocator>
    const_string(
        string_view s,
        Allocator const& a);
};

//------------------------------------------------

class const_string::factory
{
    friend class const_string;

    class base;

    base* p_;

    template<class T>
    class impl;

public:
    BOOST_URL_DECL
    ~factory();

    BOOST_URL_DECL
    factory(
        factory const& other) noexcept;

    BOOST_URL_DECL
    factory&
    operator=(
        factory const& other) noexcept;

    BOOST_URL_DECL
    factory() noexcept;

    template<class T>
    explicit
    factory(std::allocator<char> const&)
        : factory()
    {
    }

    template<class Allocator>
    explicit
    factory(Allocator const& alloc);

    template<class InitFn>
    const_string
    operator()(
        std::size_t n,
        InitFn const& init) const;

    BOOST_URL_DECL
    const_string
    operator()(string_view s) const;
};

} // urls
} // boost

#include <boost/url/impl/const_string.hpp>

#endif
