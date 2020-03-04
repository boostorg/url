//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
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
case error::no_match: return "no match for element";
case error::syntax: return "syntax";
case error::invalid: return "invalid";

case error::missing_scheme: return "missing scheme";
case error::bad_scheme_start_char: return "bad scheme start char";
case error::bad_scheme_char: return "bad scheme char";
case error::bad_username_char: return "bad user char";
case error::bad_userinfo_char: return "bad userinfo char";
case error::bad_port_char: return "bad port char";
case error::port_overflow: return "port overflow";
case error::missing_hostname: return "missing hostname";
case error::missing_port: return "missing port";

case error::bad_pct_encoding_digit: return "bad pct-encoding digit";
case error::incomplete_pct_encoding: return "incomplete pct-encoding";
case error::illegal_reserved_char: return "illegal reserved char";
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

case error::no_match:
case error::syntax:
case error::invalid:

case error::missing_scheme:
case error::bad_scheme_start_char:
case error::bad_scheme_char:
case error::bad_username_char:
case error::bad_userinfo_char:
case error::bad_port_char:
case error::port_overflow:
case error::missing_hostname:
case error::missing_port:

case error::bad_pct_encoding_digit:
case error::incomplete_pct_encoding:
case error::illegal_reserved_char:
    return condition::parse_error;
            }
        }
    };

    static codes const cat{};
    return error_code{static_cast<
        std::underlying_type<error>::type>(e), cat};
}

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
            case condition::parse_error:
                return "parsing error";
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
