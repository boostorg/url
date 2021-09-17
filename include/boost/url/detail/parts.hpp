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
        id_scheme = -1, // trailing ':'
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

class parts : private part_ids
{
    pos_t offset_[id_end + 1] = {};

public:
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

    static
    constexpr
    pos_t zero_ = 0;

    constexpr
    pos_t
    offset(int id) const noexcept
    {
        return
            id == id_scheme ?
            zero_ : offset_[id];
    }

    // size of string, without null
    constexpr
    std::size_t
    size() const noexcept
    {
        return offset(id_end);
    }

    // size of table in pos_t
    std::size_t
    tabsize() const noexcept
    {
        auto n = nseg;
        if( n < nparam)
            n = nparam;
        return 2 * n;
    }

    // size of id
    constexpr
    pos_t
    len(int id) const noexcept
    {
        return
            offset(id + 1) -
            offset(id);
    }

    // size of [begin, end)
    pos_t
    len(
        int begin,
        int end) const noexcept
    {
        BOOST_ASSERT(begin <= end);
        BOOST_ASSERT(end <= id_end);
        return offset(end) - offset(begin);
    }

    // return id as string
    constexpr
    string_view
    get(int id,
        char const* s) const noexcept
    {
        return {
            s + offset(id), len(id) };
    }

    // return [first, last) as string
    constexpr
    string_view
    get(int first,
        int last,
        char const* s) const noexcept
    {
        return { s + offset(first),
            offset(last) - offset(first) };
    }

    // change id to size n
    void
    set_size(
        int id,
        pos_t n) noexcept
    {
        auto d = n - len(id);
        for(auto i = id + 1;
            i <= id_end; ++i)
            offset_[i] += d;
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
        offset_[id + 1] = offset(id) + n;
    }

    // add n to [first, last]
    void
    adjust(
        int first,
        int last,
        std::size_t n) noexcept
    {
        for(int i = first;
                i <= last; ++i)
            offset_[i] += n;
    }

    // set [first, last) offset
    void
    collapse(
        int first,
        int last,
        std::size_t n) noexcept
    {
        for(int i = first + 1;
                i < last; ++i)
            offset_[i] = n;
    }
};

} // detail
} // urls
} // boost

#endif
