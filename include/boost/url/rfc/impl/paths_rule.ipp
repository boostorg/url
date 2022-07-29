//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PATHS_RULE_IPP
#define BOOST_URL_RFC_IMPL_PATHS_RULE_IPP

#include <boost/url/rfc/paths_rule.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/rfc/detail/segment_rule.hpp>
#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/sequence_rule.hpp>

namespace boost {
namespace urls {

auto
path_abempty_rule::
parse(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<value_type>
{
    return grammar::parse_range(
        it, end, *this,
        &path_abempty_rule::begin,
        &path_abempty_rule::increment);
}

auto
path_abempty_rule::
begin(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<pct_encoded_view>
{
    return increment(it, end);
}

auto
path_abempty_rule::
increment(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<pct_encoded_view>
{
    auto it0 = it;
    auto rv = grammar::parse(
        it, end,
        grammar::sequence_rule(
            grammar::char_rule('/'),
            detail::segment_rule));
    if(rv.has_value())
        return std::get<1>(*rv);
    it = it0;
    return BOOST_URL_ERR(
        grammar::error::end);
}

//------------------------------------------------

auto
path_absolute_rule::
parse(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<value_type>
{
    return grammar::parse_range(
        it, end, *this,
        &path_absolute_rule::begin,
        &path_absolute_rule::increment);
}

auto
path_absolute_rule::
begin(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<pct_encoded_view>
{
    if(it == end)
    {
        // expected '/'
        return error::missing_path_segment;
    }
    if(*it != '/')
    {
        // expected '/'
        return error::missing_path_separator;
    }
    ++it;
    if(it == end)
        return pct_encoded_view{};
    if(*it == '/')
    {
        // can't begin with "//"
        return error::empty_path_segment;
    }
    return grammar::parse(
        it, end, detail::segment_rule);
}

auto
path_absolute_rule::
increment(
    char const*& it,
    char const* const end
        ) const noexcept ->
    result<pct_encoded_view>
{
    auto const it0 = it;
    auto rv = grammar::parse(
        it, end,
        grammar::sequence_rule(
            grammar::char_rule('/'),
            detail::segment_rule));
    if(rv.has_value())
        return std::get<1>(*rv);
    it = it0;
    return grammar::error::end;
}

//------------------------------------------------

auto
path_noscheme_rule::
parse(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<value_type>
{
    return grammar::parse_range(
        it, end, *this,
        &path_noscheme_rule::begin,
        &path_noscheme_rule::increment);
}

auto
path_noscheme_rule::
begin(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<pct_encoded_view>
{
    return grammar::parse(
        it, end,
        detail::segment_nz_nc_rule);
}

auto
path_noscheme_rule::
increment(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<pct_encoded_view>
{
    auto const it0 = it;
    auto rv = grammar::parse(
        it, end,
        grammar::sequence_rule(
            grammar::char_rule('/'),
            detail::segment_rule));
    if(rv.has_value())
        return std::get<1>(*rv);
    it = it0;
    return grammar::error::end;
}

//------------------------------------------------

auto
path_rootless_rule::
parse(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<value_type>
{
    return grammar::parse_range(
        it, end, *this,
        &path_rootless_rule::begin,
        &path_rootless_rule::increment);
}

auto
path_rootless_rule::
begin(
    char const*& it,
    char const* end
        ) const noexcept ->
     result<pct_encoded_view>
{
    return grammar::parse(
        it, end,
        detail::segment_nz_rule);
}

auto
path_rootless_rule::
increment(
    char const*& it,
    char const* end
        ) const noexcept ->
    result<pct_encoded_view>
{
    auto const it0 = it;
    auto rv = grammar::parse(
        it, end,
        grammar::sequence_rule(
            grammar::char_rule('/'),
            detail::segment_rule));
    if(rv.has_value())
        return std::get<1>(*rv);
    it = it0;
    return grammar::error::end;
}

} // urls
} // boost

#endif
