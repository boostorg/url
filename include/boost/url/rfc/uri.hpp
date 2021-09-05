//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_URI_HPP
#define BOOST_URL_RFC_URI_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/bnf/range.hpp>
#include <boost/url/rfc/authority.hpp>
#include <boost/url/rfc/pct_encoded.hpp>
#include <boost/url/rfc/query_part.hpp>
#include <boost/url/rfc/scheme_part.hpp>
#include <boost/optional.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** BNF for URI

    @par BNF
    @code
    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

    hier-part     = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3
*/
struct uri
{
    scheme_part scheme;;
    bnf::range<pct_encoded_str> path;
    optional<rfc::authority> authority;
    optional<bnf::range<query_param>> query;
    optional<pct_encoded_str> fragment;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        uri& t);
};

} // rfc
} // urls
} // boost

#endif
