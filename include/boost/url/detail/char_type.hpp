//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_CHAR_TYPE_HPP
#define BOOST_URL_DETAIL_CHAR_TYPE_HPP

#include <boost/url/config.hpp>
#include <boost/url/error.hpp>
#include <cstdint>

namespace boost {
namespace urls {
namespace detail {

inline
char
to_lower(char c) noexcept
{
    unsigned char u =
        static_cast<unsigned char>(c - 65);
    if(u > 25)
        return c;
    return 'a' + c;
}

inline
bool
is_alpha(
    char c) noexcept
{
    unsigned char u =
        static_cast<
        unsigned char>(c);
    if(u > 'z')
        return false;
    if(u >= 'a')
        return true;
    if(u > 'Z')
        return false;
    if(u >= 'A')
        return true;
    return false;
}

inline
char
hex_digit(char c) noexcept
{
    static constexpr char tab[] =
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" //   0...15
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" //  16...31
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" //  32...47
        "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\xff\xff\xff\xff\xff\xff" //  48...63
        "\xff\x0a\x0b\x0c\x0d\x0e\x0f\xff\xff\xff\xff\xff\xff\xff\xff\xff" //  64...79
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" //  80...95
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" //  96..111
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" // 112..127
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" // 128..143
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" // 144..159
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" // 160..175
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" // 176..191
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" // 192..207
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" // 208..223
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" // 224..239
        "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" // 240..255
        ;
    return tab[static_cast<unsigned char>(c)];
}

inline
bool
is_scheme_char(
    char c) noexcept
{
    // [A-Za-z.+-]
    static constexpr char tab[] =
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" //   0...31
        "\0\0\0\0\0\0\0\0\0\0\0\1\0\1\1\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" //  32...63
        "\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\0\0\0\0\0" //  64...95
        "\0\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\0\0\0\0\0" //  96..127
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" // 128..159
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" // 160..191
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" // 192..223
        "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" // 224..255
        ;
    return tab[static_cast<unsigned char>(c)] != 0;
}

//----------------------------------------------------------

class pct_encoding
{
    char const* tab_;

    char
    needed(char c) const noexcept
    {
        return tab_[static_cast<
            unsigned char>(c)];
    }

    void
    encode(char* dest, char c) const noexcept
    {
        static constexpr char hex[] =
            "0123456789ABCDEF";
        *dest++ = '%';
        auto const u = static_cast<
            unsigned char>(c);
        *dest++ = hex[u>>4];
        *dest = hex[u&0xf];
    }

public:
    pct_encoding(pct_encoding const&) = default;
    pct_encoding& operator=(pct_encoding const&) = default;

    explicit
    pct_encoding(
        char const* tab) noexcept
        : tab_(tab)
    {
    }

    char
    is_special(char c) const noexcept
    {
        return tab_[static_cast<
            unsigned char>(c)] == '\3';
    }

    static
    void
    check_escape(
        char const* p,
        char const* end,
        error_code& ec) noexcept
    {
        if(end - p < 2)
        {
            ec = error::incomplete_pct_encoding;
        }
        else if(
            hex_digit(p[0]) == -1 ||
            hex_digit(p[1]) == -1)
        {
            ec = error::bad_pct_encoding_digit;
        }
    }

    // Also performs validation
    std::size_t
    decoded_size(
        string_view s,
        error_code& ec) const noexcept
    {
        std::size_t n = 0;
        auto p = s.data();
        auto const p1 = p + s.size();
        while(p < p1)
        {
            auto c = *p++;
            if(c != '%')
            {
                if(is_special(c))
                {
                    ec = error::illegal_reserved_char;
                    break;
                }
                ++n;
                continue;
            }
            check_escape(p, p1, ec);
            if(ec)
                break;
            p += 2;
            ++n;
        }
        return n;
    }

    void
    validate(string_view s) const
    {
        error_code ec;
        decoded_size(s, ec);
        if(ec)
            invalid_part::raise();
    }

