//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_ENCODED_VIEW_IPP
#define BOOST_URL_IMPL_SEGMENTS_ENCODED_VIEW_IPP

#include <boost/url/segments_view.hpp>
#include <boost/url/error.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/paths_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/path.hpp>
#include <ostream>

namespace boost {
namespace urls {

//------------------------------------------------

segments_encoded_view::
segments_encoded_view(
    string_view s,
    std::size_t nseg) noexcept
    : s_(s)
    , n_(nseg)
{
}

//------------------------------------------------
//
// Iterators
//
//------------------------------------------------

auto
segments_encoded_view::
begin() const noexcept ->
    iterator
{
    return iterator(s_, n_);
}

auto
segments_encoded_view::
end() const noexcept ->
    iterator
{
    return iterator(s_, n_, 0);
}

//------------------------------------------------
//
// Friends
//
//------------------------------------------------

result<segments_encoded_view>
parse_path(string_view s) noexcept
{
    if(s.empty())
        return segments_encoded_view();
    if(s[0] == '/')
        return parse_path_abempty(s);
    return parse_path_rootless(s);
}

result<segments_encoded_view>
parse_path_abempty(
    string_view s) noexcept
{
    error_code ec;
    path_abempty_rule t;
    if(! grammar::parse_string(s, ec, t))
        return ec;
    return segments_encoded_view(
        t.str, detail::path_segments(
            t.str, t.count));
}

result<segments_encoded_view>
parse_path_absolute(
    string_view s) noexcept
{
    error_code ec;
    path_absolute_rule t;
    if(! grammar::parse_string(s, ec, t))
        return ec;
    return segments_encoded_view(
        t.str, detail::path_segments(
            t.str, t.count));
}

result<segments_encoded_view>
parse_path_noscheme(
    string_view s) noexcept
{
    error_code ec;
    path_noscheme_rule t;
    if(! grammar::parse_string(s, ec, t))
        return ec;
    return segments_encoded_view(
        t.str, detail::path_segments(
            t.str, t.count));
}

result<segments_encoded_view>
parse_path_rootless(
    string_view s) noexcept
{
    error_code ec;
    path_rootless_rule t;
    if(! grammar::parse_string(s, ec, t))
        return ec;
    return segments_encoded_view(
        t.str, detail::path_segments(
            t.str, t.count));
}

} // urls
} // boost

#endif
