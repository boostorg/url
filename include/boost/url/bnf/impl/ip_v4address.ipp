//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_IP_V4ADDRESS_IPP
#define BOOST_URL_BNF_IMPL_IP_V4ADDRESS_IPP

#include <boost/url/bnf/ip_v4address.hpp>
#include <boost/url/error.hpp>
#include <boost/url/bnf/char_sets.hpp>

namespace boost {
namespace urls {
namespace bnf {

class ip_v4address::dec_octet
{
public:
    using value_type = std::uint8_t;

    value_type
    value() const noexcept
    {
        return v_;
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec)
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
            v_ = static_cast<
                std::uint8_t>(v);
            ec = {};
            return it;
        }
        if(! is_digit(*it))
        {
            v_ = static_cast<
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
            v_ = static_cast<
                std::uint8_t>(v);
            ec = {};
            return it;
        }
        if(! is_digit(*it))
        {
            v_ = static_cast<
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
        v_ = static_cast<
            std::uint8_t>(v);
        ec = {};
        return it;
    }

private:
    value_type v_;
};

char const*
ip_v4address::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    dec_octet p;

    auto it = p.parse(
        start, end, ec);
    if(ec)
        return start;
    v_.addr[0] = p.value();
    if(it == end)
    {
        ec = error::need_more;
        return start;
    }
    if(*it != '.')
    {
        ec = error::syntax;
        return start;
    }
    ++it;

    it = p.parse(it, end, ec);
    if(ec)
        return start;
    v_.addr[1] = p.value();
    if(it == end)
    {
        ec = error::need_more;
        return start;
    }
    if(*it != '.')
    {
        ec = error::syntax;
        return start;
    }
    ++it;

    it = p.parse(it, end, ec);
    if(ec)
        return start;
    v_.addr[2] = p.value();
    if(it == end)
    {
        ec = error::need_more;
        return start;
    }
    if(*it != '.')
    {
        ec = error::syntax;
        return start;
    }
    ++it;

    it = p.parse(it, end, ec);
    if(ec)
        return start;
    v_.addr[3] = p.value();
    ec = {};
    return it;
}

} // bnf
} // urls
} // boost

#endif
