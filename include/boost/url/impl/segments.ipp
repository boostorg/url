//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_IPP
#define BOOST_URL_IMPL_SEGMENTS_IPP

#include <boost/url/segments.hpp>
#include <boost/url/url.hpp>
#include <boost/url/detail/except.hpp>

namespace boost {
namespace urls {

segments::
segments() noexcept
{
}

//------------------------------------------------

auto
segments::
iterator::
operator++() noexcept ->
    iterator&
{
    BOOST_ASSERT(i_ <
        u_->segment_count());
    ++i_;
    return *this;
}

auto
segments::
iterator::
operator--() noexcept ->
    iterator&
{
    BOOST_ASSERT(i_ > 0);
    --i_;
    return *this;
}

//------------------------------------------------

auto
segments::
begin() const noexcept ->
    iterator
{
    return iterator(u_, 0);
}

auto
segments::
end() const noexcept ->
    iterator
{
    return iterator(u_,
        u_->segment_count());
}

} // urls
} // boost

#endif
