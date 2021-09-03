//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_IPV_FUTURE_IPP
#define BOOST_URL_BNF_IMPL_IPV_FUTURE_IPP

#include <boost/url/rfc/ipv_future.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/hexdig.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    ipv_future& t)
{
    using bnf::parse;
    bnf::token<
        bnf::char_set_function<
            &is_hexdig>> v0;
    bnf::token<
        masked_char_set<
            unreserved_char_mask |
            sub_delims_char_mask |
            colon_char_mask>> v1;
    auto it = parse(start, end, ec,
        'v', v0, '.', v1);
    if(ec)
        return start;
    if(v0.str().empty())
    {
        // can't be empty
        ec = error::syntax;
        return start;
    }
    if(v1.str().empty())
    {
        // can't be empty
        ec = error::syntax;
        return start;
    }
    t.s_ = string_view(
        start, it - start);
    return it;
}

} // rfc
} // urls
} // boost

#endif
