//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IP_LITERAL_HPP
#define BOOST_URL_RFC_IP_LITERAL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/ipv6_address.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** BNF for IP-literal

    @par BNF
    @code
    IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2
*/
class ip_literal
{
    ipv6_address ipv6_;
    string_view s_;
    bool is_ipv6_ = false;

public:
    bool
    is_ipv6() const noexcept
    {
        return is_ipv6_;
    }

    ipv6_address const&
    get_ipv6_address() const noexcept
    {
        BOOST_ASSERT(is_ipv6_);
        return ipv6_;
    }

    string_view
    get_ipv_future() const noexcept
    {
        BOOST_ASSERT(! is_ipv6_);
        return s_;
    }

    BOOST_URL_DECL
    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        ip_literal& t);
};

} // rfc
} // urls
} // boost

#endif
