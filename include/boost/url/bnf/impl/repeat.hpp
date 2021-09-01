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
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace bnf {

template<
    class Element,
    std::size_t N,
    std::size_t M>
char const*
repeat<Element, N, M>::
begin(
    char const* start,
    char const* end,
    error_code& ec)
{
    n_ = 0;
    return increment(
        start, end, ec);
}

template<
    class Element,
    std::size_t N,
    std::size_t M>
char const*
repeat<Element, N, M>::
increment(
    char const* start,
    char const* end,
    error_code& ec)
{
    if(start == end)
    {
        if(n_ < N)
        {
            ec = error::need_more;
            return start;
        }
        ec = error::end;
        return start;
    }
    auto it = element_.parse(
        start, end, ec);
    if(ec == error::need_more)
        return it;
    if(! ec)
    {
        ++n_;
        if(n_ <= M)
            return it;
        // treat as end
        ec = error::end;
        return start;
    }
    if(n_ >= N)
    {
        // treat as end
        ec = error::end;
        return it;
    }
    // too few elements
    ec = error::syntax;
    return it;
}

} // bnf
} // urls
} // boost

#endif
