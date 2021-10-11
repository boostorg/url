//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_PUNYCODE_HPP
#define BOOST_URL_DETAIL_PUNYCODE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/detail/except.hpp>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits.h>

namespace boost {
namespace urls {
namespace detail {

// ascii output iterator which
// counts ascii characters
struct ascii_count
{
    std::size_t* n_;

public:
    using value_type = char;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type const*;
    using reference = value_type const&;
    using iterator_category =
        std::output_iterator_tag;

    ascii_count(
        ascii_count const&) = default;
    ascii_count& operator=(
        ascii_count const&) = default;

    explicit
    ascii_count(
        std::size_t& n)
        : n_(&n)
    {
        *n_ = 0;
    }

    ascii_count&
    operator=(char) noexcept
    {
        ++*n_;
        return *this;
    }

    ascii_count&
    operator*() noexcept
    {
        return *this;
    }

    ascii_count&
    operator++() noexcept
    {
        return *this;
    }

    ascii_count&
    operator++(int) noexcept
    {
        return *this;
    }
};

// utf32 output iterator which
// counts utf8 code points
class utf8_count
{
    std::size_t* n_;

public:
    using value_type = char32_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type const*;
    using reference = value_type const&;
    using iterator_category =
        std::output_iterator_tag;

    utf8_count(
        utf8_count const&) = default;
    utf8_count& operator=(
        utf8_count const&) = default;

    explicit
    utf8_count(
        std::size_t& n) noexcept
        : n_(&n)
    {
        *n_ = 0;
    }

    utf8_count&
    operator=(char32_t cp) noexcept
    {
        if(cp < 0x80)
            *n_ += 1;
        else if(cp < 0x800)
            *n_ += 2;
        else if(cp < 0x10000)
            *n_ += 3;
        else
            *n_ += 4;
        return *this;
    }

    utf8_count&
    operator*() noexcept
    {
        return *this;
    }

    utf8_count&
    operator++() noexcept
    {
        return *this;
    }

    utf8_count&
    operator++(int) noexcept
    {
        return *this;
    }
};

// utf32 output iterator which
// counts utf32 code points
struct utf32_count
{
    std::size_t* n_;

public:
    using value_type = char32_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type const*;
    using reference = value_type const&;
    using iterator_category =
        std::output_iterator_tag;

    utf32_count(
        utf32_count const&) = default;
    utf32_count& operator=(
        utf32_count const&) = default;

    explicit
    utf32_count(
        std::size_t& n)
        : n_(&n)
    {
        *n_ = 0;
    }

    utf32_count&
    operator=(char32_t) noexcept
    {
        ++*n_;
        return *this;
    }

    utf32_count&
    operator*() noexcept
    {
        return *this;
    }

    utf32_count&
    operator++() noexcept
    {
        return *this;
    }

    utf32_count&
    operator++(int) noexcept
    {
        return *this;
    }
};

// utf32 output iterator which
// emits utf8 code points
class utf8_output
{
    char* dest_;

public:
    using value_type = char32_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type const*;
    using reference = value_type const&;
    using iterator_category =
        std::output_iterator_tag;

    utf8_output(
        char* dest)
        : dest_(dest)
    {
    }

    utf8_output&
    operator=(char32_t cp) noexcept
    {               
        if(cp < 0x80)
        {
            *dest_++ = static_cast<char>(cp);
        }
        else if(cp < 0x800)
        {
            *dest_++ = static_cast<char>( (cp >> 6)          | 0xc0);
            *dest_++ = static_cast<char>( (cp & 0x3f)        | 0x80);
        }
        else if(cp < 0x10000)
        {
            *dest_++ = static_cast<char>( (cp >> 12)         | 0xe0);
            *dest_++ = static_cast<char>(((cp >> 6) & 0x3f)  | 0x80);
            *dest_++ = static_cast<char>( (cp       & 0x3f)  | 0x80);
        }
        else
        {
            *dest_++ = static_cast<char>( (cp >> 18)         | 0xf0);
            *dest_++ = static_cast<char>(((cp >> 12) & 0x3f) | 0x80);
            *dest_++ = static_cast<char>(((cp >> 6)  & 0x3f) | 0x80);
            *dest_++ = static_cast<char>( (cp        & 0x3f) | 0x80);
        }
        return *this;
    }

    utf8_output&
    operator*() noexcept
    {
        return *this;
    }

    utf8_output&
    operator++() noexcept
    {
        return *this;
    }

    utf8_output&
    operator++(int) noexcept
    {
        return *this;
    }
};

// utf32 forward iterator which
// reads utf8 code points
class utf8_input
{
    static char32_t const invalid =
        0xffffffff;

    char const* s_;
    char const* end_;
    char32_t cp_;

public:
    using value_type = char32_t;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type const*;
    using reference = value_type const&;
    using iterator_category =
        std::forward_iterator_tag;

    utf8_input(
        char const* s,
        char const* end)
        : s_(s)
        , end_(end)
    {
        next();
    }

