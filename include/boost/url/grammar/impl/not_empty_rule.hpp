//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_IMPL_NOT_EMPTY_RULE_HPP
#define BOOST_URL_GRAMMAR_IMPL_NOT_EMPTY_RULE_HPP

#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Rule for an element which may not be empty

    This rule adapts another rule such that
    when an empty string is successfully
    parsed, the result will be an error.
*/
template<class R>
auto
not_empty_rule_t<R>::
parse(
    char const*& it,
    char const* end) const ->
        result<value_type>
{
    auto const it0 = it;
    auto rv = this->get().parse(it, end);
    if( rv.has_error() ||
        it != it0)
        return rv;
    return error::empty;
}

} // grammar
} // urls
} // boost

#endif
