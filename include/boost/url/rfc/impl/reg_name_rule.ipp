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
bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    reg_name_rule const& t)
{
    return parse(
        it, end, ec,
        pct_encoded_rule(
            reg_name_chars, t.v));
}

} // urls
} // boost

#endif
