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

#include <boost/url/url_view.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

static_url_base::
~static_url_base()
{
    // prevent url from
    // deallocating memory
    s_ = nullptr;
}

static_url_base::
static_url_base(
    char* buf,
    std::size_t cap) noexcept
    : url(buf, cap)
{
}

void
static_url_base::
copy(url const& u)
{
    this->url::copy(u.s_, u.pt_);
}

void
static_url_base::
copy(url_view const& u)
{
    this->url::copy(u.s_, u.pt_);
}

url_view
static_url_base::
convert() const noexcept
{
    return url_view(s_, pt_);
}

char*
static_url_base::
alloc_impl(std::size_t n)
{
    // should never get here
    BOOST_ASSERT(n > capacity());
    detail::throw_invalid_argument(
        "alloc_impl",
        BOOST_CURRENT_LOCATION);
}

void
static_url_base::
free_impl(char*)
{
    // should never get here
    detail::throw_invalid_argument(
        "free_impl",
        BOOST_CURRENT_LOCATION);
}

std::size_t
static_url_base::
growth_impl(
    std::size_t cap,
    std::size_t new_size)
{
    BOOST_ASSERT(new_size > cap);
    detail::throw_bad_alloc(
        BOOST_CURRENT_LOCATION);
}

static_url_base::
operator url_view() const noexcept
{
    return url_view(s_, pt_);
}

} // urls
} // boost

#endif
