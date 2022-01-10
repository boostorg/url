//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_AUTHORITY_RULE_HPP
#define BOOST_URL_RFC_AUTHORITY_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/rfc/host_rule.hpp>
#include <boost/url/rfc/port_rule.hpp>
#include <boost/url/rfc/userinfo_rule.hpp>
#include <boost/url/grammar/parse_tag.hpp>

namespace boost {
namespace urls {

/** BNF for authority

    @par BNF
    @code
    authority   = [ userinfo "@" ] host [ ":" port ]
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2"
        >3.2. Authority (rfc3986)</a>

    @see
        @ref host_rule,
        @ref port_part_rule,
        @ref userinfo_rule.
*/
struct authority_rule
{
    // userinfo
    bool has_userinfo = false;
    userinfo_rule userinfo;

    // host
    host_rule host;

    // port
    port_part_rule port;

    BOOST_URL_DECL
    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        error_code& ec,
        authority_rule& t) noexcept;
};

} // urls
} // boost

#endif
