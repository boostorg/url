//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_IMPL_ERROR_IPP
#define BOOST_URL_IMPL_ERROR_IPP

#include <boost/url/error.hpp>

namespace boost {
namespace urls {

namespace detail {

const char*
error_cat_type::
name() const noexcept
{
    return "boost.url";
}

std::string
error_cat_type::
message(int code) const
{
    return message(code, nullptr, 0);
}

char const*
error_cat_type::
message(
    int code,
    char*,
    std::size_t) const noexcept
{
    switch(static_cast<error>(code))
    {
case error::success: return "success";
case error::illegal_null: return "illegal null";
case error::illegal_reserved_char: return "illegal reserved char";
case error::non_canonical: return "non canonical";

case error::bad_pct_hexdig: return "bad hexdig in pct-encoding";
case error::incomplete_encoding: return "incomplete pct-encoding";
case error::missing_pct_hexdig: return "missing hexdig in pct-encoding";
case error::no_space: return "no space";
case error::not_a_base: return "not a base";
    }
    return "";
}

error_condition
error_cat_type::
default_error_condition(
    int ev) const noexcept
{
    switch(static_cast<error>(ev))
    {
    default:
        return {ev, *this};

case error::bad_pct_hexdig:
case error::incomplete_encoding:
case error::missing_pct_hexdig:
return grammar::condition::fatal;
    }
}

} // detail

} // urls
} // boost

#endif
