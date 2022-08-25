//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_QUERY_RULE_IPP
#define BOOST_URL_RFC_IMPL_QUERY_RULE_IPP

#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/detail/decode_view.hpp>
#include <boost/url/grammar/lut_chars.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/range_rule.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/rfc/detail/charsets.hpp>

namespace boost {
namespace urls {

namespace detail {

static
constexpr
auto
key_chars = pchars
    + '/' + '?' + '[' + ']'
    - '&' - '=';

static
constexpr
auto
value_chars = pchars
    + '/' + '?'
    - '&';

struct query_param_rule_t
{
    using value_type = query_param_view;

    result<value_type>
    parse(
        char const*& it,
        char const* end) const noexcept
    {
        query_param_view t;

        // VFALCO we don't return error::range_end
        // here, because the empty string still
        // counts as a 1-element range with
        // key="" and value=(none)

        // key
        {
            auto rv = grammar::parse(it, end,
                pct_encoded_rule(grammar::ref(
                    detail::key_chars)));
            if(! rv)
                return rv.error();    
            t.key = *rv;
        }

        // "="
        {
            auto rv = grammar::parse(
                it, end,
                grammar::delim_rule('='));
            t.has_value = rv.has_value();
            if(! t.has_value)
            {
                // key with no value
                return t;
            }
        }

        // value
        {
            auto rv = grammar::parse(it, end,
                pct_encoded_rule(grammar::ref(
                    detail::value_chars)));
            if(! rv)
                return rv.error();
            t.value = *rv;
        }

        return t;
    }
};

static constexpr query_param_rule_t query_param_rule{};

} // detail

auto
query_rule_t::
parse(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<value_type>
{
    struct increment
    {
        using value_type = query_param_view;

        result<value_type>
        parse(
            char const*& it,
            char const* end) const noexcept
        {
            // "&"
            {
                auto rv = grammar::parse(
                    it, end,
                    grammar::delim_rule('&'));
                if(! rv)
                    return rv.error();
            }

            return grammar::parse(it, end,
                detail::query_param_rule);
        }
    };

    auto rv = grammar::parse(
        it, end,
        grammar::range_rule(
            detail::query_param_rule,
            increment{}));
    if(! rv)
        return rv.error();
    return value_type(
        rv->string(),
        rv->size());
}

} // urls
} // boost

#endif