    char const*
    parse(
        char const* const begin,
        char const* const end,
        error_code& ec) const noexcept
    {
        auto p = begin;
        while(p < end)
        {
            if(*p == '%')
            {
                check_escape(
                    p + 1, end, ec);
                if(ec)
                    return p;
                p += 3;
                continue;
            }
            if(is_special(*p))
                break;
            ++p;
        }
        return p;
    }

    // unchecked
    static
    std::size_t
    raw_decoded_size(
        string_view s) noexcept
    {
        std::size_t n = 0;
        auto p = s.data();
        auto const p1 = p + s.size();
        while(p < p1)
        {
            auto c = *p;
            if(c != '%')
            {
                ++p;
                ++n;
                continue;
            }
            p += 3;
            ++n;
        }
        return n;
    }

    // Precondition: s is a valid encoded string
    static
    char*
    decode(
        char* dest,
        string_view s) noexcept
    {
        auto p = s.begin();
        auto const p1 = s.end();
        while(p < p1)
        {
            if(*p != '%')
            {
                *dest++ = *p++;
                continue;
            }
            *dest++ = static_cast<char>(
                (static_cast<unsigned char>(
                    hex_digit(p[1])) << 4) +
                static_cast<unsigned char>(
                    hex_digit(p[2])));
            p += 3;
        }
        return dest;
    }

    std::size_t
    encoded_size(
        string_view s) const noexcept
    {
        std::size_t n = 0;
        auto p = s.data();
        auto const p1 = p + s.size();
        while(p < p1)
            n += needed(*p++);
        return n;
    }

