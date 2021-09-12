//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_RELATIVE_PART_BNF_IPP
#define BOOST_URL_IMPL_RELATIVE_PART_BNF_IPP

#include <boost/url/rfc/relative_part_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/detail/paths_bnf.hpp>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    relative_part_bnf& t)
{
    using bnf::parse;
    using detail::path_noscheme_bnf;
    using detail::path_absolute_bnf;
    using detail::path_abempty_bnf;
    if(it == end)
    {
        // path-empty
        ec = {};
        t.path = {};
        t.path_count = 0;
        t.has_authority = false;
        return true;
    }
    bnf::range<pct_encoded_str> r;
    if(it[0] != '/')
    {
        // path-noscheme
        if(! parse(it, end, ec,
            path_noscheme_bnf{r}))
            return false;
        t.path = r.str();
        t.path_count = r.size();
        t.has_authority = false;
        return true;
    }
    if( end - it == 1 ||
        it[1] != '/')
    {
        // path-absolute
        if(! parse(it, end, ec,
            path_absolute_bnf{r}))
            return false;
        t.path = r.str();
        t.path_count = r.size();
        t.has_authority = false;
        return true;
    }
    // "//" authority path-abempty
    it += 2;
    if(! parse(it, end, ec,
            t.authority))
        return false;
    if(! parse(it, end, ec,
        path_abempty_bnf{r}))
        return false;
    t.path = r.str();
    t.path_count = r.size();
    t.has_authority = true;
    return true;
}

} // urls
} // boost

#endif
