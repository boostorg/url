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

/** A read-only, reference counted string

    Objects of this type represent read-only
    strings with ownership of the character
    buffer. They are reference counted which
    makes copies cheap. The type is derived
    from `string_view` which provides
    compatibility with strings in terms of
    comparisons and conversions. However,
    care must be exercised; undefined
    behavior results if the string_view
    portion of the object is modified
    directly, for example by calling
    `remove_suffix` or `operator=`.

    Slicing however, is supported, as
    copies of the `string_view` portion
    of the object are valid and remain
    valid for the lifetime of the original
    object.
*/
class string_value : public string_view
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
    class allocator;

    inline
    ~string_value();

    string_value() = default;

    template<class Allocator>
    string_value(
        std::size_t n,
        Allocator const& a,
        char*& dest);

    template< class Allocator =
        std::allocator<char> >
    explicit
    string_value(
        string_view s,
        Allocator const& a = {});

    inline
    string_value(
        string_value const& other) noexcept;

    inline
    string_value&
    operator=(string_value const& other) & noexcept;
};

} // urls
} // boost

#include <boost/url/impl/string.hpp>

#endif
