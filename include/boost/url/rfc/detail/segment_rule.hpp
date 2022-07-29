//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_DETAIL_SEGMENT_RULE_HPP
#define BOOST_URL_RFC_DETAIL_SEGMENT_RULE_HPP

#include <boost/url/grammar/not_empty_rule.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>

namespace boost {
namespace urls {
namespace detail {

/** Rule for segment

    @par BNF
    @code
    segment       = *pchar
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref path_abempty_rule,
        @ref path_absolute_rule,
        @ref path_noscheme_rule,
        @ref path_rootless_rule,
        @ref pchars,
        @ref segment_nz_rule,
        @ref segment_nz_nc_rule
*/
constexpr auto segment_rule =
    pct_encoded_rule(pchars);

//------------------------------------------------

/** Rule for segment-nz

    @par BNF
    @code
    segment-nz    = 1*pchar
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref path_abempty_rule,
        @ref path_absolute_rule,
        @ref path_noscheme_rule,
        @ref path_rootless_rule,
        @ref pchars,
        @ref segment_rule,
        @ref segment_nz_nc_rule
*/
constexpr auto segment_nz_rule =
    grammar::not_empty_rule(
        pct_encoded_rule(pchars));

//------------------------------------------------

/** Rule for segment_nz_nc

    @par BNF
    @code
    segment-nz-nc   = 1*( unreserved / pct-encoded / sub-delims / "@" )
                    ; non-zero-length segment without any colon ":"
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref path_abempty_rule,
        @ref path_absolute_rule,
        @ref path_noscheme_rule,
        @ref path_rootless_rule,
        @ref pchars,
        @ref segment_rule,
        @ref segment_nz_rule,
        @ref subdelim_chars,
        @ref unreserved_chars
*/
constexpr auto segment_nz_nc_rule =
    grammar::not_empty_rule(
        pct_encoded_rule(pchars - ':'));

} // detail
} // urls
} // boost

#endif
