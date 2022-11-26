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
class dynamic_segment
{
    std::string str_;

    enum class seg_type : std::uint8_t
    {
        literal,
        builtin,
        chrono,
        builtin_range,
        chrono_range,
    };

    seg_type seg_type_ = seg_type::literal;

    // format specs
    char fill_ = '\0';
    char align_ = '\0';
    char sign_ = '-';
    bool alternate_ = false;
    bool zero_padding_ = false;
    optional<std::size_t> width_ = {};
    optional<std::size_t> precision_ = {};
    bool use_locale_ = false;
    char type_ = 's';
    std::string chrono_specs_;

    friend struct dynamic_segment_rule_t;
public:
    dynamic_segment() = default;

    bool
    match(pct_string_view seg) const
    {
        if (seg_type_ == seg_type::literal)
            return *seg == str_;

        if (width_ &&
            seg.decoded_size() < *width_)
            return false;

        switch (seg_type_)
        {
        case seg_type::builtin:
            return match_builtin(*seg);
        case seg_type::chrono:
            return match_chrono(*seg);
        case seg_type::builtin_range:
        case seg_type::chrono_range:
            return match_range(*seg);
        default:
            return true;
        }
    }

    string_view
    string() const
    {
        return str_;
    }


    string_view
    argid() const
    {
        if (seg_type_ == seg_type::literal)
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

private:
    bool
    match_builtin(decode_view dseg) const
    {
        if (type_ == 's')
            return true;

        // remove sign
        auto remove_sign =
            [this](decode_view& dseg)
        {
            if (dseg.starts_with('-') ||
                (sign_ != '-' &&
                 dseg.starts_with(sign_)))
            {
                dseg.remove_prefix(1);
                return true;
            }
            return sign_ == '-';
        };

        // align + fill
        if (align_ != '\0')
        {
            if (align_ == '>' ||
                align_ == '^')
            {
                while (dseg.starts_with(fill_))
                {
                    dseg.remove_prefix(1);
                }
            }
            if (align_ == '<' ||
                align_ == '^')
            {
                while (dseg.ends_with(fill_))
                {
                    dseg.remove_suffix(1);
                }
            }
        }

        // remove padding
        if (width_)
        {
            // effective fill char
            char f = fill_ != '\0' ? fill_ : ' ';

            // sign-aware zero-padding
            if (zero_padding_)
            {
                f = '0';
                // any sign comes before 0s if
                // 0 padding and no fill
                if (fill_ == '\0')
                {
                    if (!remove_sign(dseg))
                        return false;
                }
            }

            // remove fill
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

        // parse sign
        if (!remove_sign(dseg))
            return false;

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
            case 'o':
                if (dseg.starts_with("0"))
                    dseg.remove_prefix(1);
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
            if (dseg.starts_with("0x"))
                dseg.remove_prefix(2);
            else
                return false;
            break;
        case 'A':
            if (dseg.starts_with("0X"))
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
            char point,
            char alternate)
        {
            auto it = dseg.begin();
            auto end = dseg.end();
            it = std::find_if_not(it, end, cs);

            // alternate form for floating-point
            // numbers requires decimal point
            if (alternate &&
                it == end)
                return false;

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
                return is_float_token(dseg, "0123456789", point, alternate_);
            case 'a':
                return is_float_token(dseg, "0123456789abcdef", point, alternate_);
            case 'A':
                return is_float_token(dseg, "0123456789ABCDEF", point, alternate_);
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

    bool
    match_range(decode_view dseg) const
    {
        if (!dseg.starts_with("["))
            return false;
        dseg.remove_prefix(1);
        if (!dseg.ends_with("]"))
            return false;
        dseg.remove_suffix(1);
        while (!dseg.empty())
        {
            auto it0 = dseg.begin();
            auto it = dseg.begin();
            std::size_t dn = 0;
            while (
                it != dseg.end() &&
                *it != ',')
            {
                ++it;
                ++dn;
            }
            if (seg_type_ == seg_type::builtin_range)
            {
                if (!match_builtin(
                        *make_pct_string_view_unsafe(
                            it0.base(),
                            it.base() - it0.base(),
                            dn)))
                {
                    return false;
                }
            }
            else
            {
                if (!match_chrono(
                        *make_pct_string_view_unsafe(
                            it0.base(),
                            it.base() - it0.base(),
                            dn)))
                {
                    return false;
                }
            }
            dseg = *make_pct_string_view_unsafe(
                it.base(),
                dseg.end().base() - it.base(),
                dseg.size() - dn);
            if (dseg.empty())
                break;
            if (!dseg.starts_with(','))
                return false;
            dseg.remove_prefix(1);
            if (dseg.starts_with(" "))
                dseg.remove_prefix(1);
        }
        return true;
    }

    bool
    match_chrono(decode_view dseg) const
    {
        static constexpr auto chrono_modifier_chars =
            grammar::lut_chars("EO");
        static constexpr auto chrono_type_chars =
            grammar::lut_chars(
                "aAbBcCdDeFgGhHIjmMnpqQrRStTuUVwWxXyYzZ%");
        static constexpr auto decimal_chars =
            grammar::lut_chars("0123456789");
        string_view fmt = chrono_specs_;

        auto consume_literal =
            [](decode_view& dseg, char ch)
        {
            if (dseg.empty() ||
                dseg.front() != ch)
                return false;
            dseg.remove_prefix(1);
            return true;
        };

        auto consume_any_number =
            [](decode_view& dseg,
               std::size_t chars)
        {
            if (dseg.size() < chars)
                return false;
            while (chars)
            {
                if (!decimal_chars(dseg.front()))
                    return false;
                dseg.remove_prefix(1);
                --chars;
            }
            return true;
        };

        auto consume_alpha =
            [](decode_view& dseg,
               std::size_t chars)
        {
            if (dseg.size() < chars)
                return false;
            while (chars)
            {
                if (!grammar::alpha_chars(dseg.front()))
                    return false;
                dseg.remove_prefix(1);
                --chars;
            }
            return true;
        };

        auto consume_any_alpha =
            [](decode_view& dseg)
        {
            if (dseg.empty() ||
                !grammar::alpha_chars(dseg.front()))
                return false;
            dseg.remove_prefix(1);
            while (grammar::alpha_chars(dseg.front()))
                dseg.remove_prefix(1);
            return true;
        };

        auto consume_number =
            [](decode_view& dseg,
               std::size_t chars,
               int lb,
               int ub)
        {
            if (dseg.size() < chars)
                return false;
            bool is_upper = true;
            bool is_lower = true;
            static int pow10[4] = {1, 10, 100, 1000};

            while (chars)
            {
                if (!decimal_chars(dseg.front()))
                    return false;
                int digit = dseg.front() - '0';
                dseg.remove_prefix(1);

                --chars;
                int lb_digit = lb / pow10[chars];
                int ub_digit = ub / pow10[chars];

                if (digit != ub_digit)
                    is_upper = false;
                if (digit != lb_digit)
                    is_lower = false;

                if (is_lower &&
                    digit < lb_digit)
                    return false;
                if (is_upper &&
                    digit > ub_digit)
                    return false;
            }
            return true;
        };

        while (
            !fmt.empty() &&
            !dseg.empty())
        {
            // consume literal
            if (fmt.front() != '%')
            {
                if (!consume_literal(dseg, fmt.front()))
                    return false;
                fmt.remove_prefix(1);
                continue;
            }

            // consume fmt rule type
            fmt.remove_prefix(1);
            BOOST_ASSERT(!fmt.empty());
            if (chrono_modifier_chars(fmt.front()))
            {
                fmt.remove_prefix(1);
                BOOST_ASSERT(!fmt.empty());
            }
            BOOST_ASSERT(chrono_type_chars(fmt.front()));
            ignore_unused(chrono_type_chars);
            char t = fmt.front();
            fmt.remove_prefix(1);

            // consume and match rule
            switch (t)
            {
            case '%':
                if (!consume_literal(dseg, '%'))
                    return false;
                break;
            case 'n':
                if (!consume_literal(dseg, '\n'))
                    return false;
                break;
            case 't':
                if (!consume_literal(dseg, '\t'))
                    return false;
                break;
            case 'Y':
            case 'G':
                if (!consume_any_number(dseg, 4))
                    return false;
                break;
            case 'j':
                if (!consume_any_number(dseg, 3))
                    return false;
                break;
            case 'd':
                if (!consume_number(dseg, 2, 1, 31))
                    return false;
                break;
            case 'y':
            case 'g':
            case 'C':
                if (!consume_any_number(dseg, 2))
                    return false;
                break;
            case 'm':
                if (!consume_number(dseg, 2, 1, 12))
                    return false;
                break;
            case 'e':
                if (dseg.size() < 2)
                    return false;
                if (dseg.front() == ' ')
                {
                    dseg.remove_prefix(1);
                    if (dseg.front() < '1' ||
                        dseg.front() > '9')
                        return false;
                    dseg.remove_prefix(1);
                }
                else if (dseg.front() < '3')
                {
                    dseg.remove_prefix(1);
                    if (!decimal_chars(dseg.front()))
                        return false;
                    dseg.remove_prefix(1);
                }
                else if (dseg.front() == '3')
                {
                    dseg.remove_prefix(1);
                    if (dseg.front() < '0' ||
                        dseg.front() > '1')
                        return false;
                    dseg.remove_prefix(1);
                }
                else
                {
                    return false;
                }
                break;
            case 'U':
            case 'W':
                if (!consume_number(dseg, 2, 0, 53))
                    return false;
                break;
            case 'V':
                if (!consume_number(dseg, 2, 1, 53))
                    return false;
                break;
            case 'H':
                if (!consume_number(dseg, 2, 0, 23))
                    return false;
                break;
            case 'S':
                if (!consume_number(dseg, 2, 0, 60))
                    return false;
                break;
            case 'M':
                if (!consume_number(dseg, 2, 0, 59))
                    return false;
                break;
            case 'I':
                if (!consume_number(dseg, 2, 1, 12))
                    return false;
                break;
            case 'w':
                if (!consume_number(dseg, 1, 0, 6))
                    return false;
                break;
            case 'u':
                if (!consume_number(dseg, 1, 1, 7))
                    return false;
                break;
            case 'b':
            case 'h':
            case 'a':
                if (!consume_alpha(dseg, 3))
                    return false;
                break;
            case 'B':
            case 'A':
                if (!consume_any_alpha(dseg))
                    return false;
                break;
            case 'c':
                if (!consume_alpha(dseg, 3))
                    return false;
                if (!consume_literal(dseg, ' '))
                    return false;
                if (!consume_alpha(dseg, 3))
                    return false;
                if (!consume_literal(dseg, ' '))
                    return false;
                if (!decimal_chars(dseg.front()) &&
                    dseg.front() != ' ')
                    return false;
                dseg.remove_prefix(1);
                if (!decimal_chars(dseg.front()))
                    return false;
                dseg.remove_prefix(1);
                if (!consume_literal(dseg, ' '))
                    return false;
                if (!consume_number(dseg, 2, 0, 24))
                    return false;
                if (!consume_literal(dseg, ':'))
                    return false;
                if (!consume_number(dseg, 2, 0, 60))
                    return false;
                if (!consume_literal(dseg, ':'))
                    return false;
                if (!consume_number(dseg, 2, 0, 60))
                    return false;
                if (!consume_literal(dseg, ' '))
                    return false;
                if (!consume_any_number(dseg, 4))
                    return false;
                break;
            case 'D':
                if (!consume_number(dseg, 2, 1, 12))
                    return false;
                if (!consume_literal(dseg, '/'))
                    return false;
                if (!consume_number(dseg, 2, 1, 31))
                    return false;
                if (!consume_literal(dseg, '/'))
                    return false;
                if (!consume_any_number(dseg, 2))
                    return false;
                break;
            case 'F':
                if (!consume_any_number(dseg, 4))
                    return false;
                if (!consume_literal(dseg, '-'))
                    return false;
                if (!consume_number(dseg, 2, 1, 12))
                    return false;
                if (!consume_literal(dseg, '-'))
                    return false;
                if (!consume_number(dseg, 2, 1, 31))
                    return false;
                break;
            case 'R':
                if (!consume_number(dseg, 2, 0, 23))
                    return false;
                if (!consume_literal(dseg, ':'))
                    return false;
                if (!consume_number(dseg, 2, 0, 59))
                    return false;
                break;
            case 'T':
                if (!consume_number(dseg, 2, 0, 23))
                    return false;
                if (!consume_literal(dseg, ':'))
                    return false;
                if (!consume_number(dseg, 2, 0, 59))
                    return false;
                if (!consume_literal(dseg, ':'))
                    return false;
                if (!consume_number(dseg, 2, 0, 60))
                    return false;
                break;
            case 'z':
                if (consume_literal(dseg, '-') ||
                    consume_literal(dseg, '+'))
                    consume_any_number(dseg, 4);
                break;
            case 'x':
            case 'X':
            case 'r':
            case 'p':
            case 'Z':
                break;
            default:
                return false;
            }
        }
        return dseg.empty() && fmt.empty();
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

// chrono_specs       ::=  [chrono_specs] conversion_spec | chrono_specs literal_char
// conversion_spec    ::=  "%" [modifier] chrono_type
// modifier           ::=  "E" | "O"
// chrono_type        ::=  "a" | "A" | "b" | "B" | "c" | "C" | "d" | "D" | "e" | "F" |
//                         "g" | "G" | "h" | "H" | "I" | "j" | "m" | "M" | "n" | "p" |
//                         "q" | "Q" | "r" | "R" | "S" | "t" | "T" | "u" | "U" | "V" |
//                         "w" | "W" | "x" | "X" | "y" | "Y" | "z" | "Z" | "%"
struct chrono_specs_rule_t
{
    using value_type = string_view;

    urls::result<value_type>
    parse(
        char const*& it,
        char const* end
    ) const noexcept
    {
        if (it == end)
        {
            BOOST_URL_RETURN_EC(
                grammar::error::need_more);
        }

        static constexpr auto chrono_literal_chars =
            grammar::all_chars - grammar::lut_chars("{}%");
        static constexpr auto chrono_modifier_chars =
            grammar::lut_chars("EO");
        static constexpr auto chrono_type_chars =
            grammar::lut_chars(
                "aAbBcCdDeFgGhHIjmMnpqQrRStTuUVwWxXyYzZ%");
        static constexpr auto conversion_spec =
            grammar::tuple_rule(
                grammar::squelch(grammar::delim_rule('%')),
                grammar::optional_rule(
                    grammar::delim_rule(chrono_modifier_chars)),
                grammar::delim_rule(chrono_type_chars));
        auto first = it;
        bool one_spec_match = false;
        while (it != end)
        {
            if (chrono_literal_chars(*it))
            {
                ++it;
            }
            else
            {
                auto it0 = it;
                auto rv = conversion_spec.parse(it, end);
                if (rv.has_value())
                {
                    one_spec_match = true;
                }
                else
                {
                    it = it0;
                    break;
                }
            }
        }
        if (!one_spec_match)
        {
            BOOST_URL_RETURN_EC(
                grammar::error::invalid);
        }
        return string_view(first, it);
    }
};

constexpr auto chrono_specs_rule = chrono_specs_rule_t{};


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

struct empty_rule_t
{
    using value_type = string_view;

    auto
    parse(
        char const*&,
        char const*) const ->
            result<value_type>
    {
        return {};
    }
};

constexpr auto empty_rule = empty_rule_t{};

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
                grammar::optional_rule(
                    grammar::delim_rule(':')),
                grammar::variant_rule(
                    grammar::not_empty_rule(format_spec_rule),
                    grammar::not_empty_rule(chrono_format_spec_rule),
                    empty_rule))),
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
            make_pct_string_view(seg)
                ->decode({}, urls::string_token::assign_to(dseg.str_));
            dseg.seg_type_ = dynamic_segment::seg_type::literal;
            return dseg;
        }
        // dynamic segment
        dynamic_segment dseg;
        dseg.str_ = string_view(begin, it);
        dseg.seg_type_ = dynamic_segment::seg_type::builtin;
        auto& opt_format = variant2::get<0>(*rv);
        if (opt_format)
        {
            auto& range_colon = std::get<0>(*opt_format);
            if (range_colon)
                dseg.seg_type_ = dynamic_segment::seg_type::builtin_range;
            auto& format_or_chrono = std::get<1>(*opt_format);
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
            else if (format_or_chrono.index() == 1)
            {
                if (!range_colon)
                    dseg.seg_type_ = dynamic_segment::seg_type::chrono;
                else
                    dseg.seg_type_ = dynamic_segment::seg_type::chrono_range;
                auto& chrono_spec =
                    variant2::get<1>(format_or_chrono);
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
                    dseg.chrono_specs_ = *opt_chrono_specs;
                }
            }
            else
            {
                // if format after ":" is empty, we have a builtin
                // with default values
                dseg.seg_type_ = dynamic_segment::seg_type::builtin;
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
