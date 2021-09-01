//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PCT_ENCODED_IPP
#define BOOST_URL_RFC_IMPL_PCT_ENCODED_IPP

#include <boost/url/rfc/pct_encoded.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/char_sets.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
pct_encoded::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    auto it = start;
    if(it == end)
    {
        ec = error::need_more;
        return start;
    }
    if(*it != '%')
    {
        ec = error::syntax;
        return start;
    }
    ++it;
    if(it == end)
    {
        ec = error::need_more;
        return start;
    }
    if(! bnf::is_hexdig(*it))
    {
        ec = error::syntax;
        return start;
    }
    ++it;
    if(it == end)
    {
        ec = error::need_more;
        return start;
    }
    if(! bnf::is_hexdig(*it))
    {
        ec = error::syntax;
        return start;
    }
    ++it;
    return it;
}

} // rfc
} // urls
} // boost

#endif
