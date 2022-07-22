//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_AUTHORITY_RULE_IPP
#define BOOST_URL_IMPL_AUTHORITY_RULE_IPP

#include <boost/url/rfc/authority_rule.hpp>
#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/sequence_rule.hpp>
#include <boost/url/rfc/host_rule.hpp>
#include <boost/url/rfc/port_rule.hpp>
#include <boost/url/rfc/userinfo_rule.hpp>

namespace boost {
namespace urls {

auto
authority_rule_t::
parse(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<value_type>
{
    auto start = it;

    value_type t;

    // [ userinfo "@" ]
    {
        auto rv = grammar::parse(
            it, end,
            grammar::sequence_rule(
                userinfo_rule,
                grammar::char_rule('@')));
        if(rv.has_value())
        {
            t.has_userinfo = true;
            t.userinfo = std::get<0>(*rv);
            start = it;
        }
        else
        {
            // rewind
            t.has_userinfo = false;
            it = start;
        }
    }

    // host
    {
        auto rv = grammar::parse(
            it, end, host_rule);
        if(! rv)
            return rv.error();
        t.host = *rv;
    }

    // [ ":" port ]
    {
        auto rv = grammar::parse(
            it, end, port_part_rule);
        if(! rv)
            return rv.error();
        t.port = *rv;
    }

    return t;
}

} // urls
} // boost

#endif
