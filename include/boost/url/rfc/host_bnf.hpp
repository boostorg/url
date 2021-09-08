//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_HOST_BNF_HPP
#define BOOST_URL_HOST_BNF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/host_type.hpp>
#include <boost/url/rfc/ipv4_address_bnf.hpp>
#include <boost/url/rfc/ipv6_address_bnf.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

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
    string_view str_;
    pct_encoded_str name_;
    ipv4_address ipv4_;
    ipv6_address ipv6_;
    string_view ipvfuture_;
    urls::host_type host_type_ =
        urls::host_type::none;

public:
    BOOST_URL_DECL
    ~host_bnf() noexcept;

    BOOST_URL_DECL
    host_bnf() noexcept;

    string_view
    str() const noexcept
    {
        return str_;
    }

    urls::host_type
    host_type() const noexcept
    {
        return host_type_;
    }

    pct_encoded_str
    get_name() const noexcept
    {
        BOOST_ASSERT(host_type_ ==
            urls::host_type::name);
        return name_;
    }

    ipv4_address const&
    get_ipv4() const noexcept
    {
        BOOST_ASSERT(host_type_ ==
            urls::host_type::ipv4);
        return ipv4_;
    }

    ipv6_address const&
    get_ipv6() const noexcept
    {
        BOOST_ASSERT(host_type_ ==
            urls::host_type::ipv6);
        return ipv6_;
    }

    string_view
    get_ipv_future() const noexcept
    {
        BOOST_ASSERT(host_type_ ==
            urls::host_type::ipvfuture);
        return ipvfuture_;
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

} // urls
} // boost

#endif
