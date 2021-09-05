//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_IPV6_ADDRESS_IPP
#define BOOST_URL_BNF_IMPL_IPV6_ADDRESS_IPP

#include <boost/url/rfc/ipv6_address.hpp>
#include <boost/url/error.hpp>
#include <boost/url/bnf/char_set.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/ipv4_address.hpp>
#include <array>
#include <memory>

namespace boost {
namespace urls {
namespace rfc {

namespace detail {

struct h16
{
    std::uint8_t* p;

    // return `true` if the hex
    // word could be 0..255 if
    // interpreted as decimal
    static
    bool
    is_octet(std::uint8_t const* p) noexcept
    {
        unsigned short word =
            static_cast<unsigned short>(
                p[0]) * 256 +
            static_cast<unsigned short>(
                p[1]);
        if(word > 0x255)
            return false;
        if(((word >>  4) & 0xf) > 9)
            return false;
        if((word & 0xf) > 9)
            return false;
        return true;
    }

    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        h16 const& t)
    {
        std::uint16_t v;
        for(;;)
        {
            if(it == end)
            {
                // expected HEXDIG
                ec = error::syntax;
                return false;
            }
            auto d =
                bnf::hexdig_value(*it);
            if(d == -1)
            {
                ec = error::syntax;
                return false;
            }
            v = d;
            ++it;
            if(it == end)
                break;
            d = bnf::hexdig_value(*it);
            if(d == -1)
                break;
            v = (16 * v) + d;
            ++it;
            if(it == end)
                break;
            d = bnf::hexdig_value(*it);
            if(d == -1)
                break;
            v = (16 * v) + d;
            ++it;
            if(it == end)
                break;
            d = bnf::hexdig_value(*it);
            if(d == -1)
                break;
            v = (16 * v) + d;
            ++it;
            break;
        }
        ec = {};
        t.p[0] = static_cast<
            std::uint8_t>(
                v / 256);
        t.p[1] = static_cast<
            std::uint8_t>(
                v % 256);
        return true;
    }
};

} // detail

//------------------------------------------------

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    ipv6_address& t)
{
    using bnf::parse;
    int n = 8;      // words needed
    int b = -1;     // value of n
                    // when '::' seen
    bool c = false; // need colon
    auto prev = it;
    t.trailing_ipv4 = false;
    for(;;)
    {
        if(it == end)
        {
            if(b != -1)
            {
                // end in "::"
                break;
            }
            BOOST_ASSERT(n > 0);
            // not enough words
            ec = error::syntax;
            return false;
        }
        if(*it == ':')
        {
            ++it;
            if(it == end)
            {
                // missing ':'
                ec = error::syntax;
                return false;
            }
            if(*it == ':')
            {
                if(b == -1)
                {
                    // first "::"
                    ++it;
                    --n;
                    b = n;
                    if(n == 0)
                        break;
                    c = false;
                    continue;
                }
                // two "::"
                ec = error::syntax;
                return false;
            }
            if(c)
            {
                prev = it;
                if(! parse(it, end, ec, 
                    detail::h16{
                        &t.octets[2*(8-n)]}))
                    return false;
                --n;
                if(n == 0)
                    break;
                continue;
            }
            // expected h16
            ec = error::syntax;
            return false;
        }
        if(*it == '.')
        {
            if(b == -1 && n > 1)
            {
                // not enough h16
                ec = error::syntax;
                return false;
            }
            if(! detail::h16::is_octet(
                &t.octets[2*(7-n)]))
            {
                // invalid octet
                ec = error::syntax;
                return false;
            }
            // rewind the h16 and
            // parse it as ipv4
            ipv4_address v4;
            it = prev;
            if(! parse(
                it, end, ec, v4))
                return false;
            t.octets[2*(7-n)+0] =
                v4.octets[0];
            t.octets[2*(7-n)+1] =
                v4.octets[1];
            t.octets[2*(7-n)+2] =
                v4.octets[2];
            t.octets[2*(7-n)+3] =
                v4.octets[3];
            t.trailing_ipv4 = true;
            --n;
            break;
        }
        if( b != -1 &&
            bnf::hexdig_value(*it) == -1)
        {
            // ends in "::"
            break;
        }
        if(! c)
        {
            prev = it;
            if(! parse(it, end, ec,
                detail::h16{
                    &t.octets[2*(8-n)]}))
                return false;
            --n;
            if(n == 0)
                break;
            c = true;
            continue;
        }
        // ':' divides a word
        ec = error::syntax;
        return false;
    }
    ec = {};
    if(b == -1)
        return true;
    if(b == n)
    {
        // "::" last
        auto const i =
            2 * (7 - n);
        std::memset(
            &t.octets[i],
            0, 16 - i);
    }
    else if(b == 7)
    {
        // "::" first
        auto const i =
            2 * (b - n);
        std::memmove(
            &t.octets[16 - i],
            &t.octets[2],
            i);
        std::memset(
            &t.octets[0],
            0, 16 - i);
    }
    else
    {
        // "::" in middle
        auto const i0 =
            2 * (7 - b);
        auto const i1 =
            2 * (b - n);
        std::memmove(
            &t.octets[16 - i1],
            &t.octets[i0 + 2],
            i1);
        std::memset(
            &t.octets[i0],
            0, 16 - (i0 + i1));
    }
    return true;
}

} // rfc
} // urls
} // boost

#endif
