//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_FRAGMENT_BNF_IPP
#define BOOST_URL_RFC_IMPL_FRAGMENT_BNF_IPP

#include <boost/url/rfc/fragment_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>

namespace boost {
namespace urls {
namespace rfc {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    fragment_bnf const& t)
{
    using bnf::parse;
    return parse(it, end, ec,
        pct_encoded_bnf<
            pchar_mask |
            slash_char_mask |
            question_char_mask>{t.v});
}

} // rfc
} // urls
} // boost

#endif
