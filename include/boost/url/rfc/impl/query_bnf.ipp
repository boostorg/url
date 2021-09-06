//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_QUERY_BNF_IPP
#define BOOST_URL_RFC_IMPL_QUERY_BNF_IPP

#include <boost/url/rfc/query_bnf.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>

namespace boost {
namespace urls {
namespace rfc {

namespace detail {

struct query_range
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
        pct_encoded_str k;
        auto const start = it;
        if(! parse(it, end, ec,
            pct_encoded_bnf<
                query_char_mask>{k}))
            return false;
        if(! parse(it, end, ec, '='))
        {
            if(it != start)
            {
                // key with no value
                ec = {};
                t.key = k;
                t.value.reset();
                return true;
            }
            // empty list
            ec = error::end;
            return false;
        }
        pct_encoded_str v;
        if(! parse(it, end, ec,
            pct_encoded_bnf<
                query_char_mask>{v}))
        {
            // VFALCO what about the key?
            return false;
        }
        t.key = k;
        t.value.emplace(v);
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
        auto it0 = it;
        if(! begin(it, end, ec, t))
        {
            if(ec == error::end)
            {
                // empty param
                ec = {};
                t.key = {};
                t.value.reset();
                return it0;
            }
            return false;
        }
        return true;
    }
};

} // detail

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    query_bnf const& t)
{
    using bnf::parse;
    return bnf::parse_range(
        it, end, ec, t.v,
            detail::query_range{});
}

} // rfc
} // urls
} // boost

#endif
