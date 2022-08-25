//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_CONST_VIEW_IPP
#define BOOST_URL_IMPL_PARAMS_CONST_VIEW_IPP

#include <boost/url/params_const_view.hpp>
#include <boost/url/url.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

auto
params_const_view::
begin() const noexcept ->
    iterator
{
    return { *u_ };
}

auto
params_const_view::
end() const noexcept ->
    iterator
{
    return { *u_, 0 };
}

//------------------------------------------------
//
// Lookup
//
//------------------------------------------------

std::size_t
params_const_view::
count(string_view key) const noexcept
{
    std::size_t n = 0;
    auto it = find(key);
    auto const end_ = end();
    while(it != end_)
    {
        ++n;
        ++it;
        it = find(it, key);
    }
    return n;
}

auto
params_const_view::
find(
    iterator from,
    string_view key) const noexcept ->
        iterator
{
    // different container
    BOOST_ASSERT(
        from.it_.u == &u_->u_);

    auto const end_ = end();
    while(from != end_)
    {
        if( (*from).key == key )
            break;
        ++from;
    }
    return from;
}

} // urls
} // boost

#endif
