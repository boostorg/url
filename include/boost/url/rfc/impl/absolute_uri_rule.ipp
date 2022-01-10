//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_ABSOLUTE_URI_RULE_IPP
#define BOOST_URL_IMPL_ABSOLUTE_URI_RULE_IPP

#include <boost/url/rfc/absolute_uri_rule.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

void
tag_invoke(
    grammar::parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    absolute_uri_rule& t) noexcept
{
    grammar::parse(
        it, end, ec,
        t.scheme_part,
        t.hier_part,
        t.query_part);
}

} // urls
} // boost

#endif
