//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_DETAIL_ROUTER_HPP
#define BOOST_URL_DETAIL_ROUTER_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/variant.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/all_chars.hpp>
#include <boost/url/grammar/alnum_chars.hpp>
#include <boost/url/grammar/alpha_chars.hpp>
#include <boost/url/grammar/lut_chars.hpp>
#include <boost/url/grammar/optional_rule.hpp>
#include <boost/url/grammar/token_rule.hpp>
#include <boost/url/grammar/variant_rule.hpp>
#include <boost/url/grammar/not_empty_rule.hpp>
#include <boost/url/grammar/detail/charset.hpp>
#include <boost/url/rfc/pchars.hpp>
#include <boost/url/rfc/detail/path_rules.hpp>
#include <boost/core/ignore_unused.hpp>
#include <vector>
#include <locale>

namespace boost {
namespace urls {
namespace detail {

// A path segment template
class segment_template
{
    enum class modifier : unsigned char
    {
        none,
        // {id?}
        optional,
        // {id*}
        star,
        // {id+}
        plus
    };

    std::string str_;
    bool is_literal_ = true;
    modifier modifier_ = modifier::none;

    friend struct segment_template_rule_t;
public:
    segment_template() = default;

    bool
    match(pct_string_view seg) const
    {
        if (is_literal_)
            return *seg == str_;

        return true;
    }

    string_view
    string() const
    {
        return str_;
    }

    string_view
    id() const
    {
        if (is_literal_)
            return {};
        return string_view{str_}
            .substr(1, str_.find_first_of('}') - 1);
    }

    bool
    empty() const
    {
        return str_.empty();
    }

    bool
    is_literal() const
    {
        return is_literal_;
    }

    bool
    has_modifier() const
    {
        return modifier_ != modifier::none;
    }

    bool
    is_optional() const
    {
        return modifier_ == modifier::optional;
    }

    bool
    is_star() const
    {
        return modifier_ == modifier::star;
    }

    bool
    is_plus() const
    {
        return modifier_ == modifier::plus;
    }

    friend
    bool operator==(
        segment_template const& a,
        segment_template const& b)
    {
        if (a.is_literal_ != b.is_literal_)
            return false;
        if (a.is_literal_)
            return a.str_ == b.str_;
        return true;
    }
};

struct segment_template_rule_t
{
    using value_type = segment_template;

    urls::result<value_type>
    parse(
        char const*& it,
        char const* end
    ) const noexcept
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
                static constexpr auto digit_cs =
                    grammar::lut_chars("0123456789");
                static constexpr auto id_start_cs =
                    grammar::alpha_chars + grammar::lut_chars('_');
                static constexpr auto id_cs =
                    grammar::alnum_chars + grammar::lut_chars('_');
                static constexpr auto modifiers_cs =
                    grammar::lut_chars("?*+");
                static constexpr auto id_rule =
                    grammar::tuple_rule(
                        grammar::variant_rule(
                            grammar::tuple_rule(
                                grammar::delim_rule(id_start_cs),
                                grammar::optional_rule(
                                    grammar::token_rule(id_cs))
                                    ),
                            grammar::token_rule(digit_cs)),
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
        rv->decode({}, urls::string_token::assign_to(t.str_));
        t.is_literal_ = true;
        return t;
    }
};

constexpr auto segment_template_rule = segment_template_rule_t{};

constexpr auto path_template_rule =
    grammar::tuple_rule(
        grammar::squelch(
            grammar::optional_rule(
                grammar::delim_rule('/'))),
        grammar::range_rule(
            segment_template_rule,
            grammar::tuple_rule(
                grammar::squelch(grammar::delim_rule('/')),
                segment_template_rule)));

} // detail
} // urls
} // boost

#endif
