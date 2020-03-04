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
#include <boost/url/detail/parts.hpp>
#include <boost/url/error.hpp>

namespace boost {
namespace urls {
namespace detail {

// https://tools.ietf.org/html/rfc3986#section-3.2

//----------------------------------------------------------
/*
    Guidelines for parsing functions

    * On a parsing error, input pointer is preserved

*/
struct parser
{
    char const* const begin_;
    char const* const end_;
    char const* p_;

    explicit
    parser(
        string_view s) noexcept
        : begin_(s.data())
        , end_(begin_ + s.size())
        , p_(begin_)
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

    string_view
    make_string(
        char const* begin,
        char const* end) noexcept
    {
        return { begin, static_cast<
            std::size_t>(end - begin) };
    }

    bool
    starts_with(
        string_view s,
        string_view prefix) noexcept
    {
        return s.substr(
            0, prefix.size()) == prefix;
    }

    void
    mark(parts& pt, int id) noexcept
    {
        if(id == id_scheme)
        {
            pt.resize(
                id_scheme,
                p_ - begin_);
            return;
        }
        pt.resize(id,
            p_ - begin_ -
            pt.offset[id]);
    }

    //------------------------------------------------------

    // scheme
    bool
    match_scheme() noexcept
    {
        if(p_ == end_)
            return false;
        if(! is_alpha(*p_))
            return false;
        while(++p_ < end_)
            if(! is_scheme_char(*p_))
                break;
        return true;
    }

    //------------------------------------------------------

    // hier-part
    void
    parse_hier_part(
        parts& pt,
        error_code& ec) noexcept
    {
        // authority
        if(match_literal("//"))
        {
            parse_authority(pt, ec);
            if(ec)
                return;
            // path-abempty
            parse_path_abempty(pt, ec);
            if(ec)
                return;
            return;
        }

        // path-absolute
        parse_path_absolute(pt, ec);
        if(ec != error::no_match)
            return;
        ec = {};

        // path-rootless
        parse_path_rootless(pt, ec);
        if(ec != error::no_match)
            return;
        ec = {};

        // path-empty
    }

    // relative-part
    void
    parse_relative_part(
        parts& pt,
        error_code& ec) noexcept
    {
        // authority
        if(match_literal("//"))
        {
            parse_authority(pt, ec);
            if(ec)
                return;
            // path-abempty
            parse_path_abempty(pt, ec);
            if(ec)
                return;
            return;
        }

        // path-absolute
        parse_path_absolute(pt, ec);
        if(ec != error::no_match)
            return;
        ec = {};

        // path-noscheme
        parse_path_noscheme(pt, ec);
        if(ec != error::no_match)
            return;
        ec = {};

        // path-empty
    }

    //------------------------------------------------------

    void
    parse_authority(
        parts& pt,
        error_code& ec) noexcept
    {
        auto const p0 = p_;
        parse_userinfo(pt, ec);
        if(ec == error::no_match)
        {
            // re-parse as host
            ec = {};
        }
        else if(ec)
        {
            return;
        }
        else if(match_literal("@"))
        {
            // add '@'
            mark(pt, id_password);
        }
        else
        {
            // requires backtracking
            p_ = p0;
            mark(pt, id_user);
            mark(pt, id_password);
        }
        parse_host(pt, ec);
    }

    void
    parse_userinfo(
        parts& pt,
        error_code& ec) noexcept
    {
        auto const p0 = p_;
        auto e = userinfo_nc_pct_set();
        p_ = e.parse(p0, end_, ec);
        if(ec)
            return;
        mark(pt, id_user);
        auto const p1 = p_;
        e = userinfo_pct_set();
        p_ = e.parse(p1, end_, ec);
        if(ec)
            return;
        mark(pt, id_password);
    }

    void
    parse_host(
        parts& pt,
        error_code& ec) noexcept
    {
        parse_hostname(pt, ec);
        if(ec)
            return;

        // port
        if( p_ >= end_ ||
            *p_ != ':')
            return;
        ++p_;
        match_port();
        mark(pt, id_port);
    }

    void
    parse_hostname(
        parts& pt,
        error_code& ec) noexcept
    {
        parse_ip_literal(pt, ec);
        if(ec)
        {
            if(ec != error::no_match)
                return;
            ec = {};
            parse_ipv4_or_domain(pt, ec);
            if(ec)
                return;
        }
        mark(pt, id_host);
    }

