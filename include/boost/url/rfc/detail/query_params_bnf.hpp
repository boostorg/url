//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_DETAIL_QUERY_PARAMS_BNF_IPP
#define BOOST_URL_RFC_DETAIL_QUERY_PARAMS_BNF_IPP

#include <boost/url/rfc/query_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>

namespace boost {
namespace urls {
namespace rfc {
namespace detail {

struct query_params_bnf
{
    static
    bool
    begin(
        char const*& it,
        char const* const end,
        error_code& ec,
        query_param& t) noexcept
    {
        using bnf::parse;
        // key
        if(! parse(it, end, ec,
            pct_encoded_bnf<
                qpchar_mask>{t.key}))
            return false;
        // "="
        if(! parse(it, end, ec, '='))
        {
            // key with no value
            ec = {};
            t.value.reset();
            return true;
        }
        // value
        t.value.emplace();
        if(! parse(it, end, ec,
            pct_encoded_bnf<
                qpchar_mask |
                equals_char_mask>{*t.value}))
        {
            ec = {};
            t.value.reset();
            return true;
        }
        return true;
    }

    static
    bool
    increment(
        char const*& it,
        char const* const end,
        error_code& ec,
        query_param& t) noexcept
    {
        using bnf::parse;
        if(! parse(it, end, ec, '&'))
        {
            // end of list
            ec = error::end;
            return false;
        }
        // key
        if(! parse(it, end, ec,
            pct_encoded_bnf<
                qpchar_mask>{t.key}))
            return false;
        // "="
        if(! parse(it, end, ec, '='))
        {
            // key with no value
            ec = {};
            t.value.reset();
            return true;
        }
        // value
        t.value.emplace();
        if(! parse(it, end, ec,
            pct_encoded_bnf<
                qpchar_mask |
                equals_char_mask>{*t.value}))
        {
            ec = {};
            t.value.reset();
            return true;
        }
        return true;
    }
};

} // detail
} // rfc
} // urls
} // boost

#endif
