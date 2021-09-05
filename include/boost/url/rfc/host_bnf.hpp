//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_HOST_BNF_HPP
#define BOOST_URL_RFC_HOST_BNF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/rfc/ipv4_address_bnf.hpp>
#include <boost/url/rfc/ipv6_address_bnf.hpp>
#include <boost/url/rfc/pct_encoded_str.hpp>
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
    domain
};

/** BNF for host

    @par BNF
    @code
    host          = IP-literal / IPv4address / reg-name
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2
*/
class host_bnf
{
    host_kind kind_ =
        host_kind::none;
    string_view s_;

    union
    {
        ipv4_address_bnf ipv4_;
        ipv6_address_bnf ipv6_;
        pct_encoded_str domain_{};
        string_view fut_;
    };

    void destroy();

public:
    host_bnf()
    {
    }

    BOOST_URL_DECL
    ~host_bnf();

    string_view
    str() const noexcept
    {
        return s_;
    }

    host_kind
    kind() const noexcept
    {
        return kind_;
    }

    ipv4_address_bnf const&
    get_ipv4() const noexcept
    {
        BOOST_ASSERT(kind_ ==
            host_kind::ipv4);
        return ipv4_;
    }

    ipv6_address_bnf const&
    get_ipv6() const noexcept
    {
        BOOST_ASSERT(kind_ ==
            host_kind::ipv6);
        return ipv6_;
    }

    string_view
    get_ipv_future() const noexcept
    {
        BOOST_ASSERT(kind_ ==
            host_kind::ipv_future);
        return fut_;
    }

    pct_encoded_str
    get_domain() const noexcept
    {
        BOOST_ASSERT(kind_ ==
            host_kind::domain);
        return domain_;
    }

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        host_bnf& t);
};

} // rfc
} // urls
} // boost

#endif
