//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_HIER_PART_IPP
#define BOOST_URL_RFC_IMPL_HIER_PART_IPP

#include <boost/url/rfc/hier_part.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/pct_encoded.hpp>

namespace boost {
namespace urls {
namespace rfc {

namespace detail {

struct segment
{
    pct_encoded_value& v;

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        segment const& t)
    {
        pct_encoded<
            unreserved_char_mask |
            sub_delims_char_mask |
            colon_char_mask |
            at_char_mask> p;
        using bnf::parse;
        auto it = parse(
            start, end, ec, p);
        if(ec)
            return start;
        t.v = p.value();
        return it;
    }
};

struct segment_nz
{
    pct_encoded_value& v;

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        segment_nz const& t)
    {
        pct_encoded<
            unreserved_char_mask |
            sub_delims_char_mask |
            colon_char_mask |
            at_char_mask> p;
        using bnf::parse;
        auto it = parse(
            start, end, ec, p);
        if(ec)
            return start;
        if(it == start)
        {
            // can't be empty
            ec = error::syntax;
            return start;
        }
        t.v = p.value();
        return it;
    }
};

struct segment_nz_nc
{
    pct_encoded_value& v;

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        segment_nz_nc const& t)
    {
        pct_encoded<
            unreserved_char_mask |
            sub_delims_char_mask |
            at_char_mask> p;
        using bnf::parse;
        auto it = parse(
            start, end, ec, p);
        if(ec)
            return start;
        if(it == start)
        {
            // can't be empty
            ec = error::syntax;
            return start;
        }
        t.v = p.value();
        return it;
    }
};

struct path_rootless
{
    string_view& v;

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        path_rootless const& t)
    {
        using bnf::parse;
        pct_encoded_value p;
        // segment-nz
        auto it = parse(
            start, end, ec,
            segment_nz{p});
        if(ec)
            return start;
        // *( "/" segment )
        for(;;)
        {
            it = parse(
                it, end, ec, '/',
                segment{p});
            if(ec)
            {
                ec = {};
                break;
            }
        }
        t.v = string_view(
            start, it - start);
        return it;
    }
};

} // detail

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    hier_part& t)
{
    using path_type =
        hier_part::path_type;
    using bnf::parse;
    if(start == end)
    {
        // path-empty
        t.a_.reset();
        // point to input
        t.v_ = path_type(
            string_view(start, 0));
        ec = {};
        return start;
    }
    auto it = start;
    if(*it != '/')
    {
        // path-rootless
        string_view s;
        it = parse(
            start, end, ec,
            detail::path_rootless{s});
        if(ec)
            return start;
        t.a_.reset();
        t.v_ = path_type(s);
        return it;
    }
    ++it;
    if(it == end)
    {
        // path-absolute, just "/"
        t.a_.reset();
        t.v_ = path_type(
            string_view(start,
                it - start));
        ec = {};
        return start;

    }
    if(*it != '/')
    {
        // path-absolute
        string_view s;
        it = parse(
            it, end, ec,
            detail::path_rootless{s});
        if(ec)
            return start;
        t.a_.reset();
        t.v_ = path_type(string_view(
            start, it - start));
        return it;
    }
    // "//" authority path-abempty
    ++it;
    authority a;
    // authority
    it = parse(it, end, ec, a);
    if(ec)
        return start;
    t.a_.emplace(a);
    // path-abempty
    auto it0 = it;
    for(;;)
    {
        pct_encoded_value p;
        it = parse(it, end, ec, '/',
            detail::segment{p});
        if(ec)
        {
            ec = {};
            break;
        }
    }
    t.v_ = path_type(string_view(
        it0, it - it0));
    return it;
}

} // rfc
} // urls
} // boost

#endif
