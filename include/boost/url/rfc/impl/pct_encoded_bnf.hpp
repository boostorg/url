//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODED_BNF_HPP
#define BOOST_URL_IMPL_PCT_ENCODED_BNF_HPP

#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/detail/pct_encoding.hpp>

namespace boost {
namespace urls {

namespace detail {

template<class CharSet>
struct pct_encoded_bnf
{
    CharSet const& cs_;
    pct_encoded_str& s_;

    pct_encoded_bnf(
        CharSet const& cs,
        pct_encoded_str& s) noexcept
        : cs_(cs)
        , s_(s)
    {
    }

    template<class CharSet_>
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_bnf<CharSet_> const& t) noexcept;
};

template<class CharSet>
bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    pct_encoded_bnf<CharSet> const& t) noexcept
{
    auto const start = it;
    // VFALCO TODO
    // opt.plus_to_space?
    if(! detail::parse_pct_encoded(
        it, end, ec, t.cs_,
            t.s_.decoded_size))
        return false;
    t.s_.str = string_view(
        start, it - start);
    return true;
}

} // detail

template<class CharSet>
detail::pct_encoded_bnf<CharSet>
pct_encoded_bnf(
    CharSet const& cs,
    pct_encoded_str& t)
{
    return detail::pct_encoded_bnf<
        CharSet>(cs, t);
}

} // urls
} // boost

#endif
