//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//


#ifndef BOOST_URL_DETAIL_TYPE_ID_HPP
#define BOOST_URL_DETAIL_TYPE_ID_HPP

namespace boost {
namespace urls {
namespace detail {

using type_id_t = void const *;

template <class T>
type_id_t
get_type_id() noexcept
{
    static constexpr char c{};
    return &c;
}

} // namespace detail
} // namespace urls
} // namespace boost

#endif
