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
#include <boost/url/grammar/alnum_chars.hpp>
#include <boost/url/grammar/alpha_chars.hpp>
#include <boost/url/grammar/lut_chars.hpp>
#include <boost/url/grammar/optional_rule.hpp>
#include <boost/url/grammar/token_rule.hpp>
#include <boost/url/grammar/variant_rule.hpp>
#include <boost/url/grammar/detail/charset.hpp>
#include <boost/url/rfc/pchars.hpp>
#include <boost/url/rfc/detail/path_rules.hpp>
#include <vector>
#include <locale>

namespace boost {
namespace urls {
namespace experimental {
namespace detail {

struct match_any_t {};

// A dynamic path segment
class dynamic_segment
{
    std::string str_;
    bool is_literal_ = true;
    bool is_chrono_ = false;
    char fill_ = '\0';
    char align_ = '\0';
    char sign_ = '\0';
    bool alternate_ = false;
    bool zero_padding_ = false;
    optional<std::size_t> width_ = {};
    optional<std::size_t> precision_ = {};
    bool use_locale_ = false;
    char type_ = '\0';
    std::string chrono_specs_;

    friend struct dynamic_segment_rule_t;
public:
    dynamic_segment() = default;

    bool
    match(pct_string_view seg) const
    {
        if (is_literal_)
            return str_ == *seg;
        if (width_ &&
            seg.decoded_size() < *width_)
            return false;
        if (type_ == '\0' || type_ == 's')
            return true;
        if (!is_chrono_)
            return match_number(seg);
        return true;
    }

private:
    bool
    match_number(pct_string_view seg) const
    {
        // remove padding
        decode_view dseg = *seg;
        if (width_)
        {
            char f = fill_ != '\0' ? fill_ : ' ';
            while (
                !dseg.empty() &&
                dseg.front() == f)
            {
                dseg.remove_prefix(1);
            }
            while (
                !dseg.empty() &&
                dseg.back() == f)
            {
                dseg.remove_suffix(1);
            }
        }

        // parse nan/inf
        switch (type_)
        {
        case 'a':
        case 'e':
        case 'f':
        case 'g':
            if (dseg == "inf" ||
                dseg == "nan")
                return true;
            break;
        case 'A':
        case 'E':
        case 'F':
        case 'G':
            if (dseg == "INF" ||
                dseg == "NAN")
                return true;
            break;
        }

        // parse alternate prefix
        if (alternate_)
        {
            switch (type_)
            {
            case 'b':
                if (dseg.starts_with("0b"))
                    dseg.remove_prefix(2);
                else
                    return false;
                break;
            case 'B':
                if (dseg.starts_with("0B"))
                    dseg.remove_prefix(2);
                else
                    return false;
                break;
            case 'x':
            case 'p':
                if (dseg.starts_with("0x"))
                    dseg.remove_prefix(2);
                else
                    return false;
                break;
            case 'X':
                if (dseg.starts_with("0X"))
                    dseg.remove_prefix(2);
                else
                    return false;
                break;
            }
        }

        // parse regular prefix
        switch (type_)
        {
        case 'a':
        case 'p':
            if (seg.starts_with("0x"))
                dseg.remove_prefix(2);
            else
                return false;
            break;
        case 'A':
            if (seg.starts_with("0X"))
                dseg.remove_prefix(2);
            else
                return false;
            break;
        }

        // parse exponent suffix
        auto remove_exp_suffix = [](decode_view &dseg, char expc) {
            decode_view::const_iterator expi = dseg.begin();
            std::size_t dp = 0;
            while (expi != dseg.end())
            {
                if (*expi == expc)
                    break;
                ++expi;
                ++dp;
            }
            if (expi != dseg.end())
            {
                ++expi;
                ++dp;
                decode_view exp =
                    *make_pct_string_view_unsafe(
                        expi.base(),
                        dseg.end().base() - expi.base(),
                        dseg.size() - dp);
                if (exp.starts_with("+") || exp.starts_with("-"))
                    exp.remove_prefix(1);
                if (!std::all_of(
                        exp.begin(), exp.end(),
                        grammar::lut_chars("0123456789")))
                    return false;
                --expi;
                --dp;
            }
            dseg = *make_pct_string_view_unsafe(
                    dseg.begin().base(),
                    expi.base() - dseg.begin().base(),
                    dp);
            return true;
        };

        switch (type_)
        {
        case 'a':
            remove_exp_suffix(dseg, 'p');
            break;
        case 'A':
            remove_exp_suffix(dseg, 'P');
            break;
        case 'e':
        case 'g':
            remove_exp_suffix(dseg, 'e');
            break;
        case 'E':
        case 'G':
            remove_exp_suffix(dseg, 'E');
            break;
        }

        if (precision_)
        {
            if (*precision_ == 0)
            {
                if (dseg.find('.') != dseg.end())
                {
                    return false;
                }
            }
            else
            {
                auto p = dseg.find('.');
                if (p == dseg.end())
                    return false;
                std::size_t d = 0;
                ++p;
                while (p != dseg.end())
                {
                    ++d;
                    ++p;
                }
                if (d != *precision_)
                    return false;
            }
        }

        auto is_token =[](
            decode_view dseg,
            grammar::lut_chars const& cs)
        {
            return std::all_of(
                dseg.begin(), dseg.end(), cs);
        };

        auto is_float_token =[](
            decode_view dseg,
            grammar::lut_chars const& cs,
            char point)
        {
            auto it = dseg.begin();
            auto end = dseg.end();
            it = std::find_if_not(it, end, cs);
            if (
                it != end &&
                *it == point)
            {
                ++it;
                it = std::find_if_not(it, end, cs);
            }
            return it == end;
        };

        char point = '.';
        if (use_locale_)
        {
            point = std::use_facet<
                        std::numpunct<char>>(std::locale()).decimal_point();
        }

        switch (type_)
        {
            case 'd':
                return is_token(dseg, "0123456789");
            case 'x':
                return is_token(dseg, "0123456789abcdef");
            case 'X':
                return is_token(dseg, "0123456789ABCDEF");
            case 'o':
                return is_token(dseg, "01234567");
            case 'g':
            case 'G':
            case 'f':
            case 'F':
            case 'e':
            case 'E':
                return is_float_token(dseg, "0123456789", point);
            case 'a':
                return is_float_token(dseg, "0123456789abcdef", point);
            case 'A':
                return is_float_token(dseg, "0123456789ABCDEF", point);
            case 'b':
            case 'B':
                return is_token(dseg, "01");
            case 'c':
                return dseg.size() == 1;
            case 'p':
                return is_token(dseg, "0123456789abcdef");
        }
        return true;
    }

