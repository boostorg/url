//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_LITERAL_HPP
#define BOOST_URL_BNF_IMPL_LITERAL_HPP

namespace boost {
namespace urls {
namespace bnf {

namespace detail {

template<char Ch>
bool
parse_literal(
    char const*& it,
    char const* const end,
    error_code& ec) noexcept
{
    if(it == end)
    {
        // end of input
        ec = error::syntax;
        return false;
    }
    if(*it != Ch)
    {
        // expected <Ch>
        ec = error::syntax;
        return false;
    }
    ++it;
    ec = {};
    return true;
}

template<
    char C0, char C1, char...Cn>
bool
parse_literal(
    char const*& it,
    char const* const end,
    error_code& ec) noexcept
{
    if(! parse_literal<C0>(
        it, end, ec))
        return false;
    return parse_literal<
        C1, Cn...>(it, end, ec);
}

} // detail

template<char...Cn>
bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    literal<Cn...> const& t) noexcept
{
    auto start = it;
    if(! detail::parse_literal<
        Cn...>(it, end, ec))
        return false;
    if(t.v)
        *t.v = string_view(
            start, it - start);
    return true;
}

} // bnf
} // urls
} // boost

#endif
