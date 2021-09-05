//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PORT_BNF_HPP
#define BOOST_URL_RFC_PORT_BNF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/optional.hpp>
#include <cstdint>

namespace boost {
namespace urls {
namespace rfc {

/** BNF for port

    @par BNF
    @code
    port          = *DIGIT
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2
*/
struct port_bnf
{
    using number_type =
        std::uint16_t;

    string_view str;
    optional<
        std::uint16_t> number;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        port_bnf& t);
};

} // rfc
} // urls
} // boost

#endif
