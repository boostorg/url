//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//


#include <boost/url/detail/config.hpp>
#include <boost/url/rfc/origin_form_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include "boost/url/rfc/detail/path_rules.hpp"
#include "detail/query_part_rule.hpp"
#include <boost/url/grammar/delim_rule.hpp>
#include <boost/url/grammar/range_rule.hpp>
#include <boost/url/grammar/tuple_rule.hpp>

namespace boost {
namespace urls {

auto
implementation_defined::origin_form_rule_t::
parse(
    char const*& it,
    char const* end
        ) const noexcept ->
    system::result<value_type>
{
    detail::url_impl u(detail::url_impl::from::string);
    u.cs_ = it;

    {
        auto rv = grammar::parse(it, end,
            grammar::range_rule(
                grammar::tuple_rule(
                    grammar::delim_rule('/'),
                    detail::segment_rule),
                1));
        if(! rv)
            return rv.error();
        u.apply_path(
            rv->string(),
            rv->size());
    }

    // [ "?" query ]
    {
        auto rv = grammar::parse(
            it, end, detail::query_part_rule);
        if(! rv)
            return rv.error();
        if(rv->has_query)
        {
            // map "?" to { {} }
            u.apply_query(
                rv->query,
                rv->count);
        }
    }

    return u.construct();
}

} // urls
} // boost

