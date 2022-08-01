//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IPV6_ADDRESS_RULE_HPP
#define BOOST_URL_RFC_IPV6_ADDRESS_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/result.hpp>

namespace boost {
namespace urls {

/** Rule for An IP version 6 style address

    @par BNF
    @code
    IPv6address =                            6( h16 ":" ) ls32
                /                       "::" 5( h16 ":" ) ls32
                / [               h16 ] "::" 4( h16 ":" ) ls32
                / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
                / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
                / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
                / [ *4( h16 ":" ) h16 ] "::"              ls32
                / [ *5( h16 ":" ) h16 ] "::"              h16
                / [ *6( h16 ":" ) h16 ] "::"

    ls32        = ( h16 ":" h16 ) / IPv4address
                ; least-significant 32 bits of address

    h16         = 1*4HEXDIG
                ; 16 bits of address represented in hexadecimal
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc4291"
        >IP Version 6 Addressing Architecture (rfc4291)</a>
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
        >3.2.2. Host (rfc3986)</a>
*/
#ifdef BOOST_URL_DOCS
constexpr __implementation_defined__ ipv6_address_rule;
#else
struct ipv6_address_rule_t
{
    using value_type =
        ipv6_address;

    BOOST_URL_DECL
    auto
    parse(
        char const*& it,
        char const* end
            ) const noexcept ->
        result<ipv6_address>;
};

constexpr ipv6_address_rule_t ipv6_address_rule{};
#endif

} // urls
} // boost

#endif
