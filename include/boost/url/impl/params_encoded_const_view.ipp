//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_ENCODED_CONST_VIEW_IPP
#define BOOST_URL_IMPL_PARAMS_ENCODED_CONST_VIEW_IPP

#include <boost/url/params_encoded_const_view.hpp>
#include <boost/url/url.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/detail/normalize.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

auto
params_encoded_const_view::
begin() const noexcept ->
    iterator
{
    return { *u_ };
}

auto
params_encoded_const_view::
end() const noexcept ->
    iterator
{
    return { *u_, 0 };
}

std::size_t
params_encoded_const_view::
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
params_encoded_const_view::
find(
    iterator it,
    string_view key) const noexcept ->
        iterator
{
    auto const end_ = end();
    while(it != end_)
    {
        if( detail::compare_encoded(
                key, it->key) == 0)
            break;
        ++it;
    }
    return it;
}

} // urls
} // boost

#endif
