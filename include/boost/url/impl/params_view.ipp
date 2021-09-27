//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_VIEW_IPP
#define BOOST_URL_IMPL_PARAMS_VIEW_IPP

#include <boost/url/params_view.hpp>
#include <boost/url/url_view.hpp>

namespace boost {
namespace urls {

params_view::
iterator::
~iterator() noexcept = default;

params_view::
iterator::
iterator() noexcept = default;

//------------------------------------------------

bool
params_view::
empty() const noexcept
{
    return u_->param_count() == 0;
}

std::size_t
params_view::
size() const noexcept
{
    return u_->param_count();
}

auto
params_view::
begin() const noexcept ->
    iterator
{
    return {};
}

auto
params_view::
end() const noexcept ->
    iterator
{
    return {};
}

} // urls
} // boost

#endif
