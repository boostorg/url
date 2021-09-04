//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_SCHEME_IPP
#define BOOST_URL_RFC_IMPL_SCHEME_IPP

#include <boost/url/rfc/scheme.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/bnf/token.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    scheme& t)
{
    auto it = start;
    if(it == end)
    {
        // expected alpha
        ec = error::syntax;
        return start;
    }
    if(! is_alpha(*it))
    {
        ec = error::syntax;
        return start;
    }
    bnf::token<
        masked_char_set<
            alnum_char_mask>> ct;
    using bnf::parse;
    it = parse(start, end, ec, ct);
    BOOST_ASSERT(it != start);
    t.s_ = ct.str();
    return it;
}

} // rfc
} // urls
} // boost

#endif