    explicit
    utf8_input(
        char const* s) noexcept
        : s_(s)
        , end_(s)
        , cp_(invalid)
    {
    }

    bool
    operator==(
        utf8_input const& other) const noexcept
    {
        return
            s_ == other.s_ &&
            end_ == other.end_ &&
            cp_ == other.cp_;
    }

    bool
    operator!=(
        utf8_input const& other) const noexcept
    {
        return !(*this == other);
    }

    char32_t
    operator*() const noexcept
    {
        BOOST_ASSERT(cp_ != invalid);
        return cp_;
    }

    utf8_input&
    operator++()
    {
        BOOST_ASSERT(cp_ != invalid);
        next();
        return *this;
    }

    utf8_input
    operator++(int)
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

private:
    static
    char32_t
    parse_utf8(
        char const*& in0,
        char const* end)
    {
        if(in0 >= end)
            throw_invalid_argument(
                BOOST_CURRENT_LOCATION);
        char32_t cp;
        char const* in = in0;

        auto const mask =
            [](char c) noexcept
            {
                return static_cast<
                    unsigned char>(
                        0xff & c);
            };

        auto const u = mask(*in);
        if(u < 0x80)
        {
            ++in;
            cp = u;
        }
        else if((u >> 5) == 0x06)
        {
            if(end - in < 2)
                throw_invalid_argument(
                    BOOST_CURRENT_LOCATION);
            cp =
                ((u << 6) & 0x7ff) +
                (mask(in[1]) & 0x3f);
            in += 2;
                 
        }
        else if((u >> 4) == 0x0e)
        {
            if(end - in < 3)
                throw_invalid_argument(
                    BOOST_CURRENT_LOCATION);
            cp =
                ((u << 12) & 0xffff) +
                ((mask(in[1]) << 6) & 0xfff) +
                (mask(in[2]) & 0x3f);
            in += 3;
        }
        else if((u >> 3) == 0x1e)
        {
            if(end - in < 4)
                throw_invalid_argument(
                    BOOST_CURRENT_LOCATION);
            cp =
                ((u << 18) & 0x1fffff) +
                ((mask(in[1]) << 12) & 0x3ffff) +
                ((mask(in[2]) << 6) & 0xfff) +
                (mask(in[3]) & 0x3f);
            in += 4;
        }
        else
        {
            throw_invalid_argument(
                BOOST_CURRENT_LOCATION);
        }
        in0 = in;
        return cp;
    }

    void
    next()
    {
        if(s_ == end_)
        {
            cp_ = invalid;
            return;
        }
        cp_ = parse_utf8(s_, end_);
    }
};

//----------------------------------------------------------

/*  This is a derivative work.
    Original is copryght by Adam M. Costello:
    http://www.nicemice.net/idn/punycode-spec.gz
*/

class puny
{
    enum : std::size_t
    {
        base = 36,
        tmin = 1,
        tmax = 26,
        skew = 38,
        damp = 700,
        initial_n = 128,
        initial_bias = 72
    };

    static
    std::size_t
    adapt(
        std::size_t delta,
        std::size_t n_points,
        bool is_first)
    {
        // scale back then increase delta
        delta /= is_first ? damp : 2;
        delta += delta / n_points;

        std::uint32_t k = 0;
        auto const lim =
            ((base - tmin) * tmax) / 2;
        while(delta > lim)
        {
            k += base;
            delta /= (base - tmin);
        }

        return k + (
            ((base - tmin + 1) * delta) /
            (delta + skew));
    }

    static
    char
    encode_digit(std::size_t c)
    {
        BOOST_ASSERT(c <= 35);
        // digit
        if(c > 25)
            return static_cast<
                char>(c + 22);
        // letter
        return static_cast<
            char>(c + 'a');
    }

    // write a variable length integer
    template<class OutputIt>
    static
    std::size_t
    encode_varint(
        OutputIt& dest, // in-out
        std::size_t bias,
        std::size_t delta) noexcept
    {
        std::size_t n = 0;
        std::size_t k = base;
        std::size_t q = delta;
        for(;;)
        {
            std::size_t t;
            if(k <= bias)
                t = tmin;
            else if(k >= bias + tmax)
                t = tmax;
            else
                t = k - bias;
            if(q < t)
                break;
            ++n;
            *dest++ = encode_digit(
                t + (q - t) % (base - t));
            q = (q - t) / (base - t);
            k += base;
        }
        *dest++ = encode_digit(q);
        ++n;
        return n;
    }

