//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_IMPL_ENCODE_HPP
#define BOOST_URL_IMPL_ENCODE_HPP

#include <boost/url/detail/encode.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/encode_opts.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/hexdig_chars.hpp>
#include <boost/url/grammar/type_traits.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

template<class CharSet>
std::size_t
encoded_size(
    string_view s,
    CharSet const& unreserved,
    encode_opts const& opt) noexcept
{
/*  If you get a compile error here, it
    means that the value you passed does
    not meet the requirements stated in
    the documentation.
*/
    static_assert(
        grammar::is_charset<CharSet>::value,
        "Type requirements not met");

    std::size_t n = 0;
    auto it = s.data();
    auto const last = it + s.size();

    if(! opt.space_to_plus)
    {
        while(it != last)
        {
            if(unreserved(*it))
                n += 1;
            else
                n += 3;
            ++it;
        }
    }
    else
    {
        // VFALCO space is usually reserved,
        // and we depend on this for an
        // optimization. if this assert
        // goes off we can split the loop
        // below into two versions.
        BOOST_ASSERT(! unreserved(' '));

        while(it != last)
        {
            auto c = *it;
            if( unreserved(c) ||
                c == ' ')
                ++n;
            else
                n += 3;
            ++it;
        }
    }
    return n;
}

//------------------------------------------------

template<class CharSet>
std::size_t
encode(
    char* dest,
    std::size_t size,
    string_view s,
    CharSet const& unreserved,
    encode_opts const& opt)
{
/*  If you get a compile error here, it
    means that the value you passed does
    not meet the requirements stated in
    the documentation.
*/
    static_assert(
        grammar::is_charset<CharSet>::value,
        "Type requirements not met");

    // '%' must be reserved
    BOOST_ASSERT(! unreserved('%'));

    char const* const hex =
        detail::hexdigs[opt.lower_case];
    auto const encode = [hex](
        char*& dest,
        unsigned char c) noexcept
    {
        *dest++ = '%';
        *dest++ = hex[c>>4];
        *dest++ = hex[c&0xf];
    };

    // VFALCO we could in theory optimize these
    // loops with another loop up to safe_last,
    // where safe_last = it + (last/it)/3,
    // and increment safe_last by 2 every
    // time we output an unescaped character
    auto it = s.data();
    auto const end = dest + size;
    auto const last = it + s.size();
    auto const dest0 = dest;
    auto const end3 = end - 3;
    if(! opt.space_to_plus)
    {
        while(it != last)
        {
            if(unreserved(*it))
            {
                if(dest == end)
                    return dest - dest0;
                *dest++ = *it++;
                continue;
            }
            if(dest > end3)
                return dest - dest0;
            encode(dest, *it++);
        }
        return dest - dest0;
    }
    else
    {
        // VFALCO space is usually reserved,
        // and we depend on this for an
        // optimization. if this assert
        // goes off we can split the loop
        // below into two versions.
        BOOST_ASSERT(! unreserved(' '));

        while(it != last)
        {
            if(unreserved(*it))
            {
                if(dest == end)
                    return dest - dest0;
                *dest++ = *it++;
                continue;
            }
            if(*it == ' ')
            {
                if(dest == end)
                    return dest - dest0;
                *dest++ = '+';
                ++it;
                continue;
            }
            if(dest > end3)
                return dest - dest0;
            encode(dest, *it++);
        }
    }
    return dest - dest0;
}

//------------------------------------------------

template<
    class StringToken,
    class CharSet>
BOOST_URL_STRTOK_RETURN
encode(
    string_view s,
    CharSet const& unreserved,
    encode_opts const& opt,
    StringToken&& token) noexcept
{
/*  If you get a compile error here, it
    means that the value you passed does
    not meet the requirements stated in
    the documentation.
*/
    static_assert(
        grammar::is_charset<CharSet>::value,
        "Type requirements not met");

    auto const n = encoded_size(
        s, unreserved, opt);
    auto p = token.prepare(n);
    if(n > 0)
        detail::encode_unsafe(
            p, p + n, s, unreserved, opt);
    return token.result();
}

} // urls
} // boost

#endif
