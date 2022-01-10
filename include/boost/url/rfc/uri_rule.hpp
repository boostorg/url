//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_URI_RULE_HPP
#define BOOST_URL_RFC_URI_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/rfc/fragment_rule.hpp>
#include <boost/url/rfc/hier_part_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/rfc/scheme_rule.hpp>
#include <boost/url/grammar/parse_tag.hpp>

namespace boost {
namespace urls {

/** Rule for URI

    @par BNF
    @code
    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3"
        >3. Syntax Components (rfc3986)</a>

    @see
        @ref fragment_part_rule,
        @ref hier_part_rule,
        @ref query_part_rule,
        @ref scheme_part_rule.
*/
struct uri_rule
{
    scheme_part_rule     scheme_part;
    hier_part_rule       hier_part;
    query_part_rule      query_part;
    fragment_part_rule   fragment_part;

    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        error_code& ec,
        uri_rule& t) noexcept
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
        uri_rule& t) noexcept;
};

} // urls
} // boost

#endif
