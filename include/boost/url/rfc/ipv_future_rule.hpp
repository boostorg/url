//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IPV_FUTURE_RULE_HPP
#define BOOST_URL_RFC_IPV_FUTURE_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/parse_tag.hpp>

namespace boost {
namespace urls {

/** BNF for IPvFuture

    @par BNF
    @code
    IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
        >3.2.2. Host (rfc3986)</a>
*/
struct ipv_future_rule
{
    string_view str;
    string_view major;
    string_view minor;

    BOOST_URL_DECL
    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        error_code& ec,
        ipv_future_rule& t) noexcept;
};

} // urls
} // boost

#endif
