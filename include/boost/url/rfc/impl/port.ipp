//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_PORT_IPP
#define BOOST_URL_RFC_IMPL_PORT_IPP

#include <boost/url/rfc/port.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/bnf/token.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    port& t)
{
    bnf::token<
        bnf::char_set_function<
            &is_digit>> tk;
    using bnf::parse;
    auto it = parse(
        start, end, ec, tk);
    if(ec)
        return start;
    t.s_ = tk.str();
    t.v_.reset();
    return it;
}

} // rfc
} // urls
} // boost

#endif
