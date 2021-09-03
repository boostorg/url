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
char const*
parse_literal(
    char const* const start,
    char const* const end,
    error_code& ec) noexcept
{
    if(start == end)
    {
        ec = error::need_more;
        return start;
    }
    if(*start != Ch)
    {
        ec = error::syntax;
        return start;
    }
    return start + 1;
}

template<char C0, char C1, char...Cn>
char const*
parse_literal(
    char const* const start,
    char const* const end,
    error_code& ec) noexcept
{
    auto it = parse_literal<C0>(
        start, end, ec);
    if(ec)
        return start;
    return parse_literal<
        C1, Cn...>(start, end, ec);
}

} // detail

template<char... Cn>
char const*
literal<Cn...>::
parse(
    char const* const start,
    char const* const end,
    error_code& ec) noexcept
{
    auto it = detail::parse_literal<
        Cn...>(start, end, ec);
    if(ec)
        return start;
    s_ = string_view(
        start, it - start);
    return it;
}

template<char...Cn>
char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    literal<Cn...> const&) noexcept
{
    auto it = detail::parse_literal<
        Cn...>(start, end, ec);
    if(ec)
        return start;
    return it;
}

} // bnf
} // urls
} // boost

#endif