    string_view
    argid() const
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

    friend
    bool operator==(
        dynamic_segment const& a,
        dynamic_segment const& b)
    {
        return a.str_ == b.str_;
    }
};

// The syntax of dynamic url components is a
// subset of:
// https://fmt.dev/latest/syntax.html

// Matches [0-9]
struct integer_chars_t
{
    constexpr
        bool
        operator()(char c) const noexcept
    {
        return
            (c >= '0' && c <= '9');
    }

#ifdef BOOST_URL_USE_SSE2
    char const*
    find_if(
        char const* first,
        char const* last) const noexcept
    {
        return grammar::detail::find_if_pred(
            *this, first, last);
    }

    char const*
    find_if_not(
        char const* first,
        char const* last) const noexcept
    {
        return grammar::detail::find_if_not_pred(
            *this, first, last);
    }
#endif
};

// digit             ::=  "0"..."9"
constexpr integer_chars_t integer_chars{};

constexpr auto integer_rule =
    grammar::token_rule(integer_chars);

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
        ++it;
        while (it != end &&
               (grammar::alpha_chars(*it) ||
                *it == '_' ||
               integer_chars(*it)))
            ++it;
        return string_view(begin, it);
    }
};

constexpr auto identifier_rule = identifier_rule_t{};

// arg_id ::=  integer | identifier
constexpr auto arg_id_rule =
    grammar::variant_rule(
        integer_rule,
        identifier_rule);

// fill        ::=  <a character other than '{' or '}'>
// - This is going to be in a URL, so we limit it to pchars
// - pchars already excludes '{' or '}'
constexpr auto fill_rule = grammar::delim_rule(urls::pchars);

// align       ::=  "<" | ">" | "^"
constexpr auto align_rule = grammar::delim_rule(grammar::lut_chars("<>^"));

// sign        ::=  "+" | "-" | " "
constexpr auto sign_rule = grammar::delim_rule(grammar::lut_chars("+- "));

// width       ::=  integer | "{" [arg_id] "}"
// "{" [arg_id] "}" is not supported
constexpr auto width_rule = integer_rule;

// precision   ::=  integer | "{" [arg_id] "}"
// "{" [arg_id] "}" is not supported
constexpr auto precision_rule = integer_rule;

// type        ::=  "a" | "A" | "b" | "B" | "c" | "d" | "e" | "E" | "f" | "F" | "g" | "G" |
//                 "o" | "p" | "s" | "x" | "X"
constexpr auto type_rule = grammar::delim_rule(grammar::lut_chars("aAbBcdeEfFgGopsxX"));

// format_spec ::=  [[fill]align][sign]["#"]["0"][width]["." precision]["L"][type]
constexpr auto format_spec_rule =
    grammar::tuple_rule(
        grammar::optional_rule(
            grammar::tuple_rule(
                grammar::optional_rule(fill_rule),
                align_rule)),
        grammar::optional_rule(sign_rule),
        grammar::optional_rule(grammar::delim_rule('#')),
        grammar::optional_rule(grammar::delim_rule('0')),
        grammar::optional_rule(width_rule),
        grammar::optional_rule(
            grammar::tuple_rule(
                grammar::squelch(grammar::delim_rule('.')),
                precision_rule)),
        grammar::optional_rule(grammar::delim_rule('L')),
        grammar::optional_rule(type_rule));

// conversion_spec    ::=  "%" [modifier] chrono_type
// modifier           ::=  "E" | "O"
// chrono_type        ::=  "a" | "A" | "b" | "B" | "c" | "C" | "d" | "D" | "e" | "F" |
//                         "g" | "G" | "h" | "H" | "I" | "j" | "m" | "M" | "n" | "p" |
//                         "q" | "Q" | "r" | "R" | "S" | "t" | "T" | "u" | "U" | "V" |
//                         "w" | "W" | "x" | "X" | "y" | "Y" | "z" | "Z" | "%"
constexpr auto conversion_spec_rule =
    grammar::tuple_rule(
        grammar::squelch(grammar::delim_rule('%')),
        grammar::optional_rule(
            grammar::delim_rule(
                grammar::lut_chars("EO"))),
        grammar::delim_rule(
            grammar::lut_chars(
                "aAbBcCdDeFgGhHIjmMnpqQrRStTuUVwWxXyYzZ%")));

// chrono_specs       ::=  [chrono_specs] conversion_spec | chrono_specs literal_char
constexpr auto chrono_specs_rule =
    grammar::range_rule(
        grammar::tuple_rule(
            grammar::optional_rule(
                conversion_spec_rule),
            grammar::delim_rule(pchars)),
        1);

// chrono_format_spec ::=  [[fill]align][width]["." precision][chrono_specs]
constexpr auto chrono_format_spec_rule =
    grammar::tuple_rule(
        grammar::optional_rule(
            grammar::tuple_rule(
                grammar::optional_rule(fill_rule),
                align_rule)),
        grammar::optional_rule(width_rule),
        grammar::optional_rule(
            grammar::tuple_rule(
                grammar::squelch(grammar::delim_rule('.')),
                precision_rule)),
        grammar::optional_rule(chrono_specs_rule));

// replacement_field ::=  "{" [arg_id] [":" (format_spec | chrono_format_spec)] "}"
constexpr auto replacement_field_rule =
    grammar::tuple_rule(
        grammar::squelch(
            grammar::delim_rule('{')),
        grammar::squelch(
            grammar::optional_rule(
                arg_id_rule)),
        grammar::optional_rule(
            grammar::tuple_rule(
                grammar::squelch(
                    grammar::delim_rule(':')),
                grammar::variant_rule(
                    format_spec_rule,
                    chrono_format_spec_rule))),
        grammar::squelch(
            grammar::delim_rule('}')));

struct dynamic_segment_rule_t
{
    using value_type = dynamic_segment;

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
        if (rv->index() == 1)
        {
            // literal segment
            string_view seg = variant2::get<1>(*rv);
            dynamic_segment dseg;
            dseg.str_ = seg;
            dseg.is_literal_ = true;
            return dseg;
        }
        // dynamic segment
        dynamic_segment dseg;
        dseg.str_ = string_view(begin, it);
        dseg.is_literal_ = false;
        auto& opt_format = variant2::get<0>(*rv);
        if (opt_format)
        {
            auto& format_or_chrono = *opt_format;
            if (format_or_chrono.index() == 0)
            {
                auto& format_spec =
                    variant2::get<0>(format_or_chrono);
                auto& opt_fill_align = std::get<0>(format_spec);
                if (opt_fill_align)
                {
                    auto& fill_align = *opt_fill_align;
                    optional<string_view>& opt_fill = std::get<0>(fill_align);
                    if (opt_fill)
                    {
                        dseg.fill_ = opt_fill->front();
                    }
                    string_view& align = std::get<1>(fill_align);
                    dseg.align_ = align.front();
                }
                optional<string_view>& opt_sign = std::get<1>(format_spec);
                if (opt_sign)
                {
                    dseg.sign_ = opt_sign->front();
                }
                optional<string_view>& opt_hash = std::get<2>(format_spec);
                if (opt_hash)
                {
                    dseg.alternate_ = true;
                }
                optional<string_view>& opt_zero = std::get<3>(format_spec);
                if (opt_zero)
                {
                    dseg.zero_padding_ = true;
                }
                auto& opt_width = std::get<4>(format_spec);
                if (opt_width)
                {
                    // integer | optional
                    string_view& width = *opt_width;
                    dseg.width_ = std::stoull(width);
                }
                auto& opt_precision = std::get<5>(format_spec);
                if (opt_precision)
                {
                    string_view& precision = *opt_precision;
                    dseg.precision_ = std::stoull(precision);
                }
                optional<string_view>& opt_locale = std::get<6>(format_spec);
                if (opt_locale)
                {
                    dseg.use_locale_ = true;
                }
                optional<string_view>& opt_type = std::get<7>(format_spec);
                if (opt_type)
                {
                    dseg.type_ = opt_type->front();
                }
            }
            else
            {
                auto& chrono_spec =
                    variant2::get<1>(format_or_chrono);
                dseg.is_chrono_ = true;
                auto& opt_fill_align = std::get<0>(chrono_spec);
                if (opt_fill_align)
                {
                    auto& fill_align = *opt_fill_align;
                    optional<string_view>& opt_fill = std::get<0>(fill_align);
                    if (opt_fill)
                    {
                        dseg.fill_ = opt_fill->front();
                    }
                    string_view& align = std::get<1>(fill_align);
                    dseg.align_ = align.front();
                }
                auto& opt_width = std::get<1>(chrono_spec);
                if (opt_width)
                {
                    string_view& width = *opt_width;
                    dseg.width_ = std::stoull(width);
                }
                auto& opt_precision = std::get<2>(chrono_spec);
                if (opt_precision)
                {
                    auto& precision = *opt_precision;
                    dseg.precision_ = std::stoull(precision);
                }
                auto& opt_chrono_specs = std::get<3>(chrono_spec);
                if (opt_chrono_specs)
                {
                    auto& chrono_specs_range = *opt_chrono_specs;
                    dseg.chrono_specs_ = chrono_specs_range.string();
                }
            }
        }
        return dseg;
    }
};

constexpr auto dynamic_segment_rule = dynamic_segment_rule_t{};

constexpr auto dynamic_path_rule = grammar::range_rule(
        dynamic_segment_rule,
        grammar::tuple_rule(
            squelch( grammar::delim_rule('/') ),
            dynamic_segment_rule));

} // detail
} // experimental
} // urls
} // boost

#endif



