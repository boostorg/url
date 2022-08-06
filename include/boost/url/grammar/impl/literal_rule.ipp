//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_IMPL_LITERAL_RULE_IPP
#define BOOST_URL_GRAMMAR_IMPL_LITERAL_RULE_IPP

#include <boost/url/grammar/literal_rule.hpp>
#include <boost/assert.hpp>
#include <cstring>

namespace boost {
namespace urls {
namespace grammar {

auto
literal_rule::
parse(
    char const*& it,
    char const* end) const noexcept ->
        result<value_type>
{
    BOOST_ASSERT(n_ > 0);
    std::size_t n = end - it;
    if(n < n_)
        return error::syntax;
    if(std::memcmp(
            it, s_, n_) != 0)
        return error::syntax;
    it += n_;
    return string_view(
        it - n_, it);
}

} // grammar
} // urls
} // boost

#endif
