//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_SCHEME_IPP
#define BOOST_URL_IMPL_SCHEME_IPP

#include <boost/url/scheme.hpp>
#include <boost/url/detail/char_type.hpp>

namespace boost {
namespace urls {

scheme
string_to_scheme(
    string_view s) noexcept
{
    using detail::to_lower;

    switch(s.size())
    {
    case 2: // ws
        if( tolower(s[0]) == 'w' &&
            tolower(s[1]) == 's')
            return scheme::ws;
        break;

    case 3:
        switch(tolower(s[0]))
        {
        case 'w': // wss
            if( tolower(s[1]) == 's' &&
                tolower(s[2]) == 's')
                return scheme::wss;
            break;

        case 'f': // ftp
            if( tolower(s[1]) == 't' &&
                tolower(s[2]) == 'p')
                return scheme::ftp;
            break;

        default:
            break;
        }
        break;

    case 4:
        switch(tolower(s[0]))
        {
        case 'f': // file
            if( tolower(s[1]) == 'i' &&
                tolower(s[2]) == 'l' &&
                tolower(s[3]) == 'e')
                return scheme::file;
            break;

        case 'h': // http
            if( tolower(s[1]) == 't' &&
                tolower(s[2]) == 't' &&
                tolower(s[3]) == 'p')
                return scheme::http;
            break;

        default:
            break;
        }
        break;

    case 5: // https
        if( tolower(s[0]) == 'h' &&
            tolower(s[1]) == 't' &&
            tolower(s[2]) == 't' &&
            tolower(s[3]) == 'p' &&
            tolower(s[4]) == 's')
            return scheme::https;
        break;

    default:
        break;
    }
    return scheme::unknown;
}

string_view
to_string(scheme s) noexcept
{
    switch(s)
    {
    case scheme::ftp:   return "ftp";
    case scheme::file:  return "file";
    case scheme::http:  return "http";
    case scheme::https: return "https";
    case scheme::ws:    return "ws";
    case scheme::wss:   return "wss";

    default:
        break;
    }
    return "<unknown>";
}

} // urls
} // boost

#endif
