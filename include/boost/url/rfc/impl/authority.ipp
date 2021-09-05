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
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/host.hpp>
#include <boost/url/rfc/port.hpp>
#include <boost/url/rfc/userinfo.hpp>

namespace boost {
namespace urls {
namespace rfc {

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    authority& t)
{
    using bnf::parse;
    auto const start = it;
    // [ userinfo "@" ]
    {
        userinfo u;
        if(parse(it, end,
            ec, u, '@'))
        {
            t.userinfo.emplace(u);
        }
        else
        {
            t.userinfo.reset();
            it = start;
            ec = {};
        }
    }
    // host
    if(! parse(it,
        end, ec, t.host))
        return false;
    // [ ":" port ]
    {
        // ":"
        if(it == end)
        {
            t.port.reset();
            goto finish;
        }
        if(*it != ':')
        {
            t.port.reset();
            goto finish;
        }
        ++it;
        // port
        port p;
        if(! parse(
            it, end, ec, p))
        {
            // never happens
            BOOST_ASSERT(
                ! ec.failed());
            return false;
        }
        t.port.emplace(p);
    }
finish:
    t.str = string_view(
        start, it - start);
    return true;
}

} // rfc
} // urls
} // boost

#endif
