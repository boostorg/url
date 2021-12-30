//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_URI_REFERENCE_RULE_HPP
#define BOOST_URL_RFC_URI_REFERENCE_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/rfc/authority_rule.hpp>
#include <boost/url/rfc/fragment_rule.hpp>
#include <boost/url/rfc/paths_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/rfc/scheme_rule.hpp>

namespace boost {
namespace urls {

/** BNF for URI-reference

    @par BNF
    @code
    URI-reference = URI / relative-ref

    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3"
        >3. Syntax Components (rfc3986)</a>


    @see
        @ref authority_rule,
        @ref fragment_part_rule,
        @ref query_part_rule,
        @ref scheme_part_rule.
*/
struct uri_reference_rule
{
    scheme_part_rule     scheme_part;
    bool                has_authority = false;
    authority_rule       authority;
    parsed_path         path;
    query_part_rule      query_part;
    fragment_part_rule   fragment_part;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        uri_reference_rule& t);
};

} // urls
} // boost

#endif