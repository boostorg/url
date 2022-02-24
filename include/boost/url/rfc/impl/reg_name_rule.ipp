//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_REG_NAME_RULE_IPP
#define BOOST_URL_IMPL_REG_NAME_RULE_IPP

#include <boost/url/rfc/reg_name_rule.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/grammar/lut_chars.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

/*  VFALCO In theory we should enforce these
    additional requirements from errata 4942:

    Such a name consists of a sequence of domain
    labels separated by ".", each domain label
    starting and ending with an alphanumeric character
    and possibly also containing "-" characters.  The
    rightmost domain label of a fully qualified domain
    name in DNS may be followed by a single "." and
    should be if it is necessary to distinguish between
    the complete domain name and some local domain.
*/
void
reg_name_rule::
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    reg_name_rule& t) noexcept
{
    struct reg_name_chars
        : grammar::lut_chars
    {
        constexpr
        reg_name_chars() noexcept
            : lut_chars(
                unreserved_chars
                + '-' + '.')
        {
        }
    };

    pct_encoded_rule<
        reg_name_chars> t0;
    grammar::parse(
        it, end, ec, t0);
    t.v = t0.s;
}

} // urls
} // boost

#endif
