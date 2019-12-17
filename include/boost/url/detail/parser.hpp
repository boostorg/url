//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_PARSER_HPP
#define BOOST_URL_DETAIL_PARSER_HPP

#include <boost/url/detail/char_type.hpp>
#include <boost/url/error.hpp>

namespace boost {
namespace url {
namespace detail {

/*  https://tools.ietf.org/html/rfc3986#section-3.2

    URI-reference = URI / relative-ref
    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]

    hier-part     = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty

    relative-part = "//" authority path-abempty
                  / path-absolute
                  / path-noscheme
                  / path-empty
 
    ; If an authority component is present, then the path
    ; component must either be empty or begin with a slash (/).
    ;
    ; If an authority component is absent, then the path cannot
    ; begin with an empty segment, that is with two slashes (//),
    ; as the following characters would be interpreted as an
    ; authority component.

    path-abempty  = *( "/" segment )
                  ; begins with "/" or is empty

    path-absolute = "/" [ segment-nz *( "/" segment ) ]
                  ; begins with "/" but not "//"

    path-noscheme = segment-nz-nc *( "/" segment )
                  ; begins with a non-colon segment

    path-rootless = segment-nz *( "/" segment )
                  ; begins with a segment

    path-empty    = 0<pchar>
    
    segment       = *pchar
    segment-nz    = 1*pchar
    segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
                    ; non-zero-length segment without any colon ":"

    authority     = [ userinfo "@" ] host [ ":" port ]
    userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    host          = IP-literal / IPv4address / reg-name
    port          = *DIGIT
*/

inline
string_view
make_string(
    char const* begin,
    char const* end) noexcept
{
    return { begin, static_cast<
        std::size_t>(end - begin) };
}

inline
bool
starts_with(
    string_view s,
    string_view prefix) noexcept
{
    return s.substr(
        0, prefix.size()) == prefix;
}

//----------------------------------------------------------

struct parser
{
    using size_type = uint32_t;

    char const* const begin_;
    char const* const end_;
    char const* p_;

    std::size_t nscheme = 0;
    std::size_t nuser = 0;
    std::size_t npass = 0;
    std::size_t nhost = 0;
    std::size_t nport = 0;
    std::size_t npath = 0;
    std::size_t npath_seg = 0;
    std::size_t nquery = 0;
    std::size_t nquery_kvp = 1;
    std::size_t nfrag = 0;
    optional<unsigned short> port;

    explicit
    parser(
        string_view s) noexcept
        : parser(s.begin(), s.end())
    {
    }

    parser(
        char const* begin,
        char const* end) noexcept
        : begin_(begin)
        , end_(end)
        , p_(begin_)
    {
    }

    bool
    done() const noexcept
    {
        return p_ == end_;
    }

    //------------------------------------------------------

    // URI-reference
    void
    parse_uri_reference(
        error_code& ec) noexcept
    {
        auto p0 = p_;
        parse_scheme(ec);
        if(! ec && maybe_literal(":"))
        {
            // URI
            ++nscheme;
            parse_hier_part(ec);
        }
        else
        {
            // rewind
            ec = {};
            p_ = p0;
            nscheme = 0;

            // relative-ref
            parse_relative_part(ec);
            if(ec)
                return;
        }
        parse_query_and_fragment(ec);
    }

    // origin
    void
    parse_origin(
        error_code& ec) noexcept
    {
        // scheme ":" [ "//" authority ]
        parse_scheme(ec);
        if(ec)
            return;
        if(! maybe_literal(":"))
            return;
        ++nscheme;
        parse_hier_part(ec);

        // authority
        if(! maybe_literal("//"))
            return;
        parse_authority(ec);
        if(ec)
            return;
        nuser += 2;
    }

    //------------------------------------------------------

