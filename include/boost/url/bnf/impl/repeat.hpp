//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_REPEAT_HPP
#define BOOST_URL_BNF_IMPL_REPEAT_HPP

#include <boost/url/error.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace bnf {

namespace detail {

template<class T>
char const*
parse_repeat(
    char const* const start,
    char const* const end,
    error_code& ec,
    std::size_t N,
    std::size_t M,
    std::size_t& n)
{
    T v;
    auto it = start;
    n = 0;
    for(;;)
    {
        it = parse(
            it, end, ec, v);
        if(ec)
            break;
        ++n;
        if(n == M)
            break;
    }
    if(n < N)
    {
        // too few
        ec = error::syntax;
        return start;
    }
    return it;
}

} // detail

template<
    class T,
    std::size_t N,
    std::size_t M>
char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    repeat<T, N, M> const& t)
{
    std::size_t n;
    auto it = detail::parse_repeat(
        start, end, ec, N, M, n);
    if(ec)
        return start;
    t.v = string_view(
        start, it - start);
    return it;
}

} // bnf
} // urls
} // boost

#endif
