//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_ABSOLUTE_URI_RULE_IPP
#define BOOST_URL_RFC_IMPL_ABSOLUTE_URI_RULE_IPP

#include <boost/url/rfc/absolute_uri_rule.hpp>
#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/sequence_rule.hpp>
#include <boost/url/grammar/optional_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/detail/hier_part_rule.hpp>
#include <boost/url/rfc/detail/scheme_rule.hpp>
#include <utility>

namespace boost {
namespace urls {

auto
absolute_uri_rule_t::
parse(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<value_type>
{
    detail::url_impl u(false);
    u.cs_ = it;

    // scheme
    {
        auto rv = grammar::parse(
            it, end,
            grammar::sequence_rule(
                detail::scheme_rule(),
                grammar::squelch(
                    grammar::char_rule(':'))));
        if(! rv)
            return rv.error();
        u.apply_scheme(rv->scheme);
    }

    // hier_part
    {
        auto rv = grammar::parse(
            it, end, detail::hier_part_rule);
        if(! rv)
            return rv.error();
        if(rv->has_authority)
            u.apply_authority(rv->authority);
        u.apply_path(
            rv->path.string(),
            rv->path.size());
    }

    // [ "?" query ]
    {
        auto rv = grammar::parse(
            it, end,
            grammar::optional_rule(
                grammar::sequence_rule(
                    grammar::squelch(
                        grammar::char_rule('?')),
                    query_rule)));
        if(! rv)
            return rv.error();
        if(rv->has_value())
        {
            auto const& v = **rv;
            u.apply_query(
                v.encoded_string(),
                v.size());
        }
    }

    return u.construct();
}

} // urls
} // boost

#endif
