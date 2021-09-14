//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_QUERY_PARAMS_BNF_IPP
#define BOOST_URL_DETAIL_QUERY_PARAMS_BNF_IPP

#include <boost/url/rfc/query_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>

namespace boost {
namespace urls {

struct query_param
{
    pct_encoded_str key;
    pct_encoded_str value;
    bool has_value = false;
};

struct query_params_bnf
{
    BOOST_URL_DECL
    static
    bool
    begin(
        char const*& it,
        char const* const end,
        error_code& ec,
        query_param& t) noexcept;

    BOOST_URL_DECL
    static
    bool
    increment(
        char const*& it,
        char const* const end,
        error_code& ec,
        query_param& t) noexcept;
};

} // urls
} // boost

#endif