    // scheme
    void
    parse_scheme(
        error_code& ec) noexcept
    {
        if(p_ == end_)
        {
            ec = error::no_match;
            return;
        }
        if(! is_alpha(*p_))
        {
            ec = error::no_match;
            return;
        }
        auto const p0 = p_;
        while(++p_ < end_)
            if(! is_scheme_char(*p_))
                break;
        nscheme = p_ - p0;
    }

    void
    parse_scheme()
    {
        error_code ec;
        parse_scheme(ec);
        if(ec)
            invalid_part::raise();
        if(! done())
            invalid_part::raise();
    }

    //------------------------------------------------------

    // hier-part
    void
    parse_hier_part(
        error_code& ec) noexcept
    {
        // authority
        if(maybe_literal("//"))
        {
            parse_authority(ec);
            if(ec)
                return;
            nuser += 2;
            // path-abempty
            parse_path_abempty(ec);
            if(ec)
                return;
            return;
        }

        // path-absolute
        parse_path_absolute(ec);
        if(ec != error::no_match)
            return;
        ec = {};

        // path-rootless
        parse_path_rootless(ec);
        if(ec != error::no_match)
            return;
        ec = {};

        // path-empty
    }

    // relative-part
    void
    parse_relative_part(
        error_code& ec) noexcept
    {
        // authority
        if(maybe_literal("//"))
        {
            parse_authority(ec);
            if(ec)
                return;
            nuser += 2;
            // path-abempty
            parse_path_abempty(ec);
            if(ec)
                return;
            return;
        }

        // path-absolute
        parse_path_absolute(ec);
        if(ec != error::no_match)
            return;
        ec = {};

        // path-noscheme
        parse_path_noscheme(ec);
        if(ec != error::no_match)
            return;
        ec = {};

        // path-empty
    }

    //------------------------------------------------------

    void
    parse_authority(
        error_code& ec) noexcept
    {
        parse_userinfo(ec);
        if(ec == error::no_match)
        {
            // re-parse as host
            ec = {};
        }
        else if(ec)
        {
            return;
        }
        parse_host(ec);
    }

    void
    parse_authority()
    {
        error_code ec;
        parse_authority(ec);
        if(ec)
            invalid_part::raise();
        if(! done())
            invalid_part::raise();
    }

    void
    parse_userinfo(
        error_code& ec) noexcept
    {
        auto const p0 = p_;
        auto e = userinfo_nc_pct_set();
        p_ = e.parse(p0, end_, ec);
        if(ec)
            return;
        nuser = p_ - p0;
        auto const p1 = p_;
        e = userinfo_pct_set();
        p_ = e.parse(p1, end_, ec);
        if(ec)
            return;
        if(! maybe_literal("@"))
        {
            // requires backtracking
            p_ = p0;
            nuser = 0;
            ec = error::no_match;
            return;
        }
        npass += p_ - p1;
    }

    void
    parse_host(
        error_code& ec) noexcept
    {
        auto p0 = p_;
        auto e =
            reg_name_pct_set();
        p_ = e.parse(p0, end_, ec);
        if(ec)
            return;
        nhost = p_ - p0;
        if( p_ >= end_ ||
            *p_ != ':')
            return;
        ++p_;
        parse_port(ec);
        if(ec)
            return;
        ++nport;
    }

    // port = *DIGIT
    void
    parse_port(
        error_code& ec) noexcept
    {
        auto const p0 = p_;
        unsigned long v = 0;
        while(p_ < end_)
        {
            auto const digit = static_cast<
                unsigned char>(*p_ - '0');
            if(digit > 9)
                break;
            v = 10 * v + digit;
            if(v > 65535)
            {
                ec = error::port_overflow;
                return;
            }
            ++p_;
        }
        if(p_ == p0)
            port = {};
        else
            port = static_cast<
                unsigned short>(v);
        nport = p_ - p0;
    }

    //------------------------------------------------------

