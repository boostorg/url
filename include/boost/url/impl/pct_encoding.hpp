//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODING_HPP
#define BOOST_URL_IMPL_PCT_ENCODING_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/grammar/is_mutable_string.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

template<class CharSet>
std::size_t
validate_pct_encoding(
    string_view s,
    error_code& ec,
    CharSet const& allowed,
    pct_decode_opts const& opt) noexcept
{
    // CharSet must satisfy is_charset
    BOOST_STATIC_ASSERT(
        grammar::is_charset<CharSet>::value);

    // can't have % in charset
    BOOST_ASSERT(! allowed('%'));

    // we can't accept plus to space if '+' is not allowed
    BOOST_ASSERT(! opt.plus_to_space || allowed('+'));

    std::size_t n = 0;
    char const* it = s.data();
    char const* end = it + s.size();
    while(it != end)
    {
        if( ! opt.allow_null &&
            *it == '\0')
        {
            // null in input
            ec = BOOST_URL_ERR(
                error::illegal_null);
            return n;
        }
        if(allowed(*it))
        {
            // unreserved
            ++n;
            ++it;
            continue;
        }
        if(*it == '%')
        {
            // escaped
            ++it;
            if(end - it < 2)
            {
                // missing HEXDIG
                ec = BOOST_URL_ERR(
                    error::missing_pct_hexdig);
                return n;
            }
            char d0;
            bool r0 = grammar::hexdig_value(it[0], d0);
            char d1;
            bool r1 = grammar::hexdig_value(it[1], d1);
            if( !r0 || !r1 )
            {
                // expected HEXDIG
                ec = BOOST_URL_ERR(
                    error::bad_pct_hexdig);
                return n;
            }
            it += 2;
            char const c = static_cast<char>(
                ((static_cast<
                    unsigned char>(d0) << 4) +
                (static_cast<
                    unsigned char>(d1))));
            if( ! opt.allow_null &&
                c == '\0')
            {
                // escaped null
                ec = BOOST_URL_ERR(
                    error::illegal_null);
                return n;
            }
            if( opt.non_normal_is_error &&
                allowed(c))
            {
                // escaped unreserved char
                ec = BOOST_URL_ERR(
                    error::non_canonical);
                return n;
            }
            ++n;
            continue;
        }
        // reserved character in input
        ec = BOOST_URL_ERR(
            error::illegal_reserved_char);
        return n;
    }
    BOOST_ASSERT(it == end);
    ec = {};
    return n;
}

//------------------------------------------------

template<class CharSet>
std::size_t
pct_decode(
    char* dest,
    char const* end,
    string_view s,
    error_code& ec,
    CharSet const& allowed,
    pct_decode_opts const& opt) noexcept
{
    // CharSet must satisfy is_charset
    BOOST_STATIC_ASSERT(
        grammar::is_charset<CharSet>::value);

    auto const n =
        validate_pct_encoding(
            s, ec, allowed, opt);
    if(ec.failed())
        return 0;
    auto const n1 =
        pct_decode_unchecked(
            dest, end, s, opt);
    if(n1 < n)
    {
        ec = error::no_space;
        return n1;
    }
    return n1;
}

//------------------------------------------------

namespace detail {
template <class Iter, class CharSet>
std::size_t
pct_encode_bytes(
    Iter it,
    Iter const end,
    CharSet const& allowed,
    pct_encode_opts const& opt = {}) noexcept
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
pct_encode_bytes(
    string_view s,
    CharSet const& allowed,
    pct_encode_opts const& opt) noexcept
{
    return detail::pct_encode_bytes(
        s.data(), s.data() + s.size(), allowed, opt);
}

//------------------------------------------------

namespace detail {
template<class Iter, class CharSet>
std::size_t
pct_encode(
    char* dest,
    char const* const end,
    Iter p,
    Iter last,
    CharSet const& allowed,
    pct_encode_opts const& opt = {})
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
}

template<class CharSet>
std::size_t
pct_encode(
    char* dest,
    char const* const end,
    string_view s,
    CharSet const& allowed,
    pct_encode_opts const& opt)
{
    return detail::pct_encode(
        dest, end, s.data(), s.data() + s.size(), allowed, opt);
}

//------------------------------------------------

template<
    class CharSet,
    class Allocator>
std::basic_string<char,
    std::char_traits<char>,
        Allocator>
pct_encode_to_string(
    string_view s,
    CharSet const& allowed,
    pct_encode_opts const& opt,
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
        pct_encode_bytes(s, allowed, opt);
    r.resize(n);
    char* dest = &r[0];
    char const* end = dest + n;
    auto const n1 = pct_encode(
        dest, end, s, allowed, opt);
    BOOST_ASSERT(n1 == n);
    (void)n1;
    return r;
}


} // urls
} // boost

#endif
