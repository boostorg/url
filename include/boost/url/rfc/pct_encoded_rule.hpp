//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PCT_ENCODED_RULE_HPP
#define BOOST_URL_RFC_PCT_ENCODED_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/parse_tag.hpp>
#include <boost/static_assert.hpp>

#include <boost/url/pct_encoding.hpp>
#include <boost/url/pct_encoding_types.hpp>
#include <boost/url/grammar/detail/charset.hpp>

namespace boost {
namespace urls {

/** Rule for a string of percent-encoded characters from a character set

    @tparam CharSet The character set indicating
    which characters are unreserved. Any character
    not in this set will be considered reserved.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>
*/
template<class CharSet>
struct pct_encoded_rule
{
    pct_encoded_str s;

    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        pct_encoded_rule& t) noexcept
    {
        parse(it, end, ec, t);
    }

private:
    static
    void
    parse(
        char const*& it,
        char const* end,
        error_code& ec,
        pct_encoded_rule& t) noexcept;
};

} // urls
} // boost

#include <boost/url/rfc/impl/pct_encoded_rule.hpp>

#endif
