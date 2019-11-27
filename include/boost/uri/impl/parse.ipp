//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/uri
//

#ifndef BOOST_URI_IMPL_PARSE_IPP
#define BOOST_URI_IMPL_PARSE_IPP

#include <boost/beast/uri/error.hpp>
#include <boost/beast/uri/input.hpp>
#include <stdexcept>

namespace boost {
namespace beast {
namespace uri {

namespace detail {

/*  References:

    Uniform Resource Identifier (URI): Generic Syntax
    https://tools.ietf.org/html/rfc3986

    Hypertext Transfer Protocol (HTTP/1.1): Semantics and Content
    https://tools.ietf.org/html/rfc7231

    Internationalized Resource Identifiers (IRIs)
    https://tools.ietf.org/html/rfc3987

    URL Living Standard
    https://url.spec.whatwg.org

    java.net Class URI
    https://docs.oracle.com/javase/7/docs/api/java/net/URI.html



    Generic URI

        [scheme:]scheme-specific-part[#fragment]

    Hierarchical URI

        [scheme:][//authority][path][?query][#fragment]

    bool is_hierarchical() const
    bool is_opaque() const
*/

template<class>
struct parser_impl
{
    bool
    is_alpha(char c)
    {
        unsigned constexpr a = 'a';
        return ((static_cast<unsigned>(c) | 32) - a) < 26U;
    }

    bool
    is_digit(char c)
    {
        unsigned constexpr zero = '0';
        return (static_cast<unsigned>(c) - zero) < 10;
    }

    /*
        unreserved      = ALPHA / DIGIT / "-" / "." / "_" / "~"
    */
    bool
    is_unreserved(char c)
    {
        return
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
             c == '-' || c == '.'  ||
             c == '_' || c == '~'
            ;
    } 

    /*
        sub-delims      = "!" / "$" / "&" / "'" / "(" / ")"
                              / "*" / "+" / "," / ";" / "="
    */
    bool
    is_sub_delim(char c)
    {
        return
            c == '!' || c == '$' || c == '&' || c == '\'' ||
            c == '(' || c == ')' || c == '*' || c == '+'  ||
            c == ',' || c == ';' || c == '='
            ;
    }

    int
    hex_val(char c)
    {
        if(c >= '0' && c <= '9')
            return c - '0';
        if(c >= 'A' && c <= 'F')
            return c - 'A' + 10;
        if(c >= 'a' && c <= 'f')
            return c - 'a' + 10;
        return -1;
    }

    char
    hex_digit(int v)
    {
        if(v < 10)
            return static_cast<char>(
                '0' + v);
        return static_cast<char>(
            'A' + v - 10);
    }

    char
    to_lower(char c)
    {
        if((static_cast<unsigned>(c) - 65U) < 26)
            return c + ('a' - 'A');
        return c;
    }

    //--------------------------------------------------------------------------

    /*
        pct-encoded     = "%" HEXDIG HEXDIG

        Caller is responsible for consuming the '%'
    */
    void
    parse_pct_encoded(
        char& c,
        input& in,
        error_code& ec)
    {
        if(in.remain() < 2)
        {
            // short input
            ec = error::syntax;
            return;
        }
        auto d1 = hex_val(in[0]);
        if(d1 == -1)
        {
            // invalid hex digit
            ec = error::syntax;
            return;
        }
        auto d2 = hex_val(in[1]);
        if(d2 == -1)
        {
            // invalid hex digit
            ec = uri::error::invalid;
            return;
        }
        c = static_cast<char>(
            16 * d1 + d2);
        in += 2;
    }

    // append a percent encoded char to the
    // current string. the hex digits are normalized
    void
    append_pct_encoded(buffer& out, char c)
    {
        auto it = out.prepare(3);
        *it++ = '%';
        *it++ = hex_digit(
            static_cast<unsigned char>(c) / 16);
        *it++ = hex_digit(
            static_cast<unsigned char>(c) % 16);
    }

    //--------------------------------------------------------------------------

