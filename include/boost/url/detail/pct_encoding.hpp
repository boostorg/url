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

} // detail
} // urls
} // boost

#endif
