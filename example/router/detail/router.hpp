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

#include <boost/url/pct_string_view.hpp>
#include <boost/url/grammar/delim_rule.hpp>
#include <boost/url/grammar/optional_rule.hpp>
#include <boost/url/grammar/range_rule.hpp>
#include <boost/url/grammar/tuple_rule.hpp>
#include <string>

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

    BOOST_URL_DECL
    bool
    match(pct_string_view seg) const;

    string_view
    string() const
    {
        return str_;
    }

    BOOST_URL_DECL
    string_view
    id() const;

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

    BOOST_URL_DECL
    urls::result<value_type>
    parse(
        char const*& it,
        char const* end
    ) const noexcept;
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
