//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_STATIC_URL_IPP
#define BOOST_URL_IMPL_STATIC_URL_IPP

#include <boost/url/static_url.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

static_url_base::
static_url_base(
    char* buf,
    std::size_t cap) noexcept
{
    s_ = buf;
    cap_ = cap;
    s_[0] = '\0';
    u_.cs_ = s_;
}

static_url_base::
static_url_base(
    char* buf,
    std::size_t cap,
    string_view s)
    : static_url_base(buf, cap)
{
    copy(parse_uri_reference(s).value());
}

void
static_url_base::
clear_impl() noexcept
{
    u_ = detail::url_impl(false);
    s_[0] = '\0';
    u_.cs_ = s_;
}

void
static_url_base::
reserve_impl(
    std::size_t n,
    op_t&)
{
    if(n <= cap_)
        return;
    detail::throw_bad_alloc();
}

void
static_url_base::
cleanup(op_t&)
{
}

} // urls
} // boost

#endif
