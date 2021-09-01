//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_LITERAL_HPP
#define BOOST_URL_BNF_LITERAL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>

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

/** BNF for literal string
*/
template<char C0, char...Cn>
class literal
{
public:
    using value_type = void;

    value_type
    value() const noexcept
    {
        return;
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec) noexcept
    {
        return detail::parse_literal<
            C0, Cn...>(start, end, ec);
    }
};

} // bnf
} // urls
} // boost

#endif
