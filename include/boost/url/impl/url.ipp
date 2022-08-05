//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_URL_IPP
#define BOOST_URL_IMPL_URL_IPP

#include <boost/url/url.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

url::
~url()
{
    if(s_)
    {
        BOOST_ASSERT(
            cap_ != 0);
        deallocate(s_);
    }
}

// construct empty
url::
url() noexcept = default;

url::
url(string_view s)
    : url(parse_uri_reference(s).value())
{
}

url::
url(url&& u) noexcept
    : url_base(u.u_)
{
    s_ = u.s_;
    cap_ = u.cap_;
    u.s_ = nullptr;
    u.cap_ = 0;
    u.u_ = detail::url_impl(false);
}

url&
url::
operator=(url&& u) noexcept
{
    if(s_)
        deallocate(s_);
    u_ = u.u_;
    s_ = u.s_;
    cap_ = u.cap_;
    u.s_ = nullptr;
    u.cap_ = 0;
    u.u_ = detail::url_impl(false);
    return *this;
}

//------------------------------------------------

char*
url::
allocate(std::size_t n)
{
    auto s = new char[n + 1];
    cap_ = n;
    return s;
}

void
url::
deallocate(char* s)
{
    delete[] s;
}

void
url::
clear_impl() noexcept
{
    if(s_)
    {
        // preserve capacity
        u_ = detail::url_impl(false);
        s_[0] = '\0';
        u_.cs_ = s_;
    }
    else
    {
        BOOST_ASSERT(u_.cs_ ==
            detail::empty_c_str_);
    }
}

void
url::
reserve_impl(
    std::size_t n)
{
    if(n > max_size())
        detail::throw_length_error(
            "n > max_size",
            BOOST_CURRENT_LOCATION);
    if(n <= cap_)
        return;
    char* s;
    if(s_ != nullptr)
    {
        // 50% growth policy
        auto const h = cap_ / 2;
        std::size_t new_cap;
        if(cap_ <= max_size() - h)
            new_cap = cap_ + h;
        else
            new_cap = max_size();
        if( new_cap < n)
            new_cap = n;
        s = allocate(new_cap);
        std::memcpy(s, s_, size());
        deallocate(s_);
        s_ = s;
    }
    else
    {
        s_ = allocate(n);
        s_[0] = '\0';
    }
    u_.cs_ = s_;
}

} // urls
} // boost

#endif
