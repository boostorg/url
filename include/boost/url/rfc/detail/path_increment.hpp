//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_DETAIL_PATH_INCREMENT_IPP
#define BOOST_URL_RFC_DETAIL_PATH_INCREMENT_IPP

#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/rfc/detail/segment_rule.hpp>
#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/sequence_rule.hpp>
#include <utility>

namespace boost {
namespace urls {
namespace detail {

struct path_increment_t
{
    using value_type = pct_encoded_view;

    result<value_type>
    parse(
        char const *&it,
        char const* end) const noexcept
    {
        auto it0 = it;
        auto rv = grammar::parse(
            it, end,
            grammar::sequence_rule(
                grammar::char_rule('/'),
                segment_rule));
        if(rv.has_value())
            return *rv;
        it = it0;
        return BOOST_URL_ERR(
            grammar::error::end);
    }
};

constexpr path_increment_t path_increment{};

} // detail
} // urls
} // boost

#endif
