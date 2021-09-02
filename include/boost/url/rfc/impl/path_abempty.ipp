//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PATH_ABEMPTY_IPP
#define BOOST_URL_RFC_IMPL_PATH_ABEMPTY_IPP

#include <boost/url/rfc/path_abempty.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/literal.hpp>
#include <boost/url/bnf/sequence.hpp>
#include <boost/url/rfc/segment.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
path_abempty::
increment(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    using namespace bnf;
    sequence<
        literal<'/'>,
        segment> p;
    auto it = p.parse(
        start, end, ec);
    if(ec)
    {
        v_ = {};
        ec = error::end;
        return it;
    }
    v_ = string_view(
        start, it - start);
    return it;
}

} // rfc
} // urls
} // boost

#endif