    static
    std::size_t
    decode_digit(
        char32_t cp)
    {
        auto const ucp = static_cast<
            std::uint32_t>(cp);
        if(ucp - 0x30 < 0x0a)
            return cp - 22;
        if(std::islower(cp))
            return cp - 'a';
        if(std::isupper(cp))
            return cp - 'A';
        return SIZE_MAX;
    }

public:
    template<
        class OutputIt, // write ascii
        class InputIt   // read utf-32
    >
    static
    std::size_t
    encode(
        OutputIt dest,
        InputIt first,
        InputIt last)
    {
        std::size_t b;
        std::size_t h;
        std::size_t delta;
        std::size_t bias;
        std::size_t n;

        std::size_t di = 0;
        std::size_t srclen = 0;

        // copy the low-ascii chars
        auto src = first;
        while(src != last)
        {
            ++srclen;
            auto const cp = *src++;
            if(cp < 128)
            {
                ++di;
                *dest++ =
                    static_cast<
                        char>(cp);
            }
        }

        b = di;
        h = di;

        // output delimiter if needed
        if(di > 0)
        {
            ++di;
            *dest++ = '-';
        }

        n = initial_n;
        bias = initial_bias;
        delta = 0;

        for(; h < srclen; n++, delta++)
        {
            // Find next smallest non-basic code point.
            std::size_t m;
            for(m = SIZE_MAX, src = first;
                src != last; ++src)
            {
                auto const cp = *src;
                if(cp >= n && cp < m)
                    m = static_cast<
                        std::size_t>(cp);
            }

            if((m - n) > (SIZE_MAX - delta) / (h + 1))
            {
                BOOST_ASSERT(0 && "OVERFLOW");
                goto fail;
            }

            delta += (m - n) * (h + 1);
            n = m;

            for(src = first; src != last; ++src)
            {
                auto const cp = *src;
                if(cp < n)
                {
                    if(++delta == 0)
                    {
                        BOOST_ASSERT(0 && "OVERFLOW");
                        goto fail;
                    }
                }
                else if(cp == n)
                {
                    auto len = encode_varint(
                        dest, bias, delta);
                    di += len;
                    bias = adapt(delta, h + 1, h == b);
                    delta = 0;
                    h++;
                }
            }
        }

    fail:
        return di;
    }

    template<class InputIt>
    static
    std::size_t
    encoded_size(
        InputIt first,
        InputIt last)
    {
        std::size_t n = 0;
        encode(ascii_count{n}, first, last);
        return n;
    }

    static
    void
    decode(
        char const* src,
        //char const* const last,
        const size_t srclen,
        char32_t* dest,
        size_t* const dstlen)
    {
        char const* const begin = src;
        char const* const end = begin + srclen;

        // validate the input and also find the last '-'
        std::size_t delim_pos =
            [begin, end]()
            {
                std::size_t n = 0;
                std::size_t i = 0;
                auto src = begin;
                while(src != end)
                {
                    if(*src & 0x80)
                    {
                        // invalid high-ascii
                        throw_invalid_argument(
                            BOOST_CURRENT_LOCATION);
                    }
                    if(*src == '-')
                        n = i;
                    ++i;
                    ++src;
                }
                return n;
            }();

        /* Copy basic code points to output. */
        //di = (std::min)(delim_pos, *dstlen);
        auto di = delim_pos;

        for(auto i = 0; i < di; i++)
            dest[i] = src[i];
            //dest[i] = 0;

        std::size_t i = 0;
        char32_t n = initial_n;
        std::size_t bias = initial_bias;

        // basic code points
        //auto bsrc = begin;

        // encoded digits
        auto csrc = src + delim_pos +
            ((delim_pos > 0) ? 1 : 0);

        for(;csrc < end && di < *dstlen; di++)
        {
            auto const i0 = i;
            for(std::size_t w = 1,
                    k = base;
                di < *dstlen;
                k += base)
            {
                auto const digit =
                    decode_digit(*csrc++);
                if(digit == SIZE_MAX)
                    goto fail;
                if(digit > (SIZE_MAX - i) / w)
                {
                    BOOST_ASSERT(0 && "OVERFLOW");
                    goto fail;
                }
                i += digit * w;
                std::size_t t;
                if(k <= bias)
                    t = tmin;
                else if(k >= bias + tmax)
                    t = tmax;
                else
                    t = k - bias;
                if(digit < t)
                    break;
                if(w > SIZE_MAX / (base - t))
                {
                    BOOST_ASSERT(0 && "OVERFLOW");
                    goto fail;
                }
                w *= base - t;
            }

            bias = adapt(
                i - i0,
                di + 1,
                i0 == 0);

            if(i / (di + 1) > SIZE_MAX - n)
            {
                BOOST_ASSERT(0 && "OVERFLOW");
                goto fail;
            }

            BOOST_ASSERT(
                i / (di + 1) < ULONG_MAX);
            n += static_cast<std::uint32_t>(
                i / (di + 1));
            i %= (di + 1);

        #if 0
            for(auto count = i - i0;
                count; --count)
                *dest++ = *bsrc++;
            *dest++ = n;
            ++delim_pos;
            ++i;
        #else
            std::memmove(
                dest + i + 1,
                dest + i,
                (di - i) * sizeof(char32_t));
            dest[i++] = n;
        #endif
        }

#if 0
        for(auto count = delim_pos - i;
            count; --count)
            *dest++ = *bsrc++;
#endif

    fail:
        *dstlen = di;
    }
};

} // detail
} // urls
} // boost

#endif