    /*
        pchar           = unreserved / pct-encoded / sub-delims / ":" / "@"
    */
    void
    parse_pchar(
        buffer& out,
        input& in,
        error_code& ec)
    {
        /*
            unreserved      = ALPHA / DIGIT / "-" / "." / "_" / "~"

            sub-delims      = "!" / "$" / "&" / "'" / "(" / ")"
                                  / "*" / "+" / "," / ";" / "="
        */
        auto const is_pchar =
            [&](char c)
            {
                return
                    is_alpha(c) ||
                    is_digit(c) ||
                     c == '-' || c == '.'  || c == '_' || c == '~'  ||
                     c == '!' || c == '$'  || c == '&' || c == '\'' ||
                     c == '(' || c == ')'  || c == '*' || c == '+'  ||
                     c == ',' || c == ';'  || c == '=' ||
                     c == ':' || c == '@'
                    ;
            };

        if(is_pchar(*in))
        {
            out.append(*in++);
            return;
        }

        if(*in != '%')
        {
            ec = error::mismatch;
            return;
        }
        in++;
        char c;
        parse_pct_encoded(c, in, ec);
        if(ec)
            return;
        if(is_unreserved(c))
        {
            out.append(c);
            return;
        }

        /*  VFALCO Small problem here
            https://tools.ietf.org/html/rfc3986#section-2.2
            URI producing applications should percent-encode data octets that
            correspond to characters in the reserved set unless these characters
            are specifically allowed by the URI scheme to represent data in that
            component.

            How do we know if the URI scheme considers `c` a delimiter?
            Maybe we need to use CRTP or something as a customization point.
        */
        append_pct_encoded(out, c);
    }

    /*
        qchar           = unreserved / pct-encoded / sub-delims / ":" / "@" / '/' / "?"
    */
    void
    parse_qchar(
        buffer& out,
        input& in,
        error_code& ec)
    {
        /*
            unreserved      = ALPHA / DIGIT / "-" / "." / "_" / "~"

            sub-delims      = "!" / "$" / "&" / "'" / "(" / ")"
                                  / "*" / "+" / "," / ";" / "="
        */
        auto const is_qchar =
            [&](char c)
            {
                return
                    is_alpha(c) ||
                    is_digit(c) ||
                     c == '-' || c == '.'  || c == '_' || c == '~'  ||
                     c == '!' || c == '$'  || c == '&' || c == '\'' ||
                     c == '(' || c == ')'  || c == '*' || c == '+'  ||
                     c == ',' || c == ';'  || c == '=' ||
                     c == ':' || c == '@'  || c == '/' || c == '?'
                    ;
            };

        if(is_qchar(*in))
        {
            out.append(*in++);
            return;
        }

        if(*in != '%')
        {
            ec = error::mismatch;
            return;
        }
        in++;
        char c;
        parse_pct_encoded(c, in, ec);
        if(ec)
            return;
        if(is_unreserved(c))
        {
            out.append(c);
            return;
        }

        /*  VFALCO Small problem here
            https://tools.ietf.org/html/rfc3986#section-2.2
            URI producing applications should percent-encode data octets that
            correspond to characters in the reserved set unless these characters
            are specifically allowed by the URI scheme to represent data in that
            component.

            How do we know if the URI scheme considers `c` a delimiter?
            Maybe we need to use CRTP or something as a customization point.
        */
        append_pct_encoded(out, c);
    }

    //--------------------------------------------------------------------------

    /*
        segment         = *pchar
    */
    void
    parse_segment(
        buffer& out,
        input& in,
        error_code& ec)
    {
        for(;;)
        {
            if(in.empty())
                break;
            parse_pchar(out, in, ec);
            if(ec)
            {
                if(ec != error::mismatch)
                    return;
                ec.clear();
                break;
            }
        }
    }

    //--------------------------------------------------------------------------

    /*
        https://tools.ietf.org/html/rfc3986#section-3.2
        The authority component is preceded by a double slash ("//") and is
        terminated by the next slash ("/"), question mark ("?"), or number
        sign ("#") character, or by the end of the URI.
    */
    void
    parse_authority(
        buffer& out,
        input& in,
        error_code& ec)
    {
        boost::ignore_unused(ec);
        auto start = out.end();
        while(! in.empty())
        {
            if( *in == '/' ||
                *in == '?' ||
                *in == '#')
                break;
            out.append(*in++);
        }
        out.parts().authority = out.piece_from(start);
    }