    std::size_t
    encode(
        char* dest,
        string_view s) const noexcept
    {
        auto p = s.data();
        auto const p1 = p + s.size();
        auto const d0 = dest;
        while(p < p1)
        {
            if(! is_special(*p))
            {
                *dest++ = *p++;
            }
            else
            {
                encode(dest, *p++);
                dest += 3;
            }
        }
        return dest - d0;
    }
};

//----------------------------------------------------------

inline
pct_encoding
reg_name_pct_set() noexcept
{
    // unreserved / subdelims
    static constexpr char tab[] =
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" //   0...31
        "\3\1\3\3\1\3\1\1\1\1\1\1\1\1\1\3" "\1\1\1\1\1\1\1\1\1\1\3\1\3\1\3\3" //  32...63
        "\3\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\3\1" //  64...95
        "\3\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\1\3" //  96..127
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 128..159
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 160..191
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 192..223
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 224..255
        ;
    return pct_encoding(tab);
}

inline
pct_encoding
userinfo_pct_set() noexcept
{
    // unreserved / subdelims / ':'
    static constexpr char tab[] =
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" //   0...31
        "\3\1\3\3\1\3\1\1\1\1\1\1\1\1\1\3" "\1\1\1\1\1\1\1\1\1\1\1\1\3\1\3\3" //  32...63
        "\3\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\3\1" //  64...95
        "\3\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\1\3" //  96..127
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 128..159
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 160..191
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 192..223
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 224..255
        ;
    return pct_encoding(tab);
}

// userinfo_pct_set without ':'
inline
pct_encoding
userinfo_nc_pct_set() noexcept
{
    return reg_name_pct_set();
}

inline
pct_encoding
pchar_pct_set() noexcept
{
    // unreserved / subdelims / ':' / '@'
    static constexpr char tab[] =
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" //   0...31
        "\3\1\3\3\1\3\1\1\1\1\1\1\1\1\1\3" "\1\1\1\1\1\1\1\1\1\1\1\1\3\1\3\3" //  32...63
        "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\3\1" //  64...95
        "\3\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\1\3" //  96..127
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 128..159
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 160..191
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 192..223
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 224..255
        ;
    return pct_encoding(tab);
}

inline
pct_encoding
pchar_nc_pct_set() noexcept
{
    // unreserved / subdelims / '@'
    static constexpr char tab[] =
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" //   0...31
        "\3\1\3\3\1\3\1\1\1\1\1\1\1\1\1\3" "\1\1\1\1\1\1\1\1\1\1\3\1\3\1\3\3" //  32...63
        "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\3\1" //  64...95
        "\3\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\1\3" //  96..127
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 128..159
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 160..191
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 192..223
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 224..255
        ;
    return pct_encoding(tab);
}

inline
pct_encoding
frag_pct_set() noexcept
{
    // unreserved / subdelims / ':' / '@' / '/' / '?'
    static constexpr char tab[] =
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" //   0...31
        "\3\1\3\3\1\3\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\1\3\1\3\1" //  32...63
        "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\3\1" //  64...95
        "\3\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\1\3" //  96..127
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 128..159
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 160..191
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 192..223
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 224..255
        ;
    return pct_encoding(tab);
}
inline
pct_encoding
qkey_pct_set() noexcept
{
    // frag_pct_set() minus '='
    static constexpr char tab[] =
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" //   0...31
        "\3\1\3\3\1\3\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\1\3\3\3\1" //  32...63
        "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\3\1" //  64...95
        "\3\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\1\3" //  96..127
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 128..159
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 160..191
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 192..223
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 224..255
        ;
    return pct_encoding(tab);
}

inline
pct_encoding
qval_pct_set() noexcept
{
    // frag_pct_set minus '&'
    static constexpr char tab[] =
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" //   0...31
        "\3\1\3\3\1\3\3\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\1\3\1\3\1" //  32...63
        "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\3\1" //  64...95
        "\3\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1" "\1\1\1\1\1\1\1\1\1\1\1\3\3\3\1\3" //  96..127
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 128..159
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 160..191
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 192..223
        "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" "\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3" // 224..255
        ;
    return pct_encoding(tab);
}

// DEPRECATED
inline
pct_encoding
query_pct_set() noexcept
{
    return frag_pct_set();
}

template<class Allocator>
string_type<Allocator>
decode(
    string_view sv,
    Allocator const& a)
{
    string_type<Allocator> s(a);
    s.resize(
        detail::pct_encoding::
            raw_decoded_size(sv));
    pct_encoding::decode(&s[0], sv);
    return s;
}

// returns true if decode(encoded)==match
inline
bool
key_equal(
    string_view encoded,
    string_view match)
{
    // VFALCO should we validate the encoded string?
    if(encoded.size() < match.size())
        return false; // trivial reject
    auto p0 = encoded.data();
    auto p1 = match.data();
    auto const e0 = p0 + encoded.size();
    auto const e1 = p1 + match.size();
    for(;;)
    {
        if(p0 >= e0)
            return p1 >= e1;
        if(p1 >= e1)
            return false;
        if(*p0 != '%')
        {
            if(*p0++ != *p1++)
                return false;
            continue;
        }
        BOOST_ASSERT(e0 - p0 >= 3);
        auto const ch = static_cast<char>(
            (static_cast<unsigned char>(
                hex_digit(p0[1])) << 4) +
            static_cast<unsigned char>(
                hex_digit(p0[2])));
        if(ch != *p1++)
            return false;
        p0 += 3;
    }       
}

//----------------------------------------------------------

struct port_string
{
    static unsigned constexpr N = 10;
    char buf_[N];
    unsigned char len_;

public:
    explicit
    port_string(
        unsigned n) noexcept
    {
        len_ = 1;
        for(;;)
        {
            buf_[N - len_] =
                '0' + (n % 10);
            n /= 10;
            if(! n)
                break;
            ++len_;
        }
    }

    char const*
    data() const noexcept
    {
        return &buf_[N - len_];
    }

    unsigned char
    size() const noexcept
    {
        return len_;
    }

    string_view
    get() const noexcept
    {
        return { data(), size() };
    }
};

} // detail
} // urls
} // boost

#endif
