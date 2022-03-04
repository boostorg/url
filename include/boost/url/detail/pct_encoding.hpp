//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_PCT_ENCODING_HPP
#define BOOST_URL_DETAIL_PCT_ENCODING_HPP

#include <boost/url/pct_encoding_types.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/assert.hpp>
#include <memory>

namespace boost {
namespace urls {
namespace detail {

BOOST_URL_DECL
bool
key_equal_encoded(
    string_view plain_key,
    pct_encoded_str encoded) noexcept;

BOOST_URL_DECL
bool
key_equal_encoded(
    string_view plain_key,
    string_view encoded) noexcept;

// compare two string_views as if they are both
// percent-decoded
int
pct_decode_compare_unchecked(
    string_view lhs,
    string_view rhs) noexcept;

// compare two string_views as if they are both
// percent-decoded and lowercase
int
pct_decode_icompare_unchecked(
    string_view lhs,
    string_view rhs) noexcept;

// check if string_view lhs starts with string_view
// rhs as if they are both percent-decoded. If
// lhs starts with rhs, return number of chars
// matched in the encoded string_view
std::size_t
pct_decode_starts_with_unchecked(
    string_view lhs,
    string_view rhs) noexcept;

// check if string_view lhs ends with string_view
// rhs as if they are both percent-decoded. If
// lhs ends with rhs, return number of chars
// matched in the encoded string_view
std::size_t
pct_decode_ends_with_unchecked(
    string_view lhs,
    string_view rhs) noexcept;

// compare two string_views as if they are both
// lowercase
int
icompare(
    string_view lhs,
    string_view rhs) noexcept;


} // detail
} // urls
} // boost

#endif
