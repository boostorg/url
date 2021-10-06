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
#include <boost/url/bnf/charset.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

namespace detail {

template<class CharSet>
std::size_t
validate_pct_encoding(
    char const*& it,
    char const* const end,
    error_code& ec,
    CharSet const& cs,
    pct_decode_opts const& opt) noexcept
{
}

} // detail

template<class CharSet>
std::size_t
validate_pct_encoding(
    string_view s,
    error_code& ec,
    CharSet const& cs,
    pct_decode_opts const& opt) noexcept
{
    // CharSet must satisfy is_charset
    BOOST_STATIC_ASSERT(
        bnf::is_charset<CharSet>::value);

    // can't have % in charset
    BOOST_ASSERT(! cs('%'));
    std::size_t n = 0;
    auto it = s.data();
    auto const end = it + s.size();
    while(it != end)
    {
        if( opt.plus_to_space &&
            *it == '+')
        {
            // plus to space
            ++n;
            ++it;
            continue;
        }
        if( ! opt.allow_null &&
            *it == '\0')
        {
            // null in input
            ec = BOOST_URL_ERR(
                error::illegal_null);
            return n;
        }
        if(cs(*it))
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
            auto const d0 =
                bnf::hexdig_value(it[0]);
            auto const d1 =
                bnf::hexdig_value(it[1]);
            if( d0 == -1 ||
                d1 == -1)
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
                cs(c))
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

template<class Allocator>
string_value
pct_decode_unchecked(
    string_view s,
    pct_decode_opts const& opt,
    Allocator const& a,
    std::size_t decoded_size)
{
    if(decoded_size == std::size_t(-1))
        decoded_size =
            pct_decoded_bytes_unchecked(s);
    char* dest;
    string_value r(
        decoded_size, a, dest);
    pct_decode_unchecked(
        dest, dest + decoded_size,
            s, opt);
    return r;
}

//------------------------------------------------

template<
    class String,
    class CharSet>
std::size_t
pct_encode_bytes(
    String const& s0,
    CharSet const& cs,
    pct_encode_opts const& opt) noexcept
{
    // String must satisfy is_stringlike
    BOOST_STATIC_ASSERT(
        is_stringlike<String>::value);

    // CharSet must satisfy is_charset
    BOOST_STATIC_ASSERT(
        bnf::is_charset<CharSet>::value);

    string_view s =
        to_string_view(s0);
    std::size_t n = 0;
    auto it = s.data();
    auto const end = it + s.size();
    if(! opt.space_to_plus)
    {
        while(it != end)
        {
            if(cs(*it))
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
    BOOST_ASSERT(! cs(' '));
    while(it != end)
    {
        if(*it == ' ')
            ++n;
        else if(cs(*it))
            ++n;
        else
            n += 3;
        ++it;
    }
    return n;
}

//------------------------------------------------

template<
    class String,
    class CharSet>
bool
pct_encode(
    char*& dest0,
    char const* const end,
    String const& s0,
    CharSet const& cs,
    pct_encode_opts const& opt)
{
    // String must satisfy is_stringlike
    BOOST_STATIC_ASSERT(
        is_stringlike<String>::value);

    // CharSet must satisfy is_charset
    BOOST_STATIC_ASSERT(
        bnf::is_charset<CharSet>::value);

    // Can't have % in charset
    BOOST_ASSERT(! cs('%'));

    static constexpr char hex[] =
        "0123456789abcdef";
    char* dest = dest0;
    string_view s = to_string_view(s0);
    auto p = s.data();
    auto const last = p + s.size();
    auto const end3 = end - 3;
    if(! opt.space_to_plus)
    {
        while(p != last)
        {
            if(cs(*p))
            {
                if(dest == end)
                {
                    dest0 = dest;
                    return false;
                }
                *dest++ = *p++;
                continue;
            }
            if(dest > end3)
            {
                dest0 = dest;
                return false;
            }
            *dest++ = '%';
            auto const u = static_cast<
                unsigned char>(*p);
            *dest++ = hex[u>>4];
            *dest++ = hex[u&0xf];
            ++p;
        }
        dest0 = dest;
        return true;
    }
    // If you are converting space
    // to plus, then space should
    // be in the list of reserved
    // characters!
    BOOST_ASSERT(! cs(' '));
    while(p != last)
    {
        if(cs(*p))
        {
            if(dest == end)
            {
                dest0 = dest;
                return false;
            }
            *dest++ = *p++;
            continue;
        }
        if(*p == ' ')
        {
            if(dest == end)
            {
                dest0 = dest;
                return false;
            }
            *dest++ = '+';
            ++p;
            continue;
        }
        if(dest > end3)
        {
            dest0 = dest;
            return false;
        }
        *dest++ = '%';
        auto const u = static_cast<
            unsigned char>(*p);
        *dest++ = hex[u>>4];
        *dest++ = hex[u&0xf];
        ++p;
    }
    dest0 = dest;
    return true;
}

//------------------------------------------------

template<
    class String,
    class CharSet,
    class Allocator>
string_value
pct_encode(
    String const& s0,
    CharSet const& cs,
    pct_encode_opts const& opt,
    Allocator const& a)
{
    // String must satisfy is_stringlike
    BOOST_STATIC_ASSERT(
        is_stringlike<String>::value);

    // CharSet must satisfy is_charset
    BOOST_STATIC_ASSERT(
        bnf::is_charset<CharSet>::value);

    auto const s = to_string_view(s0);
    if(s.empty())
        return string_value();
    auto const n =
        pct_encode_bytes(s, cs, opt);
    char* dest;
    string_value r(n, a, dest);
    char const* end = dest + n;
    bool const success = pct_encode(
        dest, end, s, cs, opt);
    BOOST_ASSERT(success);
    BOOST_ASSERT(dest == end);
    (void)end;
    (void)success;
    return r;
}

} // urls
} // boost

#endif
