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
#include <boost/url/rfc/detail/paths.hpp>

namespace boost {
namespace urls {
namespace rfc {

namespace detail {

//------------------------------------------------

// path-rootless = segment-nz *( "/" segment )
struct old_path_rootless
{
    string_view& v;

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        old_path_rootless const& t)
    {
        using bnf::parse;
        pct_encoded_str p;
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
    auto it = start;
    if(it == end)
    {
        // path-empty
        t.a_.reset();
        it = parse(start, end, ec,
            detail::path_empty{t.v_});
        ec = {};
        return it;
    }
    if(it[0] != '/')
    {
        // path-rootless
        it = parse(it, end, ec,
            detail::path_rootless{t.v_});
        if(ec)
            return start;
        t.a_.reset();
        return it;
    }
    if( end - it == 1 ||
        it[1] != '/')
    {
        // path-absolute
        it = parse(it, end, ec,
            detail::path_absolute{t.v_});
        t.a_.reset();
        ec = {};
        return it;
    }
    // "//" authority path-abempty
    it += 2;
    authority a;
    // authority
    it = parse(it, end, ec, a);
    if(ec)
        return start;
    t.a_.emplace(a);
    // path-abempty
    it = parse(it, end, ec,
        detail::path_abempty{t.v_});
    if(ec)
        return start;
    return it;
}

} // rfc
} // urls
} // boost

#endif
