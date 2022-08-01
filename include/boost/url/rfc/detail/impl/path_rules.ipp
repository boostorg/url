//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_DETAIL_IMPL_PATH_RULES_IPP
#define BOOST_URL_RFC_DETAIL_IMPL_PATH_RULES_IPP

#include <boost/url/rfc/detail/path_rules.hpp>
#include <boost/url/rfc/path_rules.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {
namespace detail {

auto
segment_ns_rule_t::
parse(
    char const*& it,
    char const* end) const noexcept ->
        result<value_type>
{
    if(it == end)
        return value_type{};
    if(*it == '/')
        return grammar::error::syntax;
    auto rv = grammar::parse(
        it, end, segment_rule);
    if(rv.has_error())
        return rv.error();
    return *rv;
}

} // detail
} // urls
} // boost

#endif
