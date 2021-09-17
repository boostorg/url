//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_PARTS_HPP
#define BOOST_URL_DETAIL_PARTS_HPP

#include <boost/url/host_type.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/string.hpp>
#include <cstdint>

namespace boost {
namespace urls {
namespace detail {

struct part_ids
{
    using pos_t = std::size_t;

    enum
    {
        id_scheme = 0,  // trailing ':'
        id_user,        // leading "//"
        id_pass,        // leading ':', trailing '@'
        id_host,
        id_port,        // leading ':'
        id_path,
        id_query,       // leading '?'
        id_frag,        // leading '#'
        id_end          // one past the end
    };
};

//----------------------------------------------------------

struct parts : private part_ids
{
    pos_t offset[id_end + 1] = {};
    pos_t decoded[id_end] = {};
    pos_t nseg = 0;
    pos_t nparam = 0;
    unsigned char ip_addr[16] = {};
    // VFALCO don't we need a bool?
    std::uint16_t port_number = 0;
    urls::host_type host_type =
        urls::host_type::none;
    urls::scheme scheme =
        urls::scheme::none;

    // size of table
    std::size_t
    tabsize() const noexcept
    {
        auto n = nseg;
        if( n < nparam)
            n = nparam;
        return sizeof(pos_t) *
            2 * (n + 1);
    }

    // size of id
    pos_t
    len(int id) const noexcept
    {
        return offset[id + 1] -
            offset[id];
    }

    // size of [begin, end)
    pos_t
    len(
        int begin,
        int end) const noexcept
    {
        BOOST_ASSERT(begin <= end);
        BOOST_ASSERT(end <= id_end);
        return offset[end] -
            offset[begin];
    }

    // return id as string
    string_view
    get(int id,
        char const* s) const noexcept
    {
        return {
            s + offset[id],
            offset[id + 1] -
                offset[id] };
    }

    // return [begin, end) as string
    string_view
    get(int begin,
        int end,
        char const* s) const noexcept
    {
        return {
            s + offset[begin],
            offset[end] -
                offset[begin] };
    }

    // change id to size n
    void
    resize(
        int id,
        pos_t n) noexcept
    {
        auto const n0 = len(id);
        auto const d = n - n0;
        for(auto i = id + 1;
            i <= id_end; ++i)
            offset[i] += d;
    }

    // trim id to size n,
    // moving excess into id+1
    void
    split(
        int id,
        std::size_t n) noexcept
    {
        BOOST_ASSERT(id < id_end - 1);
        BOOST_ASSERT(n <= len(id));
        offset[id + 1] = offset[id] + n;
    }
};

} // detail
} // urls
} // boost

#endif
