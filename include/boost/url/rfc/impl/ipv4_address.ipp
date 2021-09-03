//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_IP_V4ADDRESS_IPP
#define BOOST_URL_RFC_IMPL_IP_V4ADDRESS_IPP

#include <boost/url/rfc/ipv4_address.hpp>
#include <boost/url/error.hpp>
#include <boost/url/bnf/literal.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/bnf/sequence.hpp>
#include <boost/url/rfc/char_sets.hpp>

namespace boost {
namespace urls {
namespace rfc {

namespace detail {

struct dec_octet
{
    std::uint8_t value;

    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        dec_octet& t)
    {
        if(start == end)
        {
            ec = error::need_more;
            return start;
        }
        auto it = start;
        if(! is_digit(*it))
        {
            ec = error::syntax;
            return start;
        }
        unsigned v = *it - '0';
        ++it;
        if(it == end)
        {
            t.value = static_cast<
                std::uint8_t>(v);
            ec = {};
            return it;
        }
        if(! is_digit(*it))
        {
            t.value = static_cast<
                std::uint8_t>(v);
            ec = {};
            return it;
        }
        if(v == 0)
        {
            // bad leading '0'
            ec = error::syntax;
            return start;
        }
        v = (10 * v) + *it - '0';
        ++it;
        if(it == end)
        {
            t.value = static_cast<
                std::uint8_t>(v);
            ec = {};
            return it;
        }
        if(! is_digit(*it))
        {
            t.value = static_cast<
                std::uint8_t>(v);
            ec = {};
            return it;
        }
        if(v > 25)
        {
            // out of range
            ec = error::syntax;
            return start;
        }
        v = (10 * v) + *it - '0';
        if(v > 255)
        {
            // out of range
            ec = error::syntax;
            return start;
        }
        ++it;
        t.value = static_cast<
            std::uint8_t>(v);
        ec = {};
        return it;
    }
};

} // detail

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    ipv4_address& t)
{
    using namespace bnf;
    using bnf::parse;
    detail::dec_octet ip[4];
    auto it = parse(
        start, end, ec,
        ip[0], '.',
        ip[1], '.',
        ip[2], '.',
        ip[3]);
    if(ec)
        return start;
    t.octets[0] = ip[0].value;
    t.octets[1] = ip[1].value;
    t.octets[2] = ip[2].value;
    t.octets[3] = ip[3].value;
    return it;
}

} // rfc
} // urls
} // boost

#endif
