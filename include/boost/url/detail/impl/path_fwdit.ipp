//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_PATH_FWDIT_IPP
#define BOOST_URL_DETAIL_IMPL_PATH_FWDIT_IPP

#include <boost/url/detail/path_fwdit.hpp>
#include <boost/url/string.hpp>

namespace boost {
namespace urls {
namespace detail {

path_fwdit::
path_fwdit(
    string_view s) noexcept
    : end_(s.data() + s.size())
{
    if(s.empty())
        return;
    std::size_t pos;
    if(s.starts_with('/'))
        s.remove_prefix(1);
    pos = s.find_first_of('/');
    p_ = s.data();
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
}

auto
path_fwdit::
operator++() noexcept ->
    any_helper::any_fwdit<T>&
{
    p_ += n_;
    if(p_ == end_)
    {
        p_ = nullptr;
        return *this;
    }
    ++p_;
    string_view s(p_, end_ - p_);
    auto pos = s.find_first_of('/');
    if(pos != string_view::npos)
        n_ = pos;
    else
        n_ = s.size();
    return *this;
}

} // detail
} // urls
} // boost

#endif
