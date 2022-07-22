//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_URI_REFERENCE_RULE_IPP
#define BOOST_URL_IMPL_URI_REFERENCE_RULE_IPP

#include <boost/url/rfc/uri_reference_rule.hpp>
#include <boost/url/rfc/hier_part_rule.hpp>
#include <boost/url/rfc/relative_part_rule.hpp>
#include <boost/url/grammar/parse.hpp>

namespace boost {
namespace urls {

auto
uri_reference_rule_t::
parse(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<value_type>
{
    value_type t;

    auto const start = it;

    // scheme ":"
    {
        auto rv = grammar::parse(
            it, end, scheme_part_rule());
        if(! rv)
        {
            // rewind
            it = start;

            // relative-ref
            {
                auto rv2 = grammar::parse(
                    it, end, relative_part_rule);
                if(! rv2)
                    return rv2.error();

                auto const& v = rv2.value();
                t.has_authority =
                    v.has_authority;
                t.authority = v.authority;
                t.path = v.path;
            }
        }
        else
        {
            t.scheme_part = *rv;

            // hier-part
            auto rv2 = grammar::parse(
                it, end, hier_part_rule);
            if(! rv2)
                return rv2.error();
            auto t0 = rv2.value();

            t.has_authority =
                t0.has_authority;
            t.authority = t0.authority;
            t.path = t0.path;
        }
    }

    // [ "?" query ]
    {
        auto rv = grammar::parse(
            it, end, query_part_rule);
        if(! rv)
            return rv.error();
        t.query_part = *rv;
    }

    // [ "#" fragment ]
    {
        auto rv = grammar::parse(
            it, end, fragment_part_rule);
        if(! rv)
            return rv.error();
        t.fragment_part = *rv;
    }

    return t;
}

} // urls
} // boost

#endif
