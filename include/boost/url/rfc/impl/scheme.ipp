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
#include <boost/url/string.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/bnf/repeat.hpp>
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
    scheme const& t)
{
    using namespace bnf;
    if(start == end)
    {
        // expected alpha
        ec = error::syntax;
        return start;
    }
    auto it = start;
    if(! alpha_chars{}(*it))
    {
        // expected alpha
        ec = error::syntax;
        return start;
    }
    it = find_if_not(
        it + 1, end,
            alnum_chars{});
    t.s = string_view(
        start, it - start);
    return it;
}

} // rfc
} // urls
} // boost

#endif
