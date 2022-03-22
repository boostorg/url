//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2021 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_PARAMS_TABLE_HPP
#define BOOST_URL_DETAIL_PARAMS_TABLE_HPP

#include <boost/url/string_view.hpp>
#include <cstdint>
#include <type_traits>

namespace boost {
namespace urls {

namespace detail {

//------------------------------------------------

// params array stored at the
// end of allocated url string data

struct params_table_entry
{
    off_t kp;   // key pos
    off_t kn;   // key size
};

//------------------------------------------------

struct const_params_table
{
    explicit
    const_params_table(
        void const* end) noexcept
        : p_(reinterpret_cast<
            params_table_entry const*>(
                end))
    {
    }

    params_table_entry const&
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
                params_table_entry*>(
                    dest) - n,
            p_ - n,
            n * sizeof(
                params_table_entry));
    }

private:
    params_table_entry const* p_;
};

//------------------------------------------------

struct params_table
{
    explicit
    params_table(
        void* end) noexcept
        : p_(reinterpret_cast<
            params_table_entry*>(
                end))
    {
    }

    params_table_entry&
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
        const_params_table{p_}.copy(dest, n);
    }

private:
    params_table_entry* p_;
};

} // detail
} // urls
} // boost

#endif
