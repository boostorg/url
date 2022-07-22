//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_IPV4_ADDRESS_IPP
#define BOOST_URL_IMPL_IPV4_ADDRESS_IPP

#include <boost/url/ipv4_address.hpp>
#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/dec_octet_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/sequence_rule.hpp>
#include <boost/url/detail/except.hpp>
#include <cstring>

namespace boost {
namespace urls {

ipv4_address::
ipv4_address(
    uint_type addr) noexcept
    : addr_(addr)
{
}

ipv4_address::
ipv4_address(
    bytes_type const& bytes) noexcept
{
    addr_ =
(static_cast<unsigned long>(bytes[0]) << 24) |
(static_cast<unsigned long>(bytes[1]) << 16) |
(static_cast<unsigned long>(bytes[2]) <<  8) |
(static_cast<unsigned long>(bytes[3]));
}

ipv4_address::
ipv4_address(
    string_view s)
{
    auto r = parse_ipv4_address(s);
    if(r.has_error())
        detail::throw_invalid_argument(
            BOOST_CURRENT_LOCATION);
    *this = r.value();
}

auto
ipv4_address::
to_bytes() const noexcept ->
    bytes_type
{
    bytes_type bytes;
    bytes[0] = (addr_ >> 24) & 0xff;
    bytes[1] = (addr_ >> 16) & 0xff;
    bytes[2] = (addr_ >>  8) & 0xff;
    bytes[3] =  addr_        & 0xff;
    return bytes;
}

auto
ipv4_address::
to_uint() const noexcept ->
    uint_type
{
    return addr_;
}

string_view
ipv4_address::
to_buffer(
    char* dest,
    std::size_t dest_size) const
{
    if(dest_size < max_str_len)
        detail::throw_length_error(
            "ipv4_address::to_buffer",
            BOOST_CURRENT_LOCATION);
    auto n = print_impl(dest);
    return string_view(dest, n);
}

bool
ipv4_address::
is_loopback() const noexcept
{
    return (to_uint() & 0xFF000000) ==
        0x7F000000;
}

bool
ipv4_address::
is_unspecified() const noexcept
{
    return to_uint() == 0;
}

bool
ipv4_address::
is_multicast() const noexcept
{
    return (to_uint() & 0xF0000000) ==
        0xE0000000;
}

std::size_t
ipv4_address::
print_impl(
    char* dest) const noexcept
{
    auto const start = dest;
    auto const write =
        []( char*& dest,
            unsigned char v)
        {
            if(v >= 100)
            {
                *dest++ = '0' +
                    v / 100;
                v %= 100;
            }
            if(v >= 10)
            {
                *dest++ = '0' +
                    v / 10;
                v %= 10;
            }
            *dest++ = '0' + v;
        };
    auto const v = to_uint();
    write(dest, (v >> 24) & 0xff);
    *dest++ = '.';
    write(dest, (v >> 16) & 0xff);
    *dest++ = '.';
    write(dest, (v >>  8) & 0xff);
    *dest++ = '.';
    write(dest, (v      ) & 0xff);
    return dest - start;
}

//------------------------------------------------

auto
ipv4_address_rule_t::
parse(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<value_type>
{
    auto rv = grammar::parse(
        it, end,
        grammar::sequence_rule(
            grammar::dec_octet_rule,
            grammar::char_rule('.'),
            grammar::dec_octet_rule,
            grammar::char_rule('.'),
            grammar::dec_octet_rule,
            grammar::char_rule('.'),
            grammar::dec_octet_rule));
    if(! rv)
        return rv.error();
    std::array<unsigned char, 4> v;
    v[0] = std::get<0>(*rv);
    v[1] = std::get<2>(*rv);
    v[2] = std::get<4>(*rv);
    v[3] = std::get<6>(*rv);
    return ipv4_address(v);
}

auto
parse_ipv4_address(
    string_view s) noexcept ->
        result<ipv4_address>
{
    return grammar::parse(
        s, ipv4_address_rule);
}

} // urls
} // boost

#endif
