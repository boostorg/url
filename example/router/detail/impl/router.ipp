//
// Copyright (c) 2023 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_DETAIL_ROUTER_IPP
#define BOOST_URL_DETAIL_ROUTER_IPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/alnum_chars.hpp>
#include <boost/url/grammar/alpha_chars.hpp>
#include <boost/url/grammar/lut_chars.hpp>
#include <boost/url/grammar/token_rule.hpp>
#include <boost/url/grammar/variant_rule.hpp>
#include <boost/url/detail/replacement_field_rule.hpp>
#include <detail/router.hpp>

namespace boost {
namespace urls {
namespace detail {

bool
segment_template::
match(pct_string_view seg) const
{
    if (is_literal_)
        return *seg == str_;

    // other nodes match any string
    return true;
}

string_view
segment_template::
id() const
{
    if (is_literal_)
        return {};
    return string_view{str_}
        .substr(1, str_.find_first_of('}') - 1);
}

auto
segment_template_rule_t::
parse(
    char const*& it,
    char const* end) const noexcept
    -> urls::result<value_type>
{
    segment_template t;
    if (it != end &&
        *it == '{')
    {
        // replacement field
        auto it0 = it;
        ++it;
        auto send =
            grammar::find_if(
                it, end, grammar::lut_chars('}'));
        if (send != end)
        {
            string_view s(it, send);
            static constexpr auto modifiers_cs =
                grammar::lut_chars("?*+");
            static constexpr auto id_rule =
                grammar::tuple_rule(
                    grammar::optional_rule(
                        arg_id_rule),
                    grammar::optional_rule(
                        grammar::delim_rule(modifiers_cs)));
            if (s.empty() ||
                grammar::parse(s, id_rule))
            {
                it = send + 1;

                t.str_ = string_view(it0, send + 1);
                t.is_literal_ = false;
                if (s.ends_with('?'))
                    t.modifier_ =
                        segment_template::modifier::optional;
                else if (s.ends_with('*'))
                    t.modifier_ =
                        segment_template::modifier::star;
                else if (s.ends_with('+'))
                    t.modifier_ =
                        segment_template::modifier::plus;
                return t;
            }
        }
        it = it0;
    }
    // literal segment
    auto rv = grammar::parse(
        it, end, urls::detail::segment_rule);
    BOOST_ASSERT(rv);
    rv->decode({}, urls::string_token::assign_to(t.str_));
    t.is_literal_ = true;
    return t;
}

} // detail
} // urls
} // boost

#endif
