//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_NORMALIZED_HPP
#define BOOST_URL_DETAIL_NORMALIZED_HPP

#include <boost/url/pct_encoding_types.hpp>
#include <boost/url/string_view.hpp>

namespace boost {
namespace urls {
namespace detail {

// compare two string_views as if they are both
// percent-decoded
int
compare_encoded(
    string_view lhs,
    string_view rhs) noexcept;

// check if string_view lhs starts with string_view
// rhs as if they are both percent-decoded. If
// lhs starts with rhs, return number of chars
// matched in the encoded string_view
std::size_t
path_starts_with(
    string_view lhs,
    string_view rhs) noexcept;

// check if string_view lhs ends with string_view
// rhs as if they are both percent-decoded. If
// lhs ends with rhs, return number of chars
// matched in the encoded string_view
std::size_t
path_ends_with(
    string_view lhs,
    string_view rhs) noexcept;

// compare two string_views as if they are both
// percent-decoded and lowercase
int
ci_compare_encoded(
    string_view lhs,
    string_view rhs) noexcept;

// compare two string_views as if they are both
// lowercase
int
ci_compare(
    string_view lhs,
    string_view rhs) noexcept;

} // detail
} // urls
} // boost

#endif
