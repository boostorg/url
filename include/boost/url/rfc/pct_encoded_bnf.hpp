//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PCT_ENCODED_BNF_HPP
#define BOOST_URL_RFC_PCT_ENCODED_BNF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/pct_encoding.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/detail/char_set.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

/** BNF for a string of percent-encoded characters from a character set

    @tparam CharSet The character set indicating
    which characters are unreserved. Any character
    not in this set will be considered reserved.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>
*/
template<class CharSet>
struct pct_encoded_bnf
{
    pct_encoded_str& v;

    BOOST_STATIC_ASSERT(
        bnf::is_char_set<
            CharSet>::value);

    template<class CharSet_>
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_bnf<CharSet_> const& t) noexcept;
};

} // urls
} // boost

#include <boost/url/rfc/impl/pct_encoded_bnf.hpp>

#endif
