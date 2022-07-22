//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_IPV6_ADDRESS_IPP
#define BOOST_URL_IMPL_IPV6_ADDRESS_IPP

#include <boost/url/ipv6_address.hpp>
#include <boost/url/ipv4_address.hpp>
#include <boost/url/rfc/h16_rule.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/grammar/parse.hpp>
#include <cstring>

namespace boost {
namespace urls {

namespace detail {

// return `true` if the hex
// word could be 0..255 if
// interpreted as decimal
static
bool
maybe_octet(
    unsigned char const* p) noexcept
{
    unsigned short word =
        static_cast<unsigned short>(
            p[0]) * 256 +
        static_cast<unsigned short>(
            p[1]);
    if(word > 0x255)
        return false;
    if(((word >>  4) & 0xf) > 9)
        return false;
    if((word & 0xf) > 9)
        return false;
    return true;
}

} // detail

ipv6_address::
ipv6_address(
    bytes_type const& bytes) noexcept
{
    std::memcpy(&addr_,
        bytes.data(), 16);
}

ipv6_address::
ipv6_address(
    ipv4_address const& addr) noexcept
{
    auto const v = addr.to_bytes();
    ipv6_address::bytes_type bytes = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0xff, 0xff, v[0], v[1], v[2], v[3] } };
    std::memcpy(&addr_, bytes.data(), 16);
}

ipv6_address::
ipv6_address(
    string_view s)
{
    auto r = parse_ipv6_address(s);
    if(r.has_error())
        detail::throw_invalid_argument(
            BOOST_CURRENT_LOCATION);
    *this = r.value();
}

string_view
ipv6_address::
to_buffer(
    char* dest,
    std::size_t dest_size) const
{
    if(dest_size < max_str_len)
        detail::throw_length_error(
            "ipv6_address::to_buffer",
            BOOST_CURRENT_LOCATION);
    auto n = print_impl(dest);
    return string_view(dest, n);
}

bool
ipv6_address::
is_loopback() const noexcept
{
    return *this == loopback();
}

bool
ipv6_address::
is_unspecified() const noexcept
{
    return *this == ipv6_address();
}

bool
ipv6_address::
is_v4_mapped() const noexcept
{
    return
        addr_[ 0] == 0 && addr_[ 1] == 0 &&
        addr_[ 2] == 0 && addr_[ 3] == 0 &&
        addr_[ 4] == 0 && addr_[ 5] == 0 &&
        addr_[ 6] == 0 && addr_[ 7] == 0 &&
        addr_[ 8] == 0 && addr_[ 9] == 0 &&
        addr_[10] == 0xff &&
        addr_[11] == 0xff;
}

ipv6_address
ipv6_address::
loopback() noexcept
{
    ipv6_address a;
    a.addr_[15] = 1;
    return a;
}

std::size_t
ipv6_address::
print_impl(
    char* dest) const noexcept
{
    auto const count_zeroes =
    []( unsigned char const* first,
        unsigned char const* const last)
    {
        std::size_t n = 0;
        while(first != last)
        {
            if( first[0] != 0 ||
                first[1] != 0)
                break;
            n += 2;
            first += 2;
        }
        return n;
    };
    auto const print_hex =
    []( char* dest,
        unsigned short v)
    {
        char const* const dig =
            "0123456789abcdef";
        if(v >= 0x1000)
        {
            *dest++ = dig[v>>12];
            v &= 0x0fff;
            *dest++ = dig[v>>8];
            v &= 0x0ff;
            *dest++ = dig[v>>4];
            v &= 0x0f;
            *dest++ = dig[v];
        }
        else if(v >= 0x100)
        {
            *dest++ = dig[v>>8];
            v &= 0x0ff;
            *dest++ = dig[v>>4];
            v &= 0x0f;
            *dest++ = dig[v];
        }
        else if(v >= 0x10)
        {
            *dest++ = dig[v>>4];
            v &= 0x0f;
            *dest++ = dig[v];
        }
        else
        {
            *dest++ = dig[v];
        }
        return dest;
    };
    auto const dest0 = dest;
    // find longest run of zeroes
    std::size_t best_len = 0;
    int best_pos = -1;
    auto it = addr_.data();
    auto const v4 =
        is_v4_mapped();
    auto const end = v4 ?
        (it + addr_.size() - 4)
        : it + addr_.size();
    while(it != end)
    {
        auto n = count_zeroes(
            it, end);
        if(n == 0)
        {
            it += 2;
            continue;
        }
        if(n > best_len)
        {
            best_pos = static_cast<
                int>(it - addr_.data());
            best_len = n;
        }
        it += n;
    }
    it = addr_.data();
    if(best_pos != 0)
    {
        unsigned short v =
            (it[0] * 256U) + it[1];
        dest = print_hex(dest, v);
        it += 2;
    }
    else
    {
        *dest++ = ':';
        it += best_len;
        if(it == end)
            *dest++ = ':';
    }
    while(it != end)
    {
        *dest++ = ':';
        if(it - addr_.data() ==
            best_pos)
        {
            it += best_len;
            if(it == end)
                *dest++ = ':';
            continue;
        }
        unsigned short v =
            (it[0] * 256U) + it[1];
        dest = print_hex(dest, v);
        it += 2;
    }
    if(v4)
    {
        ipv4_address::bytes_type bytes;
        bytes[0] = it[0];
        bytes[1] = it[1];
        bytes[2] = it[2];
        bytes[3] = it[3];
        ipv4_address a(bytes);
        *dest++ = ':';
        dest += a.print_impl(dest);
    }
    return dest - dest0;
}

