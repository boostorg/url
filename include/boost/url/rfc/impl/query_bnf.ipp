//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_QUERY_BNF_IPP
#define BOOST_URL_IMPL_QUERY_BNF_IPP

#include <boost/url/rfc/query_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <boost/url/rfc/query_params_bnf.hpp>

namespace boost {
namespace urls {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    query_bnf& t)
{
    bnf::range<query_param> r;
    if(! bnf::parse_range(
        it, end, ec, r,
            query_params_bnf{}))
        return false;
    t.query = r.str();
    t.query_count = r.size();
    return true;
}

} // urls
} // boost

#endif
