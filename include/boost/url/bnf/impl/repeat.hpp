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
parse(
    char const* start,
    char const* end,
    error_code& ec)
{
    auto it = begin(
        start, end, ec);
    if(ec == error::end)
    {
        ec = {};
        return start;
    }
    for(;;)
    {
        it = increment(
            it, end, ec);
        if(ec == error::end)
            break;
        if(ec)
            return start;
    }
    ec = {};
    return it;
}

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
    v_.clear();
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
    auto it = e_.parse(
        start, end, ec);
    if(ec == error::need_more)
        return start;
    if(! ec)
    {
        if(n_ < v_.capacity())
        {
            BOOST_ASSERT(
                v_.size() == n_);
            v_.push_back(*e_);
        }
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
        return start;
    }
    // too few elements
    ec = error::syntax;
    return start;
}

} // bnf
} // urls
} // boost

#endif
