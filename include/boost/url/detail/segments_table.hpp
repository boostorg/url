//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2021 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_SEGMENTS_TABLE_HPP
#define BOOST_URL_DETAIL_SEGMENTS_TABLE_HPP

#include <boost/url/string_view.hpp>
#include <cstdint>
#include <type_traits>

namespace boost {
namespace urls {

namespace detail {

//------------------------------------------------

// segments array stored at the
// end of allocated url string data

struct segments_table_entry
{
    off_t sp;   // segment pos
    off_t sn;   // segment size
};

//------------------------------------------------

struct const_segments_table
{
    explicit
    const_segments_table(
        void const* end) noexcept
        : p_(reinterpret_cast<
            segments_table_entry const*>(
                end))
    {
    }

    segments_table_entry const&
    operator[](
        std::size_t i) const noexcept
    {
        return *(p_ - i - 1);
    }

    void
    copy(
        void* dest,
        std::size_t n) const noexcept
    {
        std::memcpy(
            reinterpret_cast<
                segments_table_entry*>(
                    dest) - n,
            p_ - n,
            n * sizeof(
                segments_table_entry));
    }

private:
    segments_table_entry const* p_;
};

//------------------------------------------------

struct segments_table
{
    explicit
    segments_table(
        void* end) noexcept
        : p_(reinterpret_cast<
            segments_table_entry*>(
                end))
    {
    }

    segments_table_entry&
    operator[](
        std::size_t i) const noexcept
    {
        return *(p_ - i - 1);
    }

    void
    copy(
        void* dest,
        std::size_t n) const noexcept
    {
        const_segments_table{p_}.copy(dest, n);
    }

private:
    segments_table_entry* p_;
};

} // detail
} // urls
} // boost

#endif