    void
    parse_ip_literal(
        parts& pt,
        error_code& ec)
    {
        if(p_ == end_)
        {
            ec = error::no_match;
            return;
        }
        if(*p_ != '[')
        {
            ec = error::no_match;
            return;
        }
        ++p_;
        parse_ip_v6(ec);
        if(ec)
            return;
        if(p_ == end_)
        {
            ec = error::syntax;
            return;
        }
        if(*p_ != ']')
        {
            ec = error::syntax;
            return;
        }
        ++p_;
        pt.host = host_type::ipv6;
    }

    void
    parse_ipv4_or_domain(
        parts& pt,
        error_code& ec)
    {
        auto const p0 = p_;
        if(match_ip_v4())
        {
            pt.host = host_type::ipv4;
            auto const p1 = p_;
            auto const e =
                reg_name_pct_set();
            p_ = e.parse(p_, end_, ec);
            if(ec)
                return;
            if(p_ != p1)
                pt.host = host_type::name;
            return;
        }
        auto const e =
            reg_name_pct_set();
        p_ = e.parse(p_, end_, ec);
        if(ec)
            return;
        if(p_ != p0)
            pt.host = host_type::name;
    }

    short
    match_octet()
    {
        short v = 0;
        if(p_ >= end_)
            return -1;
        auto digit = static_cast<
            unsigned char>(*p_ - '0');
        if(digit >= 10)
            return -1;
        ++p_;
        if(digit == 0)
        {
            // No more digits
            // allowed after '0'
            return 0;
        }
        v = digit;
        while(p_ < end_)
        {
            digit = static_cast<
                unsigned char>(*p_ - '0');
            if(digit >= 10)
                break;
            v = 10 * v + digit;
            ++p_;
            if(v > 255)
                return -1;
        }
        return v;
    }

    bool
    match_ip_v4()
    {
        auto a = match_octet();
        if( a == -1 ||
            p_ >= end_ ||
            *p_ != '.')
            return false;
        ++p_;
        auto b = match_octet();
        if( b == -1 ||
            p_ >= end_ ||
            *p_ != '.')
            return false;
        ++p_;
        auto c = match_octet();
        if( c == -1 ||
            p_ >= end_ ||
            *p_ != '.')
            return false;
        ++p_;
        auto d = match_octet();
        if(d == -1)
            return false;
        return true;
    }

    long
    parse_word(
        error_code& ec) noexcept
    {
        BOOST_ASSERT(p_ != end_);
        auto digit =
            hex_digit(*p_);
        if(digit == -1)
        {
            ec = error::syntax;
            return -1;
        }
        long v = digit;
        ++p_;

        if(p_ == end_)
            return v;
        digit =
            hex_digit(*p_);
        if(digit == -1)
            return v;
        v = 16 * v + digit;
        ++p_;

        if(p_ == end_)
            return v;
        digit =
            hex_digit(*p_);
        if(digit == -1)
            return v;
        v = 16 * v + digit;
        ++p_;

        if(p_ == end_)
            return v;
        digit =
            hex_digit(*p_);
        if(digit == -1)
            return v;
        v = 16 * v + digit;
        ++p_;

        return v;
    }

    // return `true` if the hex
    // word could be 0..255 if
    // interpreted as decimal
    bool
    maybe_octet(long word)
    {
        if(word > 0x255)
            return false;
        if(((word >>  4) & 0xf) > 9)
            return false;
        if((word & 0xf) > 9)
            return false;
        return true;
    }

