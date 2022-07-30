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
#include <boost/url/rfc/detail/path_increment.hpp>
#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/grammar/range.hpp>
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
        it, end, detail::path_increment);
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
    struct begin
    {
        using value_type = pct_encoded_view;

        result<value_type>
        parse(
            char const *&it,
            char const* end) const noexcept
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
    };

    return grammar::parse_range(
        it, end, begin{},
            detail::path_increment);
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
        it, end,
        detail::segment_nz_nc_rule,
        detail::path_increment);
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
        it, end,
        detail::segment_nz_rule,
        detail::path_increment);
}

} // urls
} // boost

#endif
