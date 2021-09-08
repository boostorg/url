//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_IMPL_IPV4_ADDRESS_IPP
#define BOOST_URL_IMPL_IPV4_ADDRESS_IPP

#include <boost/url/ipv4_address.hpp>
#include <boost/url/detail/network_order.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/ipv4_address_bnf.hpp>
#include <cstring>

namespace boost {
namespace urls {

ipv4_address::
ipv4_address(
    bytes_type const& bytes)
{
    std::memcpy(&addr_,
        bytes.data(), 4);
}

ipv4_address::
ipv4_address(uint_type addr)
    : addr_(detail::host_to_network(addr))
{
}

auto
ipv4_address::
to_bytes() const noexcept ->
    bytes_type
{
    bytes_type bytes;
    std::memcpy(
        bytes.data(),
        &addr_, 4);
    return bytes;
}

auto
ipv4_address::
to_uint() const noexcept ->
    uint_type
{
    return detail::network_to_host(
        addr_);
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
        0xE0000000;;
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

std::ostream&
operator<<(
    std::ostream& os,
    ipv4_address const& a)
{
    char buf[ipv4_address::max_str_len + 1];
    auto n = a.print_impl(buf);
    buf[n] = '\0';
    os << string_view(buf, n);
    return os;
}

ipv4_address
make_ipv4_address(
    string_view s,
    error_code& ec) noexcept
{
    ipv4_address_bnf t;
    using bnf::parse;
    if(! parse(s, ec, t))
        return {};
    return t.addr;
}

ipv4_address
make_ipv4_address(
    string_view s)
{
    error_code ec;
    auto const addr =
        make_ipv4_address(s, ec);
    detail::maybe_throw(ec,
        BOOST_CURRENT_LOCATION);
    return addr;
}

} // urls
} // boost

#endif
