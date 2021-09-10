//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_ERROR_IPP
#define BOOST_URL_IMPL_ERROR_IPP

#include <boost/url/error.hpp>

namespace boost {
namespace urls {

parse_error::
parse_error()
    : std::invalid_argument(
        "parse error")
{
}

void
parse_error::
raise()
{
    BOOST_THROW_EXCEPTION(
        parse_error());
}
    
//---

invalid_part::
invalid_part()
    : std::invalid_argument(
        "bad url argument")
{
}

void
invalid_part::
raise()
{
    BOOST_THROW_EXCEPTION(
        invalid_part());
}
    
//---

too_large::
too_large()
    : std::length_error(
        "too large")
{
}

void
too_large::
raise()
{
    BOOST_THROW_EXCEPTION(
        too_large());
}
    
//---

out_of_range::
out_of_range()
    : std::out_of_range(
        "out of range")
{
}

void
out_of_range::
raise()
{
    BOOST_THROW_EXCEPTION(
        out_of_range());
}
    
//----------------------------------------------------------

error_code
make_error_code(error e)
{
    struct codes : error_category
    {
        const char*
        name() const noexcept override
        {
            return "boost.url";
        }

        std::string
        message(int ev) const override
        {
            switch(static_cast<error>(ev))
            {
            default:
case error::syntax: return "syntax";

case error::bad_alpha: return "bad alpha";
case error::bad_digit: return "bad digit";
case error::bad_empty_element: return "bad empty element";
case error::bad_hexdig: return "bad hexdig";
case error::bad_ipv6: return "bad ipv6";
case error::bad_leading_zero: return "bad leading zero";
case error::bad_octet: return "bad octet";
case error::bad_schemeless_path_segment: return "bad segment-nz-nc";
case error::empty: return "incomplete";
case error::empty_path_segment: return "empty path segment";
case error::end: return "end";
case error::illegal_null: return "illegal null";
case error::illegal_reserved_char: return "illegal reserved char";
case error::incomplete: return "incomplete";
case error::invalid: return "invalid";
case error::leftover_input: return "leftover input";
case error::missing_char_literal: return "missing char literal";
case error::missing_path_segment: return "missing path segment";
case error::missing_path_separator: return "missing path sep";
case error::missing_words: return "missing words";
case error::non_canonical: return "non canonical";
case error::wrong_char_literal: return "wrong char literal";

case error::bad_pct_hexdig: return "bad hexdig in pct-encoding";
case error::incomplete_pct_encoding: return "incomplete pct-encoding";
case error::missing_pct_hexdig: return "missing hexdig in pct-encoding";
            }
        }

        error_condition
        default_error_condition(
            int ev) const noexcept override
        {
            switch(static_cast<error>(ev))
            {
            default:
                return {ev, *this};

case error::bad_pct_hexdig:
case error::incomplete_pct_encoding:
case error::missing_pct_hexdig:
    return condition::fatal;
            }
        }
    };

    static codes const cat{};
    return error_code{static_cast<
        std::underlying_type<error>::type>(e), cat};
}

//------------------------------------------------

error_condition
make_error_condition(condition c)
{
    struct codes : error_category
    {
        const char*
        name() const noexcept override
        {
            return "boost.url";
        }

        std::string
        message(int cv) const override
        {
            switch(static_cast<condition>(cv))
            {
            default:
            case condition::fatal:
                return "fatal condition";
            }
        }
    };
    static codes const cat{};
    return error_condition{static_cast<
        std::underlying_type<condition>::type>(c), cat};
}

} // urls
} // boost

#endif
