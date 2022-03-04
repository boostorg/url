//
// Copyright (c) 2022 alandefreitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_REMOVE_DOT_SEGMENTS_HPP
#define BOOST_URL_DETAIL_REMOVE_DOT_SEGMENTS_HPP

#include <boost/url/string_view.hpp>
#include <cstdint>

namespace boost {
namespace urls {
namespace detail {

std::size_t
remove_dot_segments(
    char* dest,
    char const* end,
    string_view s,
    bool remove_unmatched) noexcept;

int
normalized_path_compare(
    string_view lhs,
    string_view rhs,
    bool remove_unmatched_lhs,
    bool remove_unmatched_rhs) noexcept;

} // detail
} // urls
} // boost

#endif
