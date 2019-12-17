//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_VIEW_IPP
#define BOOST_URL_IMPL_VIEW_IPP

#include <boost/url/view.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/detail/char_type.hpp>

namespace boost {
namespace url {

//----------------------------------------------------------
//
// scheme
//
//----------------------------------------------------------

string_view
view::
scheme() const noexcept
{
    auto s = get(id_scheme);
    if(s.empty())
        return s;
    BOOST_ASSERT(s.back() == ':');
    s.remove_suffix(1); // ':'
    return s;
}

//----------------------------------------------------------
//
// authority
//
//----------------------------------------------------------

string_view
view::
encoded_authority() const noexcept
{
    auto s = get(
        id_username,
        id_path);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() >= 2);
        s.remove_prefix(2);
    }
    return s;
}

//
// userinfo
//

string_view
view::
encoded_userinfo() const noexcept
{
    auto s = get(
        id_username,
        id_hostname);
    if(s.empty())
        return s;
    if(s.back() == '@')
        s.remove_suffix(1);
    BOOST_ASSERT(s.size() >= 2);
    BOOST_ASSERT(
        s.substr(0, 2) == "//");
    s.remove_prefix(2);
    return s;
}

string_view
view::
encoded_username() const noexcept
{
    auto s = get(id_username);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() >= 2);
        BOOST_ASSERT(
            s.substr(0, 2) == "//");
        s.remove_prefix(2);
    }
    return s;
}

string_view
view::
encoded_password() const noexcept
{
    auto s = get(id_password);
    switch(s.size())
    {
    case 1:
        BOOST_ASSERT(s.front() == '@');
    case 0:
        return {};
    default:
        BOOST_ASSERT(s.back() == '@');
        s.remove_suffix(1);
        if(s.front() == ':')
            s.remove_prefix(1);
        return s;
    }
}

//
// host
//

string_view
view::
encoded_hostname() const noexcept
{
    return get(id_hostname);
}

string_view
view::
port_string() const noexcept
{
    auto s = get(id_port);
    BOOST_ASSERT(
        s.empty() || s.front() == ':');
    if(! s.empty())
        s.remove_prefix(1);
    return s;
}

//----------------------------------------------------------
//
// path
//
//----------------------------------------------------------

string_view
view::
encoded_segment(
    int pos) const
{
    auto s = get(id_segment(pos));
    if(! s.empty())
        s = s.substr(1);
    return s;
}

//----------------------------------------------------------
//
// query
//
//----------------------------------------------------------

//----------------------------------------------------------
//
// fragment
//
//----------------------------------------------------------

} // url
} // boost

#endif
