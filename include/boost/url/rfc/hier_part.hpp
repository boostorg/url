//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_HIER_PART_HPP
#define BOOST_URL_RFC_HIER_PART_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/bnf/range.hpp>
#include <boost/url/rfc/authority.hpp>
#include <boost/url/rfc/pct_encoded.hpp>
#include <boost/optional.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** BNF for hier-part

    @par BNF
    @code
    hier-part     = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty

    path          = path-abempty    ; begins with "/" or is empty
                  / path-absolute   ; begins with "/" but not "//"
                  / path-noscheme   ; begins with a non-colon segment
                  / path-rootless   ; begins with a segment
                  / path-empty      ; zero characters

    path-abempty  = *( "/" segment )
    path-absolute = "/" [ segment-nz *( "/" segment ) ]
    path-noscheme = segment-nz-nc *( "/" segment )
    path-rootless = segment-nz *( "/" segment )
    path-empty    = 0<pchar>

    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3
*/
class hier_part
{
    using path_type =
        bnf::range<pct_encoded_value>;
    path_type v_;
    optional<rfc::authority> a_;

public:
    optional<
        rfc::authority> const&
    authority() const noexcept
    {
        return a_;
    }

    bnf::range<
        pct_encoded_value> const&
    path() const noexcept
    {
        return v_;
    }

    BOOST_URL_DECL
    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        hier_part& t);
};

} // rfc
} // urls
} // boost

#endif
