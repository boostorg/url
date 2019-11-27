//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_SCHEME_IPP
#define BOOST_URL_IMPL_SCHEME_IPP

#include <boost/beast/url/detail/rfc3986.hpp>

namespace boost {
namespace beast {
namespace url {

namespace detail {

template<class = void>
known_scheme
string_to_scheme(string_view s)
{
    switch(s.size())
    {
    case 2: // ws
        if( tolower(s[0]) == 'w' &&
            tolower(s[1]) == 's')
            return known_scheme::ws;
        break;

    case 3:
        switch(tolower(s[0]))
        {
        case 'w': // wss
            if( tolower(s[1]) == 's' &&
                tolower(s[2]) == 's')
                return known_scheme::wss;
            break;

        case 'f': // ftp
            if( tolower(s[1]) == 't' &&
                tolower(s[2]) == 'p')
                return known_scheme::ftp;
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
                return known_scheme::file;
            break;

        case 'h': // http
            if( tolower(s[1]) == 't' &&
                tolower(s[2]) == 't' &&
                tolower(s[3]) == 'p')
                return known_scheme::http;
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
            return known_scheme::https;
        break;

    case 6:
        if( tolower(s[0]) == 'g' &&
            tolower(s[1]) == 'o' &&
            tolower(s[2]) == 'p' &&
            tolower(s[3]) == 'h' &&
            tolower(s[4]) == 'e' &&
            tolower(s[5]) == 'r')
            return known_scheme::gopher;
        break;

    default:
        break;
    }
    return known_scheme::unknown;
}

template<class = void>
string_view
to_string(known_scheme s)
{
    switch(s)
    {
    case known_scheme::ftp:       return "ftp";
    case known_scheme::file:      return "file";
    case known_scheme::gopher:    return "gopher";
    case known_scheme::http:      return "http";
    case known_scheme::https:     return "https";
    case known_scheme::ws:        return "ws";
    case known_scheme::wss:       return "wss";
    default:
        break;
    }
    return "<unknown>";
}

template<class = void>
bool
is_special(known_scheme s)
{
    switch(s)
    {
    case known_scheme::ftp:
    case known_scheme::file:
    case known_scheme::gopher:
    case known_scheme::http:
    case known_scheme::https:
    case known_scheme::ws:
    case known_scheme::wss:
        return true;

    default:
        break;
    }
    return false;
}

} // detail

inline
known_scheme
string_to_scheme(string_view s)
{
    return detail::string_to_scheme(s);
}

inline
string_view
to_string(known_scheme s)
{
    return detail::to_string(s);
}

inline
bool
is_special(known_scheme s)
{
    return detail::is_special(s);
}

} // url
} // beast
} // boost

#endif