    void
    parse_ip_v6(
        error_code& ec) noexcept
    {
        if(p_ == end_)
        {
            ec = error::syntax;
            return;
        }
        int n = 8; // words needed
        bool b = false; // got '::'
        bool c = false; // need colon
        long word = 0;
        for(;;)
        {
            if(p_ == end_)
            {
                if(b)
                    break;
                // not enough words
                BOOST_ASSERT(n > 0);
                ec = error::syntax;
                return;
            }
            if(*p_ == ':')
            {
                ++p_;
                if(p_ == end_)
                {
                    ec = error::syntax;
                    return;
                }
                if(*p_ == ':')
                {
                    if(b)
                    {
                        // two '::'
                        ec = error::syntax;
                        return;
                    }
                    ++p_;
                    --n;
                    if(n == 0)
                        break;
                    b = true;
                    c = false;
                }
                else if(c)
                {
                    word = parse_word(ec);
                    if(ec)
                        return;
                    --n;
                    if(n == 0)
                        break;
                }
                else
                {
                    // can't start with ':'
                    ec = error::syntax;
                    return;
                }
            }
            else if(*p_ == '.')
            {
                if(! b && n > 1)
                {
                    // not enough words
                    ec = error::syntax;
                    return;
                }
                if(! maybe_octet(word))
                {
                    // invalid octet
                    ec = error::syntax;
                    return;
                }
                ++p_;
                auto m = match_octet();
                if( m == -1)
                {
                    ec = error::syntax;
                    return;
                }
                if(p_ >= end_)
                {
                    ec = error::syntax;
                    return;
                }
                if(*p_ != '.')
                {
                    ec = error::syntax;
                    return;
                }
                ++p_;
                m = match_octet();
                if( m == -1)
                {
                    ec = error::syntax;
                    return;
                }
                if(p_ >= end_)
                {
                    ec = error::syntax;
                    return;
                }
                if(*p_ != '.')
                {
                    ec = error::syntax;
                    return;
                }
                ++p_;
                m = match_octet();
                if(m == -1)
                {
                    ec = error::syntax;
                    return;
                }
                return;
            }
            else if(b && hex_digit(*p_) == -1)
            {
                // end in "::"
                break;
            }
            else if(! c)
            {
                word = parse_word(ec);
                if(ec)
                    return;
                --n;
                if(n == 0)
                    break;
                c = true;
            }
            else
            {
                // ':' divides words
                ec = error::syntax;
                return;
            }
        }
    }

    // port = *DIGIT
    void
    match_port() noexcept
    {
        while(p_ < end_)
        {
            auto const digit = static_cast<
                unsigned char>(*p_ - '0');
            if(digit > 9)
                break;
            ++p_;
        }
    }

    //------------------------------------------------------

    // path-abempty
    void
    parse_path_abempty(
        parts& pt,
        error_code& ec) noexcept
    {
        // *( "/" segment )
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
            ++pt.nseg;
        }
        mark(pt, id_path);
    }

    // path-absolute
    void
    parse_path_absolute(
        parts& pt,
        error_code& ec) noexcept
    {
        // "/" [ segment-nz *( "/" segment ) ]
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
        if(p_ != end_)
        {
            if(*p_ == '/')
            {
                // begins with "/" but not "//"
                ec = error::invalid;
                return;
            }
            auto const e =
                pchar_pct_set();
            // segment-nz
            p_ = e.parse(
                p_ + 1, end_, ec);
            if(ec)
                return;
            ++pt.nseg;
            while(p_ < end_)
            {
                if(*p_ != '/')
                    break;
                p_ = e.parse(
                    p_ + 1, end_, ec);
                if(ec)
                    return;
                ++pt.nseg;
            }
        }
        mark(pt, id_path);
    }

    // path-noscheme
    void
    parse_path_noscheme(
        parts& pt,
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
        ++pt.nseg;
        e = pchar_pct_set();
        while(p_ < end_)
        {
            if(*p_ != '/')
                break;
            p_ = e.parse(
                p_ + 1, end_, ec);
            if(ec)
                return;
            ++pt.nseg;
        }
        mark(pt, id_path);
    }

    // path-rootless
    void
    parse_path_rootless(
        parts& pt,
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
        ++pt.nseg;
        while(p_ < end_)
        {
            if(*p_ != '/')
                break;
            p_ = e.parse(
                p_ + 1, end_, ec);
            if(ec)
                return;
            ++pt.nseg;
        }
        mark(pt, id_path);
    }

    //------------------------------------------------------
    //
    // query
    //
    //------------------------------------------------------

    void
    parse_query(
        parts& pt,
        error_code& ec) noexcept
    {
        if(p_ == end_)
            return;
        if(*p_ != '?')
            return;
        auto const ek =
            qkey_pct_set();
        auto const ev =
            qval_pct_set();
        std::size_t n = 0;
        for(;;)
        {
            ++n;
            p_ = ek.parse(
                p_, end_, ec);
            if(ec)
                return;
            if(p_ >= end_)
                break;
            if(*p_ != '=')
                break;
            ++p_;
            if(p_ >= end_)
                break;
            p_ = ev.parse(
                p_, end_, ec);
            if(ec)
                return;
            if(p_ >= end_)
                break;
            if(*p_ != '&')
                break;
            ++p_;
        }
        mark(pt, id_query);
        pt.nparam = n;
    }

