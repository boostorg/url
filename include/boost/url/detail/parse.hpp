//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_PARSE_HPP
#define BOOST_URL_DETAIL_PARSE_HPP

#include <boost/url/detail/char_type.hpp>
#include <boost/url/error.hpp>

namespace boost {
namespace url {
namespace detail {

/*
    https://en.wikipedia.org/wiki/Uniform_Resource_Identifier

    Generic Syntax
    
    URI = scheme:[//authority]path[?query][#fragment]

    ----------------------------------------------------------------------

    https://tools.ietf.org/html/rfc3986#section-3.2

    URI-reference = URI / relative-ref

    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

    absolute-URI  = scheme ":" hier-part [ "?" query ]

    scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )

    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]

    hier-part     = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty

    relative-part = "//" authority path-abempty
                  / path-absolute
                  / path-noscheme
                  / path-empty

    authority     = [ userinfo "@" ] host [ ":" port ]
    userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    host          = IP-literal / IPv4address / reg-name
    port          = *DIGIT

    path-abempty  = *( "/" segment )
    path-absolute = "/" [ segment-nz *( "/" segment ) ]
    path-noscheme = segment-nz-nc *( "/" segment )
    path-rootless = segment-nz *( "/" segment )
    path-empty    = 0<pchar>

    segment       = *pchar
    segment-nz    = 1*pchar
    segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
                    ; non-zero-length segment without any colon ":"
*/

class measure_cb
{
    std::size_t chars_ = 0;
    std::size_t segs_ = 0;
    std::size_t params_ = 0;

public:
    std::size_t
    char_count() const noexcept
    {
        return chars_;
    }

    std::size_t
    segment_count() const noexcept
    {
        return segs_;
    }

    std::size_t
    param_count() const noexcept
    {
        return params_;
    }

    void
    on_scheme(
        string_view s) noexcept
    {
        chars_ += s.size();
    }

    void
    on_userinfo(
        string_view s) noexcept
    {
        chars_ += s.size();
    }
};

class base_cb
{
public:
    void
    on_scheme(
        string_view) const noexcept
    {
    }
};

inline
string_view
make_string(
    char const* begin,
    char const* end) noexcept
{
    return { begin, static_cast<
        std::size_t>(end - begin) };
}

//----------------------------------------------------------

template<class Callback>
char const*
parse_scheme(
    char const* const begin,
    char const* const end,
    error_code& ec,
    Callback&& cb)
{
    // VFALCO is this check needed?
    if(begin == end)
    {
        ec = error::missing_scheme;
        return begin;
    }

    if(! is_alpha(*begin))
    {
        ec = error::bad_scheme_start_char;
        return begin;
    }
    auto p = begin;
    while(++p < end)
        if(! is_scheme_char(*p))
            break;
    cb.on_scheme(make_string(begin, p));
    return p;
}

template<class Callback>
void
parse_scheme(
    string_view s,
    error_code& ec,
    Callback&& cb)
{
    auto const p = parse_scheme(
        s.begin(), s.end(), ec, cb);
    if(ec)
        return;
    if(p != s.end())
        ec = error::bad_scheme_char;
}

//----------------------------------------------------------

template<class Callback>
char const*
parse_userinfo(
    char const* const begin,
    char const* const end,
    error_code& ec,
    Callback&& cb)
{
    if(begin == end)
        return begin;
    auto p = begin;
    auto const e =
        userinfo_pct_set();
    while(p < end)
    {
        if(*p == '%')
        {
            e.check_escape(p + 1, end, ec);
            if(ec)
                break;
            p += 3;
        }
        else if(e.is_special(*p))
        {
            cb.on_userinfo(
                make_string(begin, p));
            break;
        }
        ++p;
    }
    return p;
}

template<class Callback>
void
parse_userinfo(
    string_view s,
    error_code& ec,
    Callback&& cb)
{
    auto const p = parse_userinfo(
        s.begin(), s.end(), ec, cb);
    if(ec)
        return;
    if(p != s.end())
        ec = error::bad_userinfo_char;
}

//----------------------------------------------------------

unsigned short
parse_port(
    string_view s,
    error_code& ec)
{
    BOOST_ASSERT(! s.empty());
    unsigned long v = 0;
    auto const p0 = &s[0];
    auto const p1 = p0 + s.size();
    auto p = p0;
    while(p < p1)
    {
        auto const digit = static_cast<
            unsigned char>(*p++) - '0';
        if(digit > 9)
        {
            ec = error::bad_port_char;
            return {};
        }
        v = 10 * v + digit;
        if(v > 65535)
        {
            ec = error::port_overflow;
            return {};
        }
    }
    return static_cast<unsigned short>(v);
}

//----------------------------------------------------------

} // detail
} // url
} // boost

#endif
