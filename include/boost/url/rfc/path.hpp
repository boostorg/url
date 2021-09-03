//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PATH_HPP
#define BOOST_URL_RFC_PATH_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/rfc/pct_encoded.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** Element type for a path expressed as a range
*/
struct path
{
    using value_type =
        pct_encoded_value;

    BOOST_URL_DECL
    static
    char const*
    begin(
        char const* const start,
        char const* const end,
        error_code& ec,
        value_type& t);

    BOOST_URL_DECL
    static
    char const*
    increment(
        char const* const start,
        char const* const end,
        error_code& ec,
        value_type& t);
};

} // rfc
} // urls
} // boost

#endif
