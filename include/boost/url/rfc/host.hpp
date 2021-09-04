//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_HOST_HPP
#define BOOST_URL_RFC_HOST_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/rfc/ipv4_address.hpp>
#include <boost/url/rfc/ipv6_address.hpp>
#include <boost/url/rfc/pct_encoded.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** The kind of host
*/
enum class host_kind
{
    none,
    ipv4,
    ipv6,
    ipv_future,
    named
};

/** BNF for host

    @par BNF
    @code
    host          = IP-literal / IPv4address / reg-name
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2
*/
class host
{
    host_kind kind_ =
        host_kind::none;

    union
    {
        ipv4_address ipv4_;
        ipv6_address ipv6_;
        pct_encoded_str name_{};
    };

    void destroy();

public:
    host()
    {
    }

    BOOST_URL_DECL
    ~host();

    host_kind
    kind() const noexcept
    {
        return kind_;
    }

    ipv4_address const&
    get_ipv4() const noexcept
    {
        BOOST_ASSERT(kind_ ==
            host_kind::named);
        return ipv4_;
    }

    ipv6_address const&
    get_ipv6() const noexcept
    {
        BOOST_ASSERT(kind_ ==
            host_kind::ipv6);
        return ipv6_;
    }

    pct_encoded_str
    get_name() const noexcept
    {
        BOOST_ASSERT(kind_ ==
            host_kind::named);
        return name_;
    }

    BOOST_URL_DECL
    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        host& t);
};

} // rfc
} // urls
} // boost

#endif
