//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_EXPERIMENTAL_DETAIL_ROUTER_HPP
#define BOOST_URL_EXPERIMENTAL_DETAIL_ROUTER_HPP

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
namespace experimental {
namespace detail {

struct match_any_t {};

// A dynamic path segment
class segment_template
{
    std::string str_;
    bool is_literal_ = true;

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
            .substr(1, str_.find_first_of(":}") - 1);
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

// The syntax of dynamic url components is a
// subset of:
// https://fmt.dev/latest/syntax.html

// digit             ::=  "0"..."9"
constexpr auto integer_chars =
    grammar::lut_chars("0123456789");

// identifier        ::=  id_start id_continue*
// id_start          ::=  "a"..."z" | "A"..."Z" | "_"
// id_continue       ::=  id_start | digit
struct identifier_rule_t
{
    using value_type = string_view;

    urls::result<value_type>
    parse(
        char const*& it,
        char const* end
    ) const noexcept
    {
        if (it == end)
            return grammar::error::need_more;
        char const* begin = it;
        if (!grammar::alpha_chars(*it) &&
            *it != '_')
            return grammar::error::invalid;
        static constexpr auto identifier_chars =
            grammar::alnum_chars + grammar::lut_chars('_');
        ++it;
        it = grammar::find_if_not(it, end, identifier_chars);
        return string_view(begin, it);
    }
};

constexpr auto identifier_rule = identifier_rule_t{};

// arg_id ::=  integer | identifier
constexpr auto arg_id_rule =
    grammar::variant_rule(
        grammar::token_rule(integer_chars),
        identifier_rule);

// replacement_field ::=  "{" [arg_id] "}"
constexpr auto replacement_field_rule =
    grammar::tuple_rule(
        grammar::squelch(
            grammar::delim_rule('{')),
        grammar::squelch(
            grammar::optional_rule(
                arg_id_rule)),
        grammar::squelch(
            grammar::delim_rule('}')));

struct segment_template_rule_t
{
    using value_type = segment_template;

    urls::result<value_type>
    parse(
        char const*& it,
        char const* end
    ) const noexcept
    {
        auto begin = it;
        auto rv = grammar::variant_rule(
            replacement_field_rule,
            urls::detail::segment_rule)
                .parse(it, end);
        if (!rv)
            return rv.error();
        segment_template t;
        if (rv->index() == 0)
        {
            // segment replacement
            t.str_ = {begin, it};
            t.is_literal_ = false;
            return t;
        }
        // literal segment
        string_view seg = variant2::get<1>(*rv);
        make_pct_string_view(seg)
            ->decode({}, urls::string_token::assign_to(t.str_));
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
} // experimental
} // urls
} // boost

#endif
