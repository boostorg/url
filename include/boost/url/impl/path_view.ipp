//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PATH_VIEW_IPP
#define BOOST_URL_IMPL_PATH_VIEW_IPP

#include <boost/url/path_view.hpp>
#include <boost/url/error.hpp>
#include <boost/url/rfc/query_bnf.hpp>
#include <boost/url/rfc/detail/paths_bnf.hpp>
#include <boost/url/detail/except.hpp>

namespace boost {
namespace urls {

path_view::
path_view() noexcept
    : s_("")
    , n_(0)
{
}

path_view::
iterator::
iterator(
    string_view s)
    : next_(s.data())
    , end_(s.data() + s.size())
{
    if(next_ == end_)
    {
        next_ = nullptr;
        return;
    }
    error_code ec;
    detail::
        path_abempty_bnf::begin(
            next_, end_, ec, v_.s_);
    if(ec)
        detail::throw_system_error(
            ec, BOOST_CURRENT_LOCATION);
}

auto
path_view::
iterator::
operator++() noexcept ->
    iterator&
{
    error_code ec;
    detail::
        path_abempty_bnf::increment(
            next_, end_, ec, v_.s_);
    if(ec == error::end)
    {
        next_ = nullptr;
        return *this;
    }
    if(ec)
        detail::throw_system_error(
            ec, BOOST_CURRENT_LOCATION);
    return *this;
}

//------------------------------------------------

auto
path_view::
begin() const noexcept ->
    iterator
{
    return iterator(s_);
}

auto
path_view::
end() const noexcept ->
    iterator
{
    return iterator(
        s_.data() + s_.size());
}

//------------------------------------------------

path_view
parse_path(
    string_view s,
    error_code& ec)
{
    using bnf::parse;
    bnf::range<
        pct_encoded_str> t;
    if(! parse(s, ec,
        detail::path_abempty_bnf{t}))
        return {};
    return path_view(
        t.str(), t.size());
}

path_view
parse_path(
    string_view s)
{
    error_code ec;
    auto p = parse_path(s, ec);
    detail::maybe_throw(ec,
        BOOST_CURRENT_LOCATION);
    return p;
}

} // urls
} // boost

#endif
