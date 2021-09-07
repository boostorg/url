//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_IPV6_ADDRESS_HPP
#define BOOST_URL_IPV6_ADDRESS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <array>
#include <cstdint>
#include <iosfwd>

namespace boost {
namespace urls {

class ipv4_address;

class ipv6_address
{
public:
    /** The number of characters in the longest possible ipv4 string
    */
    // ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff
    // ::ffff:255.255.255.255
    // 12345678901234567890123456789012345678901234567890
    //          1         2         3        4
    static
    constexpr
    std::size_t max_str_len = 49;

    /** The type used to represent an address as an array of bytes
    */
    using bytes_type =
        std::array<unsigned char, 16>;

    /** Constructor
    */
    BOOST_URL_DECL
    ipv6_address() noexcept;

    /** Construct from raw bytes
    */
    BOOST_URL_DECL
    explicit
    ipv6_address(
        bytes_type const& bytes) noexcept;

    /** Constructor
    */
    BOOST_URL_DECL
    ipv6_address(
        ipv6_address const& other) noexcept;

    /** Assign from another address
    */
    BOOST_URL_DECL
    ipv6_address&
    operator=(
        ipv6_address const& other) noexcept;

    /** Return the address as bytes, in network byte order
    */
    BOOST_URL_DECL
    bytes_type
    to_bytes() const noexcept;

    /** Return the address as a string

        The returned string will not
        contain surrounding square brackets.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.
    */
    template<class Allocator =
        std::allocator<char>>
    string_type<Allocator>
    to_string(Allocator const& a = {}) const;

    /** Return true if the address is unspecified
    */
    BOOST_URL_DECL
    bool
    is_unspecified() const noexcept;

    /** Return true if the address is a loopback address
    */
    BOOST_URL_DECL
    bool
    is_loopback() const noexcept;

    /** Return true if the address is link local
    */
    BOOST_URL_DECL
    bool
    is_link_local() const noexcept;

    /** Return true if the address is site local
    */
    BOOST_URL_DECL
    bool
    is_site_local() const noexcept;

    /** Return true if the address is a mapped IPv4 address
    */
    BOOST_URL_DECL
    bool
    is_v4_mapped() const noexcept;

    /** Return true if the address is a multicast address
    */
    BOOST_URL_DECL
    bool
    is_multicast() const noexcept;

    /** Return true if the address is a global multicast address
    */
    BOOST_URL_DECL
    bool
    is_multicast_global() const noexcept;

    /** Return true if the address is a link-local multicast address
    */
    BOOST_URL_DECL
    bool
    is_multicast_link_local() const noexcept;

    /** Return true if the address is a node-local multicast address
    */
    BOOST_URL_DECL
    bool
    is_multicast_node_local() const noexcept;

    /** Return true if the address is a org-local multicast address
    */
    BOOST_URL_DECL
    bool
    is_multicast_org_local() const noexcept;

    /** Return true if the address is a site-local multicast address
    */
    BOOST_URL_DECL
    bool
    is_multicast_site_local() const noexcept;
  
    /** Return true if two addresses are equal
    */
    BOOST_URL_DECL
    friend
    bool
    operator==(
        ipv6_address const& a1,
        ipv6_address const& a2) noexcept;

    /** Return true if two addresses are not equal
    */
    friend
    bool
    operator!=(
        ipv6_address const& a1,
        ipv6_address const& a2) noexcept
    {
        return !( a1 == a2 );
    }

    /** Return an address object that represents the loopback address
    */
    BOOST_URL_DECL
    static
    ipv6_address
    loopback() noexcept;

    /** Format the address to an output stream
    */
    BOOST_URL_DECL
    friend
    std::ostream&
    operator<<(
        std::ostream&,
        ipv6_address const&);

private:
    BOOST_URL_DECL
    std::size_t
    print_impl(
        char* dest) const noexcept;

    bytes_type addr_;
};

/** Return an IPv6 address from an IP address string
*/
BOOST_URL_DECL
ipv6_address
make_ipv6_address(
    string_view s,
    error_code& ec) noexcept;

/** Return an IPv6 address from an IP address string
*/
BOOST_URL_DECL
ipv6_address
make_ipv6_address(
    string_view s);

/** Return an IPv6 address from an IPv4 address
*/
BOOST_URL_DECL
ipv6_address
make_ipv6_address(
    ipv4_address const& a) noexcept;

} // urls
} // boost

#include <boost/url/impl/ipv6_address.hpp>

#endif
