//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_CHAR_TYPE_HPP
#define BOOST_URL_DETAIL_CHAR_TYPE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/char_set.hpp>
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
            bnf::hexdig_value(p[0]) == -1 ||
            bnf::hexdig_value(p[1]) == -1)
        {
            ec = error::bad_pct_hexdig;
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

    bool
    check(string_view s) const noexcept
    {
        error_code ec;
        (void) decoded_size(s, ec);
        return !ec;
    }

    string_view
    validate(string_view s) const
    {
        if( !check(s) )
            invalid_part::raise();
        return s;
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
                    bnf::hexdig_value(p[1])) << 4) +
                static_cast<unsigned char>(
                    bnf::hexdig_value(p[2])));
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
