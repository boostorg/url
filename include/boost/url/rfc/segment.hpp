//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_SEGMENT_HPP
#define BOOST_URL_RFC_SEGMENT_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/pchar.hpp>
#include <boost/url/bnf/repeat.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** BNF for segment

    @par BNF
    @code
    segment       = *pchar

    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
    pct-encoded   = "%" HEXDIG HEXDIG
    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
using segment = bnf::zero_or_more<pchar>;

/** BNF for segment

    @par BNF
    @code
    segment-nz    = 1*pchar

    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
    pct-encoded   = "%" HEXDIG HEXDIG
    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
using segment_nz = bnf::one_or_more<pchar>;

/** BNF for segment

    @par BNF
    @code
    segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
                  ; non-zero-length segment without any colon ":"

    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
    pct-encoded   = "%" HEXDIG HEXDIG
    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
class segment_nz_nc
{
public:
    using value_type = string_view;

    value_type
    value() const noexcept
    {
        return v_;
    }

    BOOST_URL_DECL
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec);

private:
    class pchar_nc;

    value_type v_;
};

} // rfc
} // urls
} // boost

#endif