    /*
        path-abempty    = *( "/" segment )
        segment         = *pchar

    */
    void
    parse_path_abempty(
        buffer& out,
        input& in,
        error_code& ec)
    {
        if(in.empty())
        {
            auto start = out.end();
            // normalize: path is not empty
            out.append('/');
            out.parts().path = out.piece_from(start);
            return;
        }
        auto start = out.end();
        while(! in.empty() && *in == '/')
        {
            in++;
            out.append('/');
            parse_segment(out, in, ec);
            if(ec)
                return;
        }
        out.parts().path = out.piece_from(start);
    }

    //--------------------------------------------------------------------------

    /*
        scheme      = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) ":"
    */
    void
    parse_scheme(
        buffer& out,
        input& in,
        error_code& ec)
    {
        if(in.empty())
        {
            // bad scheme
            ec = error::syntax;
            return;
        }
        if(! is_alpha(*in))
        {
            // bad scheme
            ec = error::syntax;
            return;
        }

        auto start = out.end();
        for(;;)
        {
            out.append(to_lower(*in++));
            if(in.empty())
            {
                // bad scheme
                ec = error::syntax;
                return;
            }
            if( ! is_alpha(*in) &&
                ! is_digit(*in) &&
                *in != '+' &&
                *in != '-' &&
                *in != '.')
                break;
        }

        out.parts().scheme = out.piece_from(start);
        out.parts().scheme_value = string_to_scheme(out.scheme());
    }

    /*
        hier-part   = "//" authority path-abempty
                    / path-absolute
                    / path-rootless
                    / path-empty
    */
    void
    parse_hier_part(
        buffer& out,
        input& in,
        error_code& ec)
    {
        if( in.remain() >= 2 &&
            in[0] == '/' &&
            in[1] == '/')
        {
            in += 2;
            auto s = out.prepare(2);
            s[0] = '/';
            s[1] = '/';
            parse_authority(out, in, ec);
            if(ec)
                return;
            parse_path_abempty(out, in, ec);
            if(ec)
                return;
        }

    }

    /*
        query       = *qchar

        qchar       = pchar / "/" / "?"
    */
    void
    parse_query(
        buffer& out,
        input& in,
        error_code& ec)
    {
        auto start = out.end();
        for(;;)
        {
            if(in.empty())
                break;
            parse_qchar(out, in, ec);
            if(ec)
            {
                if(ec != error::mismatch)
                    return;
                ec.clear();
                break;
            }
        }
        if(out.end() != start)
            out.parts().query = out.piece_from(start);
    }

    //--------------------------------------------------------------------------

    /*
        absolute-URI    = scheme ":" hier-part [ "?" query ]

        https://tools.ietf.org/html/rfc3986#section-4.3

        To allow for transition to absoluteURIs in all requests in future
        versions of HTTP, all HTTP/1.1 servers MUST accept the absoluteURI
        form in requests, even though HTTP/1.1 clients will only generate
        them in requests to proxies.
    */
    void
    parse_absolute_form(
        buffer& out,
        input& in,
        error_code& ec)
    {
        // scheme
        parse_scheme(out, in, ec);
        if(ec)
            return;

        // ":"
        if(in.empty())
        {
            ec = error::syntax;
            return;
        }
        if(*in++ != ':')
        {
            ec = error::syntax;
            return;
        }
        out.append(':');

        // hier-part
        parse_hier_part(out, in, ec);
        if(ec)
            return;

        // "?"
        if(in.empty())
            return;
        if(*in != '?')
        {
            ec = error::syntax;
            return;
        }
        in++;
        out.append('?');

        // query
        parse_query(out, in, ec);
        if(ec)
            return;

        if(! in.empty())
        {
            // spurious characters
            ec = error::syntax;
            return;
        }
    }
};

using parser = parser_impl<void>;

} // detail

//------------------------------------------------------------------------------

/*
    absolute-URI    = scheme ":" hier-part [ "?" query ]

    https://tools.ietf.org/html/rfc3986#section-4.3

    To allow for transition to absoluteURIs in all requests in future
    versions of HTTP, all HTTP/1.1 servers MUST accept the absoluteURI
    form in requests, even though HTTP/1.1 clients will only generate
    them in requests to proxies.
*/
inline
void
parse_absolute_form(
    buffer& out,
    string_view s,
    error_code& ec)
{
    input in{s};
    out.clear();
    ec.assign(0, ec.category());
    try
    {
        detail::parser p;
        p.parse_absolute_form(out, in, ec);
    }
    catch(std::length_error const&)
    {
        //ec = error::overflow
    }
}

} // uri
} // beast
} // boost

#endif