//------------------------------------------------

auto
ipv6_address_rule_t::
parse(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<ipv6_address>
{
    int n = 8;      // words needed
    int b = -1;     // value of n
                    // when '::' seen
    bool c = false; // need colon
    auto prev = it;
    ipv6_address::bytes_type bytes;
    result<h16_rule_t::value_type> rv;
    for(;;)
    {
        if(it == end)
        {
            if(b != -1)
            {
                // end in "::"
                break;
            }
            BOOST_ASSERT(n > 0);
            // not enough words
            return error::missing_words;
        }
        if(*it == ':')
        {
            ++it;
            if(it == end)
            {
                // missing ':'
                return error::missing_char_literal;
            }
            if(*it == ':')
            {
                if(b == -1)
                {
                    // first "::"
                    ++it;
                    --n;
                    b = n;
                    if(n == 0)
                        break;
                    c = false;
                    continue;
                }
                // two "::"
                return error::bad_ipv6;
            }
            if(c)
            {
                prev = it;
                rv = grammar::parse(
                    it, end, h16_rule);
                if(! rv)
                    return rv.error();
                bytes[2*(8-n)+0] = rv->hi;
                bytes[2*(8-n)+1] = rv->lo;
                --n;
                if(n == 0)
                    break;
                continue;
            }
            // expected h16
            return error::missing_words;
        }
        if(*it == '.')
        {
            if(b == -1 && n > 1)
            {
                // not enough h16
                return error::bad_ipv6;
            }
            if(! detail::maybe_octet(
                &bytes[2*(7-n)]))
            {
                // invalid octet
                return error::bad_octet;
            }
            // rewind the h16 and
            // parse it as ipv4
            it = prev;
            auto rv1 = grammar::parse(
                it, end, ipv4_address_rule);
            if(! rv1)
                return rv1.error();
            auto v4 = rv1.value();
            auto const b4 =
                v4.to_bytes();
            bytes[2*(7-n)+0] = b4[0];
            bytes[2*(7-n)+1] = b4[1];
            bytes[2*(7-n)+2] = b4[2];
            bytes[2*(7-n)+3] = b4[3];
            --n;
            break;
        }
        char d;
        if( b != -1 &&
            !grammar::hexdig_value(*it, d))
        {
            // ends in "::"
            break;
        }
        if(! c)
        {
            prev = it;
            rv = grammar::parse(
                it, end, h16_rule);
            if(! rv)
                return rv.error();
            bytes[2*(8-n)+0] = rv->hi;
            bytes[2*(8-n)+1] = rv->lo;
            --n;
            if(n == 0)
                break;
            c = true;
            continue;
        }
        // ':' divides a word
        return error::bad_ipv6;
    }
    if(b == -1)
        return ipv6_address{bytes};
    if(b == n)
    {
        // "::" last
        auto const i =
            2 * (7 - n);
        std::memset(
            &bytes[i],
            0, 16 - i);
    }
    else if(b == 7)
    {
        // "::" first
        auto const i =
            2 * (b - n);
        std::memmove(
            &bytes[16 - i],
            &bytes[2],
            i);
        std::memset(
            &bytes[0],
            0, 16 - i);
    }
    else
    {
        // "::" in middle
        auto const i0 =
            2 * (7 - b);
        auto const i1 =
            2 * (b - n);
        std::memmove(
            &bytes[16 - i1],
            &bytes[i0 + 2],
            i1);
        std::memset(
            &bytes[i0],
            0, 16 - (i0 + i1));
    }
    return ipv6_address{bytes};
}

auto
parse_ipv6_address(
    string_view s) noexcept ->
        result<ipv6_address>
{
    return grammar::parse(
        s, ipv6_address_rule);
}

} // urls
} // boost

#endif
