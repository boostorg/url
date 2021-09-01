//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PATH_ABSOLUTE_IPP
#define BOOST_URL_RFC_IMPL_PATH_ABSOLUTE_IPP

#include <boost/url/rfc/path_absolute.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/algorithm.hpp>
#include <boost/url/rfc/segment.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
path_absolute::
begin(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    if(start == end)
    {
        ec = error::syntax;
        return start;
    }
    auto it = start;
    if(*it != '/')
    {
        ec = error::syntax;
        return it;
    }
    ++it;
    if(it == end)
    {
        ec = {};
        v_ = string_view(
            start, it - start);
        return it;
    }
    if(*it == '/')
    {
        // can't begin with "//"
        ec = error::syntax;
        return it;
    }
    it = bnf::consume<
        segment_nz>(it, end, ec);
    v_ = string_view(
        start, it - start);
    return it;
}

char const*
path_absolute::
increment(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    if(start == end)
    {
        v_ = {};
        ec = error::end;
        return start;
    }
    auto it = start;
    if(*it != '/')
    {
        v_ = {};
        ec = error::end;
        return it;
    }
    ++it;
    it = bnf::consume<
        segment>(it, end, ec);
    v_ = string_view(
        start, it - start);
    return it;
}

} // rfc
} // urls
} // boost

#endif
