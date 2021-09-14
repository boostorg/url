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

#ifndef BOOST_URL_DOCS
class ipv4_address;
#endif

/** Represents an IP version 6 style address

    Objects of this type are used to construct
    and manipulate IP version 6 addresses.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc4291">
        IP Version 6 Addressing Architecture (rfc4291)</a>
*/
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

        Octets are stored in network byte order.
    */
    using bytes_type =
        std::array<unsigned char, 16>;

    /** Constructor

        Default constructed objects represent
        the unspecified address.

        @li <a href="https://datatracker.ietf.org/doc/html/rfc4291#section-2.5.2">
            2.5.2. The Unspecified Address</a>

        @see @ref is_unspecified
    */
    BOOST_URL_DECL
    ipv6_address() noexcept;

    /** Constructor
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

    /** Assignment
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

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc4291#section-2.2">
            2.2. Text Representation of Addresses (rfc4291)</a>
    */
    template<class Allocator =
        std::allocator<char>>
    string_type<Allocator>
    to_string(Allocator const& a = {}) const;

    /** Write a dotted decimal string representing the address to a buffer

        The resulting buffer is not null-terminated.

        @throw std::length_error `dest_size < ipv6_address::max_str_len`

        @return The formatted string

        @param dest The buffer in which to write,
        which must have at least `dest_size` space.

        @param dest_size The size of the output buffer.
    */
    BOOST_URL_DECL
    string_view
    to_buffer(
        char* dest,
        std::size_t dest_size) const;

    /** Return true if the address is unspecified

        The address 0:0:0:0:0:0:0:0 is called the
        unspecified address. It indicates the
        absence of an address.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc4291#section-2.5.2">
            2.5.2. The Unspecified Address (rfc4291)</a>
    */
    BOOST_URL_DECL
    bool
    is_unspecified() const noexcept;

    /** Return true if the address is a loopback address

        The unicast address 0:0:0:0:0:0:0:1 is called
        the loopback address. It may be used by a node
        to send an IPv6 packet to itself.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc4291#section-2.5.3">
            2.5.3. The Loopback Address (rfc4291)</a>
    */
    BOOST_URL_DECL
    bool
    is_loopback() const noexcept;

    /** Return true if the address is a mapped IPv4 address

        This address type is used to represent the
        addresses of IPv4 nodes as IPv6 addresses.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc4291#section-2.5.5.2">
            2.5.5.2. IPv4-Mapped IPv6 Address (rfc4291)</a>
    */
    BOOST_URL_DECL
    bool
    is_v4_mapped() const noexcept;

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

        The unicast address 0:0:0:0:0:0:0:1 is called
        the loopback address. It may be used by a node
        to send an IPv6 packet to itself.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc4291#section-2.5.3">
            2.5.3. The Loopback Address (rfc4291)</a>
    */
    BOOST_URL_DECL
    static
    ipv6_address
    loopback() noexcept;

private:
    BOOST_URL_DECL
    std::size_t
    print_impl(
        char* dest) const noexcept;

    bytes_type addr_;
};

/** Format the address to an output stream
*/
BOOST_URL_DECL
std::ostream&
operator<<(
    std::ostream& os,
    ipv6_address const& addr);

/** Return an IPv6 address from an IP address string

    @par Exception Safety
    Does not throw.

    @return The parsed address if successful,
        otherwise a default constructed object.

    @param s The string to parse

    @param ec Set to the error, if any occurred
*/
BOOST_URL_DECL
ipv6_address
make_ipv6_address(
    string_view s,
    error_code& ec) noexcept;

/** Return an IPv6 address from an IP address string

    @throw system_error Thrown on failure

    @param s The string to parse
*/
BOOST_URL_DECL
ipv6_address
make_ipv6_address(
    string_view s);

/** Return an IPv6 address from an IPv4 address

    @param a The ipv4 address to use
*/
BOOST_URL_DECL
ipv6_address
make_ipv6_address(
    ipv4_address const& a) noexcept;

} // urls
} // boost

#include <boost/url/impl/ipv6_address.hpp>

#endif
