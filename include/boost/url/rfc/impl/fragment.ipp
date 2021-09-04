//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_FRAGMENT_IPP
#define BOOST_URL_BNF_IMPL_FRAGMENT_IPP

#include <boost/url/rfc/uri.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/pct_encoded.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    fragment const& t)
{
    using bnf::parse;
    return parse(start, end, ec,
        pct_encoded<
            unsub_char_mask |
            colon_char_mask |
            at_char_mask>{t.v});
}

} // rfc
} // urls
} // boost

#endif
