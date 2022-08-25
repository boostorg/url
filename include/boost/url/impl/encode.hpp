//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_ENCODE_HPP
#define BOOST_URL_IMPL_ENCODE_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/encode_opts.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/hexdig_chars.hpp>
#include <boost/url/grammar/type_traits.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

namespace detail {

template <class Iter, class CharSet>
std::size_t
encoded_size_impl(
    Iter it,
    Iter const end,
    CharSet const& allowed,
    encode_opts const& opt = {}) noexcept
{
    // CharSet must satisfy is_charset
    BOOST_STATIC_ASSERT(
        grammar::is_charset<CharSet>::value);

    std::size_t n = 0;
    if (!opt.space_to_plus)
    {
        while (it != end)
        {
            if (allowed(*it))
                ++n;
            else
                n += 3;
            ++it;
        }
        return n;
    }
    // If you are converting space
    // to plus, then space should
    // be in the list of reserved
    // characters!
    BOOST_ASSERT(!allowed(' '));
    while (it != end)
    {
        if (*it == ' ')
            ++n;
        else if (allowed(*it))
            ++n;
        else
            n += 3;
        ++it;
    }
    return n;
}
}

template<class CharSet>
std::size_t
encoded_size(
    string_view s,
    CharSet const& allowed,
    encode_opts const& opt) noexcept
{
    return detail::encoded_size_impl(
        s.data(),
        s.data() + s.size(),
        allowed,
        opt);
}

//------------------------------------------------

namespace detail {

template<class Iter, class CharSet>
std::size_t
encode_impl(
    char* dest,
    char const* const end,
    Iter p,
    Iter last,
    CharSet const& allowed,
    encode_opts const& opt = {})
{
    // CharSet must satisfy is_charset
    BOOST_STATIC_ASSERT(
        grammar::is_charset<CharSet>::value);

    // Can't have % in charset
    BOOST_ASSERT(! allowed('%'));

    static constexpr char hex[] =
        "0123456789abcdef";
    auto const dest0 = dest;
    auto const end3 = end - 3;
    if(! opt.space_to_plus)
    {
        while(p != last)
        {
            if(allowed(*p))
            {
                if(dest == end)
                    return dest - dest0;
                *dest++ = *p++;
                continue;
            }
            if(dest > end3)
                return dest - dest0;
            *dest++ = '%';
            auto const u = static_cast<
                unsigned char>(*p);
            *dest++ = hex[u>>4];
            *dest++ = hex[u&0xf];
            ++p;
        }
        return dest - dest0;
    }
    // If you are converting space
    // to plus, then space should
    // be in the list of reserved
    // characters!
    BOOST_ASSERT(! allowed(' '));
    while(p != last)
    {
        if(allowed(*p))
        {
            if(dest == end)
                return dest - dest0;
            *dest++ = *p++;
            continue;
        }
        if(*p == ' ')
        {
            if(dest == end)
                return dest - dest0;
            *dest++ = '+';
            ++p;
            continue;
        }
        if(dest > end3)
            return dest - dest0;
        *dest++ = '%';
        auto const u = static_cast<
            unsigned char>(*p);
        *dest++ = hex[u>>4];
        *dest++ = hex[u&0xf];
        ++p;
    }
    return dest - dest0;
}

} // detail

template<class CharSet>
std::size_t
encode(
    char* dest,
    char const* const end,
    string_view s,
    CharSet const& allowed,
    encode_opts const& opt)
{
    return detail::encode_impl(
        dest,
        end,
        s.data(),
        s.data() + s.size(),
        allowed,
        opt);
}

//------------------------------------------------

template<
    class CharSet,
    class Allocator>
std::basic_string<char,
    std::char_traits<char>,
        Allocator>
encode_to_string(
    string_view s,
    CharSet const& allowed,
    encode_opts const& opt,
    Allocator const& a)
{
    // CharSet must satisfy is_charset
    BOOST_STATIC_ASSERT(
        grammar::is_charset<CharSet>::value);

    std::basic_string<
        char,
        std::char_traits<char>,
        Allocator> r(a);
    if(s.empty())
        return r;
    auto const n =
        encoded_size(s, allowed, opt);
    r.resize(n);
    char* dest = &r[0];
    char const* end = dest + n;
    auto const n1 = encode(
        dest, end, s, allowed, opt);
    BOOST_ASSERT(n1 == n);
    (void)n1;
    return r;
}

} // urls
} // boost

#endif
