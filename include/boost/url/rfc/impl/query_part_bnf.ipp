//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_QUERY_PART_BNF_IPP
#define BOOST_URL_IMPL_QUERY_PART_BNF_IPP

#include <boost/url/rfc/query_part_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/query_bnf.hpp>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    query_part_bnf& t)
{
    using bnf::parse;
    auto start = it;
    if( it == end ||
        *it != '?')
    {
        ec = {};
        t.has_query = false;
        return true;
    }
    ++it;
    if(! parse(it, end, ec, t.query))
        return false;
    t.has_query = true;
    t.query_part = string_view(
        start, it - start);
    return true;
}

} // urls
} // boost

#endif
