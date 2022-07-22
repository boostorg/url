//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_ABSOLUTE_URI_RULE_HPP
#define BOOST_URL_RFC_ABSOLUTE_URI_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/result.hpp>
#include <boost/url/rfc/hier_part_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/rfc/scheme_rule.hpp>

namespace boost {
namespace urls {

/** Rule for absolute-URI

    @par BNF
    @code
    absolute-URI    = scheme ":" hier-part [ "?" query ]

    hier-part       = "//" authority path-abempty
                    / path-absolute
                    / path-rootless
                    / path-empty
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.3"
        >4.3. Absolute URI (rfc3986)</a>

    @see
        @ref hier_part_rule,
        @ref query_part_rule,
        @ref scheme_part_rule.
*/
struct absolute_uri_rule_t
{
    struct value_type
    {
        scheme_part_rule::value_type scheme_part;
        decltype(hier_part_rule)::value_type hier_part;
        decltype(query_part_rule)::value_type  query_part;
    };

    BOOST_URL_DECL
    auto
    parse(
        char const*& it,
        char const* end
            ) const noexcept ->
        result<value_type>;
};

constexpr absolute_uri_rule_t absolute_uri_rule{};

} // urls
} // boost

#endif
