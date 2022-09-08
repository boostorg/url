//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_EXCEPT_IPP
#define BOOST_URL_DETAIL_IMPL_EXCEPT_IPP

#include <boost/url/detail/except.hpp>
#include <boost/throw_exception.hpp>
#include <boost/system/system_error.hpp>
#include <new>
#include <stdexcept>

namespace boost {
namespace urls {
namespace detail {

void
throw_url_too_large(
    source_location const& loc)
{
    throw_exception(
        std::length_error(
            "url too large"), loc);
}

void
throw_bad_alloc(
    source_location const& loc)
{
    throw_exception(
        std::bad_alloc(), loc);
}

void
throw_length_error(
    char const* what,
    source_location const& loc)
{
    throw_exception(
        std::length_error(what), loc);
}

void
throw_invalid_argument(
    source_location const& loc)
{
    throw_system_error(
        make_error_code(
            errc::invalid_argument,
            &loc));
}

void
throw_system_error(
    error_code const& ec,
    source_location const& loc)
{
    throw_exception(
        boost::system::system_error(
            ec), loc);
}

} // detail
} // url
} // boost

#endif
