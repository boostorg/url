//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_DETAIL_CHAR_TABLE_HPP
#define BOOST_URL_RFC_DETAIL_CHAR_TABLE_HPP

#include <boost/url/detail/config.hpp>
#include <cstdint>

namespace boost {
namespace urls {
namespace rfc {
namespace detail {

BOOST_URL_DECL
extern
std::uint8_t
char_table[256];

} // detail
} // rfc
} // urls
} // boost

#endif
