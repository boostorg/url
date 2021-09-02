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
#include <boost/url/rfc/char_sets.hpp>
#include <boost/url/rfc/hexdig.hpp>
#include <boost/url/rfc/ipv4_address.hpp>
#include <array>
#include <memory>

namespace boost {
namespace urls {
namespace rfc {

//------------------------------------------------

class ipv6_address::h16
{
public:
    using value_type = std::array<
        std::uint8_t, 2>;

    value_type const&
    operator*() const noexcept
    {
        return v_;
    }

    value_type const*
    operator->() const noexcept
    {
        return &v_;
    }

    // return `true` if the hex
    // word could be 0..255 if
    // interpreted as decimal
    bool
    is_octet() const noexcept
    {
        unsigned short word =
            static_cast<unsigned short>(
                v_[0]) * 256 +
            static_cast<unsigned short>(
                v_[1]);
        if(word > 0x255)
            return false;
        if(((word >>  4) & 0xf) > 9)
            return false;
        if((word & 0xf) > 9)
            return false;
        return true;
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec)
    {
        std::uint16_t v;
        auto it = start;
        for(;;)
        {
            if(start == end)
            {
                ec = error::need_more;
                return start;
            }
            auto d = hex_digit(*it);
            if(d == -1)
            {
                ec = error::syntax;
                return start;
            }
            v = d;
            ++it;
            if(it == end)
                break;
            d = hex_digit(*it);
            if(d == -1)
                break;
            v = (16 * v) + d;
            ++it;
            if(it == end)
                break;
            d = hex_digit(*it);
            if(d == -1)
                break;
            v = (16 * v) + d;
            ++it;
            if(it == end)
                break;
            d = hex_digit(*it);
            if(d == -1)
                break;
            v = (16 * v) + d;
            ++it;
            break;
        }
        ec = {};
        v_[0] = static_cast<
            std::uint8_t>(
                v / 256);
        v_[1] = static_cast<
            std::uint8_t>(
                v % 256);
        return it;
    }

private:
    value_type v_;
};

//------------------------------------------------

char const*
ipv6_address::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    h16 hp;
    int  n = 8;     // words needed
    int  b = -1;    // value of n
                    // when '::' seen
    bool c = false; // need colon
    auto prev = start;
    auto it = start;
    v_.trailing_ipv4 = false;
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
            ec = error::need_more;
            return start;
        }
        if(*it == ':')
        {
            ++it;
            if(it == end)
            {
                ec = error::need_more;
                return start;
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
                return start;
            }
            if(c)
            {
                prev = it;
                it = hp.parse(
                    it, end, ec);
                if(ec)
                    return start;
                v_.octets[2*(8-n)+0] =
                    (*hp)[0];
                v_.octets[2*(8-n)+1] =
                    (*hp)[1];
                --n;
                if(n == 0)
                    break;
                continue;
            }
            // expected h16
            ec = error::syntax;
            return start;
        }
        if(*it == '.')
        {
            if(b == -1 && n > 1)
            {
                // not enough h16
                ec = error::syntax;
                return start;
            }
            if(! hp.is_octet())
            {
                // invalid octet
                ec = error::syntax;
            }
            // rewind the h16 and
            // parse it as ipv4
            ipv4_address p;
            it = p.parse(
                prev, end, ec);
            if(ec)
                return start;
            v_.octets[2*(7-n)+0] = (*p)[0];
            v_.octets[2*(7-n)+1] = (*p)[1];
            v_.octets[2*(7-n)+2] = (*p)[2];
            v_.octets[2*(7-n)+3] = (*p)[3];
            v_.trailing_ipv4 = true;
            --n;
            break;
        }
        if( b != -1 &&
            hex_digit(*it) == -1)
        {
            // ends in "::"
            break;
        }
        if(! c)
        {
            prev = it;
            it = hp.parse(
                it, end, ec);
            if(ec)
                return start;
            v_.octets[2*(8-n)+0] =
                (*hp)[0];
            v_.octets[2*(8-n)+1] =
                (*hp)[1];
            --n;
            if(n == 0)
                break;
            c = true;
            continue;
        }
        // ':' divides a word
        ec = error::syntax;
        return start;
    }
    if(b == -1)
        return it;
    if(b == n)
    {
        // "::" last
        auto const i =
            2 * (7 - n);
        std::memset(
            &v_.octets[i],
            0, 16 - i);
    }
    else if(b == 7)
    {
        // "::" first
        auto const i =
            2 * (b - n);
        std::memmove(
            &v_.octets[16 - i],
            &v_.octets[2],
            i);
        std::memset(
            &v_.octets[0],
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
            &v_.octets[16 - i1],
            &v_.octets[i0 + 2],
            i1);
        std::memset(
            &v_.octets[i0],
            0, 16 - (i0 + i1));
    }
    return it;
}

} // rfc
} // urls
} // boost

#endif
