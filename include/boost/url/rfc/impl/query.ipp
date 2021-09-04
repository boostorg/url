//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_QUERY_IPP
#define BOOST_URL_RFC_IMPL_QUERY_IPP

#include <boost/url/rfc/query.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/detail/paths.hpp>
#include <boost/url/rfc/char_sets.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
query::
begin(
    char const* const start,
    char const* const end,
    error_code& ec,
    query_param& t) noexcept
{
    return increment(
        start, end, ec, t);
}

char const*
query::
increment(
    char const* const start,
    char const* const end,
    error_code& ec,
    query_param& t) noexcept
{
    //pct_encoded<pchar
    return nullptr;
}

} // rfc
} // urls
} // boost

#endif
