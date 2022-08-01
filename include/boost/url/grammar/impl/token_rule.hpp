//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_URL_IMPL_GRAMMAR_TOKEN_RULE_HPP
#define BOOST_URL_IMPL_GRAMMAR_TOKEN_RULE_HPP

#include <boost/url/grammar/error.hpp>

namespace boost {
namespace urls {
namespace grammar {

template<class CharSet>
auto
token_rule_t<CharSet>::
parse(
    char const*& it,
    char const* end
        ) const noexcept
    -> result<value_type>
{
    auto const it0 = it;
    if(it == end)
        return error::incomplete;
    it = (find_if_not)(it, end, cs_);
    if(it == it0)
        return error::syntax;
    return string_view(it0, it - it0);
}

} // grammar
} // urls
} // boost

#endif