    //------------------------------------------------------
    //
    // fragment
    //
    //------------------------------------------------------

    void
    parse_fragment(
        parts& pt,
        error_code& ec) noexcept
    {
        if(p_ == end_)
            return;
        if(*p_ != '#')
            return;
        ++p_;
        auto const e =
            frag_pct_set();
        p_ = e.parse(
            p_, end_, ec);
        if(ec)
            return;
        mark(pt, id_frag);
    }

    //------------------------------------------------------

    bool
    match_literal(
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

// URL (a.k.a. URI-reference)
inline
void
parse_url(
    parts& pt,
    string_view s,
    error_code& ec) noexcept
{
    parser pr(s);
    if( pr.match_scheme() &&
        pr.match_literal(":"))
    {
        // absolute-URI
        pr.mark(pt, id_scheme);
        pr.parse_hier_part(pt, ec);
    }
    else
    {
        // rewind
        ec = {};
        pr.p_ = pr.begin_;

        // relative-ref
        pr.parse_relative_part(pt, ec);
        if(ec)
            return;
    }

    pr.parse_query(pt, ec);
    if(ec)
        return;
  
    pr.parse_fragment(pt, ec);
    if(ec)
        return;

    if(! pr.done())
        ec = error::syntax;
}

// origin
// VFALCO This should throw not return error code
inline
void
parse_origin(
    parts& pt,
    string_view s,
    error_code& ec)
{
    parser pr(s);
    
    // scheme ":" [ "//" authority ]
    if(! pr.match_scheme())
    {
        ec = error::syntax;
        return;
    }
    if(! pr.match_literal(":"))
    {
        ec = error::syntax;
        return;
    }
    pr.mark(pt, id_scheme);
    pr.parse_hier_part(pt, ec);

    // authority
    if(! pr.match_literal("//"))
        return;
    pr.parse_authority(pt, ec);
    if(ec)
        return;
    pt.resize(id_user,
        pt.length(id_user) + 1);

    if(! pr.done())
        ec = error::syntax;
}

inline
void
parse_scheme(
    parts& pt,
    string_view s)
{
    parser pr(s);
    if(! pr.match_scheme())
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();
    pr.mark(pt, id_scheme);
}

inline
void
parse_authority(
    parts& pt,
    string_view s)
{
    parser pr(s);
    error_code ec;
    pr.parse_authority(pt, ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();
}

inline
void
parse_userinfo(
    parts& pt,
    string_view s)
{
    parser pr(s);
    error_code ec;
    pr.parse_userinfo(pt, ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();
}

inline
void
parse_hostname(
    parts& pt,
    string_view s)
{
    parser pr(s);
    error_code ec;
    pr.parse_hostname(pt, ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();
}

inline
void
parse_plain_hostname(
    parts& pt,
    string_view s)
{
    parser pr(s);
    error_code ec;
    pr.parse_ip_literal(pt, ec);
    if(ec)
    {
        if(ec != error::no_match)
        {
            pt.host = host_type::name;
            return;
        }
        ec = {};
        if(pr.match_ip_v4())
        {
            if(pr.done())
            {
                pt.host = host_type::ipv4;
                return;
            }
            pt.host = host_type::name;
            return;
        }
    }
    else if(pr.done())
    {
        return;
    }
    pt.host = host_type::name;
}

inline
void
match_port(string_view s)
{
    parser pr(s);
    error_code ec;
    pr.match_port();
    if(! pr.done())
        invalid_part::raise();
}

inline
void
match_path_abempty(string_view s)
{
    parts pt;
    parser pr(s);
    error_code ec;
    pr.parse_path_abempty(pt, ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();
}

inline
void
match_path_absolute(string_view s)
{
    parts pt;
    parser pr(s);
    error_code ec;
    pr.parse_path_absolute(pt, ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();
}

inline
void
match_path_noscheme(string_view s)
{
    parts pt;
    parser pr(s);
    error_code ec;
    pr.parse_path_noscheme(pt, ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();
}

inline
void
match_path_rootless(string_view s)
{
    parts pt;
    parser pr(s);
    error_code ec;
    pr.parse_path_rootless(pt, ec);
    if(ec)
        invalid_part::raise();
    if(! pr.done())
        invalid_part::raise();
}

} // detail
} // urls
} // boost

#endif
