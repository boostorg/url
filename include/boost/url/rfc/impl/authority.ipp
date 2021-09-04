//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_AUTHORITY_IPP
#define BOOST_URL_RFC_IMPL_AUTHORITY_IPP

#include <boost/url/rfc/authority.hpp>
#include <boost/url/bnf/literal.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/host.hpp>
#include <boost/url/rfc/port.hpp>
#include <boost/url/rfc/userinfo.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    authority& t)
{
    using bnf::parse;
    auto it = start;
    // [ userinfo "@" ]
    {
        userinfo u;
        it = parse(it, end,
            ec, u, '@');
        if(! ec.failed())
        {
            t.u_.emplace(u);
        }
        else
        {
            t.u_.reset();
            ec = {};
        }
    }
    // host
    it = parse(
        it, end, ec, t.h_);
    if(ec)
        return start;
    // [ ":" port ]
    {
        // ":"
        if(it == end)
        {
            t.p_.reset();
            return it;
        }
        if(*it != ':')
        {
            t.p_.reset();
            return it;
        }
        ++it;
        // port
        port p;
        it = parse(
            it, end, ec, p);
        if(ec)
        {
            // never happens
            BOOST_ASSERT(
                ! ec.failed());
            return start;
        }
        t.p_.emplace(p);
    }
    return it;
}

} // rfc
} // urls
} // boost

#endif
