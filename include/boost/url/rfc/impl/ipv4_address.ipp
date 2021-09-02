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
#include <boost/url/bnf/char_sets.hpp>
#include <boost/url/bnf/literal.hpp>
#include <boost/url/bnf/sequence.hpp>

namespace boost {
namespace urls {
namespace rfc {

class ipv4_address::dec_octet
{
public:
    std::uint8_t const*
    operator->() const noexcept
    {
        return &v_;
    }

    std::uint8_t
    operator*() const noexcept
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
        if(! bnf::is_digit(*it))
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
        if(! bnf::is_digit(*it))
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
        if(! bnf::is_digit(*it))
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
    std::uint8_t v_;
};

char const*
ipv4_address::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    using namespace bnf;
    sequence<
        dec_octet, literal<'.'>,
        dec_octet, literal<'.'>,
        dec_octet, literal<'.'>,
        dec_octet> p;
    auto it = p.parse(
        start, end, ec);
    if(ec)
        return start;
    v_[0] = *get<0>(p);
    v_[1] = *get<2>(p);
    v_[2] = *get<4>(p);
    v_[3] = *get<6>(p);
    return it;
}

} // rfc
} // urls
} // boost

#endif
