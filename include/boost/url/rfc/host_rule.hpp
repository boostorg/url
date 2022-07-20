//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_HOST_RULE_HPP
#define BOOST_URL_RFC_HOST_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/host_type.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/ipv4_address.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/grammar/parse_tag.hpp>

namespace boost {
namespace urls {

/** Rule for host

    @par BNF
    @code
    host          = IP-literal / IPv4address / reg-name
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
        >3.2.2. Host (rfc3986)</a>

    @see
        @ref host_type,
        @ref ipv4_address,
        @ref ipv6_address.
*/
struct host_rule
{
    urls::host_type host_type =
        urls::host_type::none;
    pct_encoded_view name;
    ipv4_address ipv4;
    ipv6_address ipv6;
    string_view ipvfuture;
    string_view host_part;

    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        error_code& ec,
        host_rule& t) noexcept
    {
        return parse(it, end, ec, t);
    }

private:
    BOOST_URL_DECL
    static
    void
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        host_rule& t) noexcept;
};

} // urls
} // boost

#endif
