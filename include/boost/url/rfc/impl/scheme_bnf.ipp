//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SCHEME_BNF_IPP
#define BOOST_URL_IMPL_SCHEME_BNF_IPP

#include <boost/url/rfc/scheme_bnf.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/bnf/repeat.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    scheme_bnf& t)
{
    using namespace bnf;
    auto const start = it;
    if(it == end)
    {
        // expected alpha
        ec = error::syntax;
        return false;
    }
    if(! alpha_chars{}(*it))
    {
        // expected alpha
        ec = error::syntax;
        return false;
    }
    it = find_if_not(
        it + 1, end,
            alnum_chars{});
    t.str = string_view(
        start, it - start);
    t.id = string_to_scheme(
        t.str);
    return true;
}

} // urls
} // boost

#endif
