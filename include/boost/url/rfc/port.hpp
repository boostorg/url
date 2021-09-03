//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PORT_HPP
#define BOOST_URL_RFC_PORT_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** BNF for port

    @par BNF
    @code
    port          = *DIGIT
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2
*/
struct ipv4_address
{
    using value_type =
        std::array<std::uint8_t, 4>;

    value_type octets;

    BOOST_URL_DECL
    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        ipv4_address& t);
};

} // rfc
} // urls
} // boost

#endif