    // path-abempty
    void
    parse_path_abempty(
        error_code& ec) noexcept
    {
        auto const p0 = p_;
        auto const e =
            pchar_pct_set();
        while(p_ < end_)
        {
            if(*p_ != '/')
                break;
            p_ = e.parse(
                p_ + 1, end_, ec);
            if(ec)
                return;
            ++npath_seg;
        }
        npath = p_ - p0;
    }

    // path-absolute
    void
    parse_path_absolute(
        error_code& ec) noexcept
    {
        if(p_ == end_)
        {
            ec = error::no_match;
            return;
        }
        if(*p_ != '/')
        {
            ec = error::no_match;
            return;
        }
        ++p_;
        if(p_ == end_)
        {
            // first segment can't be empty
            ec = error::invalid;
            return;
        }
        if(*p_ == '/')
        {
            // begins with "/" but not "//"
            ec = error::invalid;
            return;
        }
        npath++; // leading '/'
        parse_path_rootless(ec);
    }

    // path-rootless
    void
    parse_path_rootless(
        error_code& ec) noexcept
    {
        auto const p0 = p_;
        auto const e =
            pchar_pct_set();
        p_ = e.parse(p0, end_, ec);
        if(ec)
            return;
        if(p_ == p0)
        {
            // first segment can't be empty
            ec = error::no_match;
            return;
        }
        ++npath_seg;
        while(p_ < end_)
        {
            if(*p_ != '/')
                break;
            p_ = e.parse(
                p_ + 1, end_, ec);
            if(ec)
                return;
            ++npath_seg;
        }
        npath += p_ - p0;
    }

    // path-noscheme
    void
    parse_path_noscheme(
        error_code& ec) noexcept
    {
        auto const p0 = p_;
        auto e =
            pchar_nc_pct_set();
        p_ = e.parse(p0, end_, ec);
        if(ec)
            return;
        if(p_ == p0)
        {
            // first segment can't be empty
            ec = error::no_match;
            return;
        }
        ++npath_seg;
        e = pchar_pct_set();
        while(p_ < end_)
        {
            if(*p_ != '/')
                break;
            p_ = e.parse(
                p_ + 1, end_, ec);
            if(ec)
                return;
            ++npath_seg;
        }
        npath += p_ - p0;
    }

    void
    write_path_offsets(
        size_type* base) noexcept
    {
        auto const e = pchar_pct_set();
        auto p = begin_ +
            nscheme +
            nuser +
            npass +
            nhost +
            nport;
        auto const end = p + npath;
        *base-- = static_cast<
            size_type>(p - begin_);
        while(p < end)
        {
            if(*p != '/')
                break;
            error_code ec;
            p = e.parse(
                p + 1, end, ec);
            BOOST_ASSERT(! ec);
            *base-- = static_cast<
                size_type>(p - begin_);
        }
    }

    //------------------------------------------------------

    void
    parse_query_and_fragment(
        error_code& ec) noexcept
    {
        if(p_ == end_)
            return;

        // query
        if(*p_ == '?')
        {
            auto const p0 = p_;
            ++p_;
            auto const e =
                query_pct_set();
            p_ = e.parse(p_, end_, ec);
            if(ec)
                return;
            nquery = p_ - p0;
            if(p_ == end_)
                return;
        }

        // fragment
        if(*p_ == '#')
        {
            auto const p0 = p_;
            ++p_;
            auto const e =
                frag_pct_set();
            p_ = e.parse(p_, end_, ec);
            if(ec)
                return;
            nfrag = p_ - p0;
            if(p_ == end_)
                return;
        }

        ec = error::invalid;
    }

    //------------------------------------------------------

    bool
    maybe_literal(
        string_view s) noexcept
    {
        if(s.size() > static_cast<
            std::uintptr_t>(end_ - p_))
            return false;
        if(! starts_with(
            make_string(p_, end_), s))
            return false;
        p_ += s.size();
        return true;
    }
};

} // detail
} // url
} // boost

#endif
