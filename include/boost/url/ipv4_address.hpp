//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_IPV4_ADDRESS_HPP
#define BOOST_URL_IPV4_ADDRESS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <array>
#include <cstdint>
#include <iosfwd>

namespace boost {
namespace urls {

class ipv4_address
{
public:
    /** The number of characters in the longest possible ipv4 string
    */
    static
    constexpr
    std::size_t max_str_len = 15;

    /** The type used to represent an address as an unsigned integer
    */
    using uint_type =
        std::uint_least32_t;

    /** The type used to represent an address as an array of bytes
    */
    using bytes_type =
        std::array<unsigned char, 4>;

    /** Constructor
    */
    ipv4_address() noexcept
        : addr_(0)
    {
    }

    /** Construct from raw bytes
    */
    BOOST_URL_DECL
    explicit
    ipv4_address(
        bytes_type const & bytes);

    /** Construct from an unsigned integer in host byte order
    */
    BOOST_URL_DECL
    explicit
    ipv4_address(uint_type addr);

    /** Constructor
    */
    ipv4_address(
        ipv4_address const& other) noexcept
        : addr_(other.addr_)
    {
    }

    /** Assign from another address
    */
    ipv4_address&
    operator=(
        ipv4_address const& other) noexcept
    {
        addr_ = other.addr_;
        return *this;
    }

    /** Return the address as bytes, in network byte order
    */
    BOOST_URL_DECL
    bytes_type
    to_bytes() const noexcept;

    /** Return the address as an unsigned integer in host byte order
    */
    BOOST_URL_DECL
    uint_type
    to_uint() const noexcept;

    /** Return the address as a string in dotted decimal format

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

    /** Return true if the address is a loopback address
    */
    BOOST_URL_DECL
    bool
    is_loopback() const noexcept;

    /** Return true if the address is unspecified
    */
    BOOST_URL_DECL
    bool
    is_unspecified() const noexcept;

    /** Return true if the address is a multicast address
    */
    BOOST_URL_DECL
    bool
    is_multicast() const noexcept;

    /** Return true if two addresses are equal
    */
    friend
    bool
    operator==(
        ipv4_address const& a1,
        ipv4_address const& a2) noexcept
    {
        return a1.addr_ == a2.addr_;
    }

    /** Return true if two addresses are not equal
    */
    friend
    bool
    operator!=(
        ipv4_address const& a1,
        ipv4_address const& a2) noexcept
    {
        return a1.addr_ != a2.addr_;
    }

    /** Return an address object that represents any address
    */
    static
    ipv4_address
    any() noexcept
    {
        return ipv4_address();
    }

    /** Return an address object that represents the loopback address
    */
    static
    ipv4_address
    loopback() noexcept
    {
        return ipv4_address(0x7F000001);
    }

    /** Return an address object that represents the broadcast address
    */
    static
    ipv4_address
    broadcast() noexcept
    {
        return ipv4_address(0xFFFFFFFF);
    }

    BOOST_URL_DECL
    friend
    std::ostream&
    operator<<(
        std::ostream&,
        ipv4_address const&);

private:
    BOOST_URL_DECL
    std::size_t
    print_impl(
        char* dest) const noexcept;

    // network order
    uint_type addr_;
};

#if 0
/** Format the ipv4_address to an output stream
*/
BOOST_URL_DECL
std::ostream&
operator<<(
    std::ostream&,
    ipv4_address const&);
#endif

} // urls
} // boost

#include <boost/url/impl/ipv4_address.hpp>

#endif
