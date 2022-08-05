//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_URL_VIEW_BASE_HPP
#define BOOST_URL_URL_VIEW_BASE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/authority_view.hpp>
#include <boost/url/host_type.hpp>
#include <boost/url/ipv4_address.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/params_view.hpp>
#include <boost/url/params_encoded_view.hpp>
#include <boost/url/pct_encoding.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/segments_encoded_view.hpp>
#include <boost/url/segments_view.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/detail/url_impl.hpp>
#include <boost/assert.hpp>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <utility>

namespace boost {
namespace urls {

class BOOST_SYMBOL_VISIBLE
    url_view_base
    : protected detail::parts_base
{
    detail::url_impl u_;

    friend class url;
    friend class url_base;
    friend class url_view;
    friend class static_url_base;
    friend class params;
    friend class params_view;
    friend class params_encoded;
    friend class params_encoded_view;
    friend class segments;
    friend class segments_view;
    friend class segments_encoded;
    friend class segments_encoded_view;

    struct shared_impl;

    url_view_base() noexcept;
    explicit url_view_base(
        detail::url_impl const&) noexcept;

    ~url_view_base() = default;
    url_view_base(
        url_view_base const&) = default;
    url_view_base& operator=(
        url_view_base const&) = delete;

#ifndef BOOST_URL_DOCS
public:
#endif
    BOOST_URL_DECL
    std::size_t
    digest(std::size_t = 0) const noexcept;

public:
    //--------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------

    /** Return the maximum number of characters allowed in a URL.

        This does not include a null terminator.

        @par Exception Safety
        Throws nothing.
    */
    static
    constexpr
    std::size_t
    max_size() noexcept
    {
        return BOOST_URL_MAX_SIZE;
    }

    /** Return the number of characters in the URL.

        This function returns the number of
        characters in the URL, not including
        any null terminator, if present.

        @par Exception Safety
        Throws nothing.
    */
    std::size_t
    size() const noexcept
    {
        return u_.offset(id_end);
    }

    /** Return true if the URL is empty.

        An empty URL is a <em>relative-ref</em> with
        zero path segments.

        @par Exception Safety
        Throws nothing.
    */
    bool
    empty() const noexcept
    {
        return size() == 0;
    }

    /** Return a pointer to the first character of the view.

        This function returns a pointer to the
        first character of the view, which is
        not guaranteed to be null-terminated.

        @par Exception Safety
        Throws nothing.
    */
    char const*
    data() const noexcept
    {
        return u_.cs_;
    }

    /** Return the complete encoded URL

        This function returns the URL as a
        percent-encoded string.

        @par Exception Safety
        Throws nothing.
    */
    string_view
    string() const noexcept
    {
        return string_view(
            data(), size());
    }

    /** Return a shared persistent copy of the URL view

        This function returns a read-only copy of
        the URL view, with shared lifetime. The
        new URL view owns (persists) the underlying
        string.
        <br>
        The main benefit of this function over
        `std::make_shared` or `std::allocate_shared`
        is using a single allocation for both the
        new view and the character buffer. Thus,
        it requires access to the underlying object
        representation to achieve that.
        <br>
        This function makes a copy of the storage
        pointed to by this, and attaches it to a
        new constant @ref url_view returned in a
        shared pointer. The lifetime of the storage
        for the characters will extend for the
        lifetime of the shared object. This allows
        the new view to be copied and passed around
        after the original string buffer is destroyed.

        @par Example
        @code
        std::shared_ptr<url_view const> sp;
        {
            std::string s( "http://example.com" );
            url_view u( s );                        // u references characters in s

            assert( u.data() == s.data() );         // same buffer

            sp = u.persist();

            assert( sp->data() != s.data() );       // different buffer
            assert( sp->string() == s);             // same contents

            // s is destroyed and thus u
            // becomes invalid, but sp remains valid.
        }
        std::cout << *sp; // works
        @endcode
    */
    BOOST_URL_DECL
    std::shared_ptr<
        url_view const> persist() const;

    //--------------------------------------------
    //
    // Scheme
    //
    //--------------------------------------------

    /** Return true if this contains a scheme

        This function returns true if this
        contains a scheme. URLs with schemes
        are called absolute URLs.

        @par Example
        @code
        url_view u( "http://www.example.com" );

        assert( u.has_scheme() );
        @endcode

        @par BNF
        @code
        URI             = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

        absolute-URI    = scheme ":" hier-part [ "?" query ]

        scheme          = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1"
            >3.1. Scheme (rfc3986)</a>

        @see
            @ref scheme,
            @ref scheme_id.
    */
    BOOST_URL_DECL
    bool
    has_scheme() const noexcept;

    /** Return the scheme

        This function returns the scheme if it
        exists, without a trailing colon (':').
        Otherwise it returns an empty string.

        @par Example
        @code
        assert( url_view( "http://www.example.com" ).scheme() == "http" );
        @endcode

        @par BNF
        @code
        scheme          = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )

        URI             = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

        absolute-URI    = scheme ":" hier-part [ "?" query ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1"
            >3.1. Scheme (rfc3986)</a>

        @see
            @ref has_scheme,
            @ref scheme_id.
    */
    BOOST_URL_DECL
    string_view
    scheme() const noexcept;

    /** Return a constant representing the scheme

        This function returns a @ref urls::scheme constant
        to identify the scheme as a well-known scheme.
        If the scheme is not recognized, the value
        @ref urls::scheme::unknown is returned. If
        this does not contain a scheme, then
        @ref urls::scheme::none is returned.

        @par BNF
        @code
        URI             = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

        absolute-URI    = scheme ":" hier-part [ "?" query ]

        scheme          = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1"
            >3.1. Scheme (rfc3986)</a>

        @see @ref urls::scheme
    */
    BOOST_URL_DECL
    urls::scheme
    scheme_id() const noexcept;

    //--------------------------------------------
    //
    // Authority
    //
    //--------------------------------------------

    /** Return true if an authority is present.

        This function returns `true` if the URL
        contains an authority. The authority is
        always preceded by a double slash ("//").

        @par Example
        @code
        assert( url_view( "http://www.example.com/index.htm" ).has_authority() == true );

        assert( url_view( "//" ).has_authority() == true );

        assert( url_view( "/file.txt" ).has_authority() == false );
        @endcode

        @par BNF
        @code
        authority       = [ userinfo "@" ] host [ ":" port ]

        URI             = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

        absolute-URI    = scheme ":" hier-part [ "?" query ]

        URI-reference   = URI / relative-ref

        relative-ref    = relative-part [ "?" query ] [ "#" fragment ]

        hier-part       = "//" authority path-abempty
                        ; (more...)

        relative-part   = "//" authority path-abempty
                        ; (more...)

        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2"
            >3.2. Authority (rfc3986)</a>

        @see
            @ref encoded_authority.
    */
    bool
    has_authority() const noexcept
    {
        return u_.len(id_user) > 0;
    }

    /** Return the authority.

        This function returns the authority as a
        percent-encoded string.

        @par Example
        @code
        assert( url_view( "http://www.example.com/index.htm" ).encoded_authority() == "www.example.com" );
        @endcode

        @par BNF
        @code
        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2"
            >3.2. Authority (rfc3986)</a>

        @see
            @ref has_authority.
    */
    BOOST_URL_DECL
    string_view
    encoded_authority() const noexcept;

    /** Return the authority.

        This function returns the authority as a
        an @ref authority_view.

        @par Example
        @code
        assert( url_view( "http://www.example.com/index.htm" ).authority().encoded_host() == "www.example.com" );
        @endcode

        @par BNF
        @code
        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2"
            >3.2. Authority (rfc3986)</a>

        @see
            @ref has_authority.
    */
    BOOST_URL_DECL
    authority_view
    authority() const noexcept;

    //--------------------------------------------

    /** Return true if a userinfo is present.

        This function returns true if this
        contains a userinfo.

        @par Example
        @code
        url_view u( "http://user@example.com" );

        assert( u.has_userinfo() == true );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_userinfo,
            @ref userinfo.
    */
    BOOST_URL_DECL
    bool
    has_userinfo() const noexcept;

    /** Return the userinfo

        This function returns the userinfo
        as a percent-encoded string.

        @par Example
        @code
        assert( url_view( "http://user:pass@example.com" ).encoded_userinfo() == "user:pass" );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref has_userinfo,
            @ref userinfo.
    */
    BOOST_URL_DECL
    string_view
    encoded_userinfo() const noexcept;

    /** Return the userinfo

        This function returns the userinfo as a
        string with percent-decoding applied, using
        the optionally specified allocator.

        @par Example
        @code
        url_view u( "http://user:pass@example.com" );

        assert( u.userinfo() == "user:pass" );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Throws nothing

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref has_userinfo,
            @ref encoded_userinfo.
    */
    pct_encoded_view
    userinfo() const noexcept
    {
        pct_decode_opts opt;
        opt.plus_to_space = false;
        string_view s = encoded_userinfo();
        return
            detail::access::construct(
                s, u_.decoded_[id_user] +
                has_password() +
                u_.decoded_[id_pass], opt);
    }

    //--------------------------------------------

    /** Return the user

        This function returns the user portion of
        the userinfo as a percent-encoded string.

        @par Example
        @code
        url_view u( "http://user:pass@example.com" );

        assert( u.encoded_user() == "user" );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        user        = *( unreserved / pct-encoded / sub-delims )
        password    = *( unreserved / pct-encoded / sub-delims / ":" )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_password,
            @ref has_password,
            @ref password,
            @ref user.
    */
    BOOST_URL_DECL
    string_view
    encoded_user() const noexcept;

    /** Return the user

        This function returns the user portion of
        the userinfo as a string with percent-decoding
        applied, using the optionally specified
        allocator.

        @par Example
        @code
        assert( url_view( "http://user:pass@example.com" ).user() == "user" );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        user        = *( unreserved / pct-encoded / sub-delims )
        password    = *( unreserved / pct-encoded / sub-delims / ":" )
        @endcode

        @par Exception Safety
        Throws nothing

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_password,
            @ref encoded_user,
            @ref has_password,
            @ref password.
    */
    pct_encoded_view
    user() const noexcept
    {
        pct_decode_opts opt;
        opt.plus_to_space = false;
        string_view s = encoded_user();
        return
            detail::access::construct(
                s, u_.decoded_[id_user], opt);
    }

    /** Return true if this contains a password

        This function returns true if the userinfo
        contains a password (which may be empty).

        @par Example
        @code
        assert( url_view( "http://user@example.com" ).has_password() == false );

        assert( url_view( "http://user:pass@example.com" ).has_password() == true );

        assert( url_view( "http://:@" ).has_password() == true );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        user        = *( unreserved / pct-encoded / sub-delims )
        password    = *( unreserved / pct-encoded / sub-delims / ":" )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_password,
            @ref encoded_user,
            @ref password,
            @ref user.
    */
    BOOST_URL_DECL
    bool
    has_password() const noexcept;

    /** Return the password

        This function returns the password portion
        of the userinfo as a percent-encoded string.

        @par Example
        @code
        url_view u( "http://user:pass@example.com" );

        assert( u.encoded_user() == "user" );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        user        = *( unreserved / pct-encoded / sub-delims )
        password    = *( unreserved / pct-encoded / sub-delims / ":" )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_password,
            @ref has_password,
            @ref password,
            @ref user.
    */
    BOOST_URL_DECL
    string_view
    encoded_password() const noexcept;

    /** Return the password

        This function returns the password from the
        userinfo with percent-decoding applied.

        @par Exception Safety
        Throws nothing

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_password,
            @ref encoded_user,
            @ref has_password,
            @ref password.
    */
    pct_encoded_view
    password() const noexcept
    {
        pct_decode_opts opt;
        opt.plus_to_space = false;
        string_view s = encoded_password();
        return
            detail::access::construct(
                s, u_.decoded_[id_pass], opt);
    }

    //--------------------------------------------

    /** Return the type of host present, if any

        This function returns a
            @ref urls::host_type
        constant representing the type of
        host this contains, which may be
        @ref urls::host_type::none.

        @par Example
        @code
        assert( url_view( "/favicon.png" ).host_type() == host_type::none );

        assert( url_view( "http://example.com" ).host_type() == host_type::name );

        assert( url_view( "http://192.168.0.1" ).host_type() == host_type::ipv4 );
        @endcode

        @par BNF
        @code
        host        = IP-literal / IPv4address / reg-name

        IP-literal  = "[" ( IPv6address / IPvFuture  ) "]"

        IPvFuture   = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>

        @see
            @ref encoded_host,
            @ref has_port,
            @ref host,
            @ref port,
            @ref port_number.
    */
    urls::host_type
    host_type() const noexcept
    {
        return u_.host_type_;
    }

    /** Return the host

        This function returns the host portion of
        the authority as a percent-encoded string,

        @par Example
        @code
        assert( url_view( "/favicon.png" ).encoded_host() == "" );

        assert( url_view( "http://example.com" ).encoded_host() == "example.com" );

        assert( url_view( "http://192.168.0.1" ).encoded_host() == "192.168.0.1" );
        @endcode

        @par BNF
        @code
        host        = IP-literal / IPv4address / reg-name

        IP-literal  = "[" ( IPv6address / IPvFuture  ) "]"

        IPvFuture   = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )

        reg-name    = *( unreserved / pct-encoded / "-" / ".")
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>

        @see
            @ref has_port,
            @ref host,
            @ref host_type,
            @ref port,
            @ref port_number.
    */
    BOOST_URL_DECL
    string_view
    encoded_host() const noexcept;

    /** Return the hostname

        This function returns the hostname with
        percent-encoding.
        The hostname is formed from the host
        string, with the additional step of
        removing the enclosing square brackets
        when the host type is
        @ref host_type::ipv6 or
        @ref host_type::ipvfuture.

        @par BNF
        @code
        host        = IP-literal / IPv4address / reg-name

        IP-literal  = "[" ( IPv6address / IPvFuture  ) "]"
        @endif

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>

        @see
            @ref hostname.
    */
    BOOST_URL_DECL
    string_view
    encoded_hostname() const noexcept;

    /** Return the hostname

        This function returns the hostname with
        percent-encoding removed.
        The hostname is formed from the host
        string, with the additional step of
        removing the enclosing square brackets
        when the host type is
        @ref host_type::ipv6 or
        @ref host_type::ipvfuture.

        @par BNF
        @code
        host        = IP-literal / IPv4address / reg-name

        IP-literal  = "[" ( IPv6address / IPvFuture  ) "]"
        @endif

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>

        @see
            @ref encoded_hostname.
    */
    BOOST_URL_DECL
    pct_encoded_view
    hostname() const noexcept;

    /** Return the host

        This function returns the host portion of
        the authority as a string with percent-decoding
        applied, using the optionally specified
        allocator.

        @par Example
        @code
        assert( url_view( "/favicon.png" ).host() == "" );

        assert( url_view( "http://example.com" ).host() == "example.com" );

        assert( url_view( "http://192.168.0.1" ).host() == "192.168.0.1" );
        @endcode

        @par BNF
        @code
        host        = IP-literal / IPv4address / reg-name

        IP-literal  = "[" ( IPv6address / IPvFuture  ) "]"

        IPvFuture   = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )

        reg-name    = *( unreserved / pct-encoded / "-" / ".")
        @endcode

        @par Exception Safety
        Throws nothing

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>

        @see
            @ref encoded_host,
            @ref encoded_host_and_port,
            @ref has_port,
            @ref host_type,
            @ref port,
            @ref port_number.
    */
    pct_encoded_view
    host() const noexcept
    {
        pct_decode_opts opt;
        opt.plus_to_space = false;
        string_view s = encoded_host();
        return
            detail::access::construct(
                s, u_.decoded_[id_host], opt);
    }

    /** Return the host as an IPv4 address

        If @ref host_type equals @ref urls::host_type::ipv4,
        this function returns the corresponding
        @ref ipv4_address
        of the host if it exists, otherwise it
        returns the unspecified address which
        is equal to "0.0.0.0".

        @par BNF
        @code
        IPv4address = dec-octet "." dec-octet "." dec-octet "." dec-octet

        dec-octet   = DIGIT                 ; 0-9
                    / %x31-39 DIGIT         ; 10-99
                    / "1" 2DIGIT            ; 100-199
                    / "2" %x30-34 DIGIT     ; 200-249
                    / "25" %x30-35          ; 250-255
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>

        @see
            @ref ipv4_address.
    */
    BOOST_URL_DECL
    urls::ipv4_address
    ipv4_address() const noexcept;

    /** Return the host as an IPv6 address

        If @ref host_type equals
        @ref urls::host_type::ipv6, this
        function returns the corresponding
        @ref ipv6_address of the host if it
        exists, otherwise it returns the
        unspecified address which is equal
        to "0:0:0:0:0:0:0:0".

        @par BNF
        @code
        IPv6address =                            6( h16 ":" ) ls32
                    /                       "::" 5( h16 ":" ) ls32
                    / [               h16 ] "::" 4( h16 ":" ) ls32
                    / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
                    / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
                    / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
                    / [ *4( h16 ":" ) h16 ] "::"              ls32
                    / [ *5( h16 ":" ) h16 ] "::"              h16
                    / [ *6( h16 ":" ) h16 ] "::"

        ls32        = ( h16 ":" h16 ) / IPv4address
                    ; least-significant 32 bits of address

        h16         = 1*4HEXDIG
                    ; 16 bits of address represented in hexadecimal
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>

        @see
            @ref ipv6_address.
    */
    BOOST_URL_DECL
    urls::ipv6_address
    ipv6_address() const noexcept;

    /** Return the host as an IPvFuture string

        If @ref host_type equals
        @ref urls::host_type::ipvfuture, this
        function returns a string representing
        the address. Otherwise it returns the
        empty string.

        @par BNF
        @code
        IPvFuture  = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>
    */
    BOOST_URL_DECL
    string_view
    ipvfuture() const noexcept;

    /** Return true if the URL contains a port

        This function returns true if the
        authority contains a port.

        @par BNF
        @code
        authority   = [ userinfo "@" ] host [ ":" port ]

        port        = *DIGIT
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3"
            >3.2.3. Port (rfc3986)</a>

        @see
            @ref encoded_host,
            @ref encoded_host_and_port
            @ref host,
            @ref host_type,
            @ref port,
            @ref port_number.
    */
    BOOST_URL_DECL
    bool
    has_port() const noexcept;

    /** Return the port

        This function returns the port specified
        in the authority, or an empty string if
        there is no port.

        @par BNF
        @code
        port        = *DIGIT
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3"
            >3.2.3. Port (rfc3986)</a>

        @see
            @ref encoded_host,
            @ref encoded_host_and_port
            @ref has_port,
            @ref host,
            @ref host_type,
            @ref port_number.
    */
    BOOST_URL_DECL
    string_view
    port() const noexcept;

    /** Return the port as an integer.

        This function returns the port as an
        integer if the authority specifies
        a port and the number can be represented.
        Otherwise it returns zero.

        @par BNF
        @code
        port        = *DIGIT
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3"
            >3.2.3. Port (rfc3986)</a>

        @see
            @ref encoded_host,
            @ref encoded_host_and_port
            @ref has_port,
            @ref host,
            @ref host_type,
            @ref port,
    */
    BOOST_URL_DECL
    std::uint16_t
    port_number() const noexcept;

    /** Return the host and port

        This function returns the host and
        port of the authority as a single
        percent-encoded string.

        @par BNF
        @code
        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3"
            >3.2.3. Port (rfc3986)</a>

        @see
            @ref encoded_host,
            @ref encoded_host_and_port
            @ref has_port,
            @ref host,
            @ref host_type,
            @ref port,
    */
    BOOST_URL_DECL
    string_view
    encoded_host_and_port() const noexcept;

    //--------------------------------------------

    /** Return the origin

        This function returns the origin as
        a percent-encoded string. The origin
        consists of the scheme and authority.
        This string will be empty if no
        authority is present.
    */
    BOOST_URL_DECL
    string_view
    encoded_origin() const noexcept;

    //--------------------------------------------
    //
    // Path
    //
    //--------------------------------------------

    /** Return true if the path is absolute.

        This function returns true if the path
        begins with a forward slash ('/').
    */
    bool
    is_path_absolute() const noexcept
    {
        return
            u_.len(id_path) > 0 &&
            u_.cs_[u_.offset(id_path)] == '/';
    }

    /** Return the path.

        This function returns the path as a
        percent-encoded string.

        @par BNF
        @code
        path          = [ "/" ] segment *( "/" segment )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
            >3.3. Path (rfc3986)</a>
    */
    string_view
    encoded_path() const noexcept
    {
        return u_.get(id_path);
    }

    /** Return the path

        This function returns the path as a
        string with percent-decoding applied.

        @par BNF
        @code
        query           = *( pchar / "/" / "?" )

        query-part      = [ "?" query ]
        @endcode

        @par Exception Safety
        Throws nothing

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
            >3.4. Query (rfc3986)</a>

        @see
            @ref encoded_query,
            @ref has_query.
    */
    pct_encoded_view
    path() const noexcept
    {
        pct_decode_opts opt;
        opt.plus_to_space = false;
        string_view s = encoded_path();
        return
            detail::access::construct(
                s, u_.decoded_[id_path], opt);
    }

    /** Return the path segments

        This function returns the path segments as
        a read-only bidirectional range.

        @par BNF
        @code
        path          = [ "/" ] segment *( "/" segment )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
            >3.3. Path (rfc3986)</a>
    */
    segments_encoded_view
    encoded_segments() const noexcept
    {
        return segments_encoded_view(
            encoded_path(), u_.nseg_);
    }

    /** Return the path segments

        This function returns the path segments as
        a read-only bidirectional range.

        @par BNF
        @code
        path          = [ "/" ] segment *( "/" segment )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
            >3.3. Path (rfc3986)</a>
    */
    segments_view
    segments() const noexcept
    {
        return {encoded_path(), u_.nseg_};
    }

    //--------------------------------------------
    //
    // Query
    //
    //--------------------------------------------

    /** Return true if this contains a query

        This function returns true if this
        contains a query.

        @par BNF
        @code
        query           = *( pchar / "/" / "?" )

        query-part      = [ "?" query ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
            >3.4. Query (rfc3986)</a>

        @see
            @ref encoded_query,
            @ref query.
    */
    BOOST_URL_DECL
    bool
    has_query() const noexcept;

    /** Return the query

        This function returns the query as
        a percent-encoded string.

        @par BNF
        @code
        query           = *( pchar / "/" / "?" )

        query-part      = [ "?" query ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
            >3.4. Query (rfc3986)</a>

        @see
            @ref encoded_query,
            @ref query.
    */
    BOOST_URL_DECL
    string_view
    encoded_query() const noexcept;

    /** Return the query

        This function returns the query as a
        string with percent-decoding applied.

        When plus signs appear in the query
        portion of the URL, they are converted
        to spaces automatically upon decoding.
        This behavior can be changed by setting
        decode options.

        @par BNF
        @code
        query           = *( pchar / "/" / "?" )

        query-part      = [ "?" query ]
        @endcode

        @par Exception Safety
        Throws nothing

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
            >3.4. Query (rfc3986)</a>

        @see
            @ref encoded_query,
            @ref has_query.
    */
    pct_encoded_view
    query() const noexcept
    {
        pct_decode_opts opt;
        opt.plus_to_space = true;
        string_view s = encoded_query();
        return
            detail::access::construct(
                s, u_.decoded_[id_query], opt);
    }

    /** Return the query parameters

        This function returns the query
        parameters as a non-modifiable
        forward range of key/value pairs.
        Each string returned by the container
        is percent-encoded.

        @par BNF
        @code
        query-params    = [ query-param ] *( "&" [ query-param ] )

        query-param     = key [ "=" value ]

        @endcode
    */
    BOOST_URL_DECL
    params_encoded_view
    encoded_params() const noexcept;

    /** Return the query parameters

        This function returns the query
        parameters as a non-modifiable
        forward range of key/value pairs
        where each returned string has
        percent-decoding applied.

        @par BNF
        @code
        query-params    = [ query-param ] *( "&" [ query-param ] )

        query-param     = key [ "=" value ]
        @endcode

    */
    BOOST_URL_DECL
    params_view
    params() const noexcept;

    //--------------------------------------------
    //
    // Fragment
    //
    //--------------------------------------------

    /** Return true if a fragment exists.

        This function returns true if this
        contains a fragment.

        @par BNF
        @code
        URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

        relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.5"
            >3.5. Fragment (rfc3986)</a>

        @see
            @ref encoded_fragment,
            @ref fragment.
    */
    BOOST_URL_DECL
    bool
    has_fragment() const noexcept;

    /** Return the fragment.

        This function returns the fragment as a
        percent-encoded string.

        @par BNF
        @code
        fragment        = *( pchar / "/" / "?" )

        pchar           = unreserved / pct-encoded / sub-delims / ":" / "@"
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.5"
            >3.5. Fragment (rfc3986)</a>

        @see
            @ref fragment,
            @ref has_fragment,
            @ref pchars.
    */
    BOOST_URL_DECL
    string_view
    encoded_fragment() const noexcept;

    /** Return the fragment.

        This function returns the fragment as a
        string with percent-decoding applied.

        @par BNF
        @code
        fragment        = *( pchar / "/" / "?" )

        fragment-part   = [ "#" fragment ]
        @endcode

        @par Exception Safety
        Throws nothing

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.5"
            >3.5. Fragment (rfc3986)</a>

        @see
            @ref encoded_fragment,
            @ref has_fragment.
    */
    pct_encoded_view
    fragment() const noexcept
    {
        pct_decode_opts opt;
        opt.plus_to_space = false;
        string_view s = encoded_fragment();
        return
            detail::access::construct(
                s, u_.decoded_[id_frag], opt);
    }

    //--------------------------------------------
    //
    // Comparison
    //
    //--------------------------------------------

    /** Compare two URLs.

        This function compares two URLs
        according to Syntax-Based comparison
        algorithm.

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2"
            >6.2.2 Syntax-Based Normalization (rfc3986)</a>
    */
    BOOST_URL_DECL
    int
    compare(url_view_base const& other) const noexcept;

    /** Return `true` if two url_views are equal.

        Two url_views are equal when their
        normalized representations are the same.

        Two URLs can compare equal even if their
        serialized representations are not
        identical strings.

        @par Example
        @code
        url_view u1( "HtTp://cXaMpLe.oRG/" );
        url_view u2( "http://example.org/" );
        assert( u1.compare(u2) == 0 );
        @endcode

        @par Effects
        `return url( lhs ).normalize() == url( rhs ).normalize();`

        @par Complexity
        Linear in string size.

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator==(
        url_view_base const& lhs,
        url_view_base const& rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    /** Return `true` if two url_views are not equal.

        Two url_views are equal when their
        normalized representations are the same.

        Two URLs can compare not equal even if
        their serialized representations represent
        the same resource.

        @par Effects
        `return ! url( lhs ).normalize() == url( rhs ).normalize();`

        @par Complexity
        Linear in string size.

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator!=(
        url_view_base const& lhs,
        url_view_base const& rhs) noexcept
    {
        return ! (lhs == rhs);
    }

    /** Lexicographically compares two url_views

        Each URL component is compared
        alphabetically on a character by character
        basis as if each was normalized first.

        Two URLs can compare equal even if their
        serialized representations are not
        identical strings.

        @par Complexity
        Linear in string sizes.

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator<(
        url_view_base const& lhs,
        url_view_base const& rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    /** Lexicographically compares two url_views

        Each URL component is compared
        alphabetically on a character by character
        basis as if each was normalized first.

        Two URLs can compare equal even if their
        serialized representations are not
        identical strings.

        @par Complexity
        Linear in string sizes.

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator<=(
        url_view_base const& lhs,
        url_view_base const& rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    /** Lexicographically compares two url_views

        Each URL component is compared
        alphabetically on a character by character
        basis as if each was normalized first.

        Two URLs can compare equal even if their
        serialized representations are not
        identical strings.

        @par Complexity
        Linear in string sizes.

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator>(
        url_view_base const& lhs,
        url_view_base const& rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    /** Lexicographically compares two url_views

        Each URL component is compared
        alphabetically on a character by character
        basis as if each was normalized first.

        Two URLs can compare equal even if their
        serialized representations are not
        identical strings.

        @par Complexity
        Linear in string sizes.

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator>=(
        url_view_base const& lhs,
        url_view_base const& rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

    // hidden friend
    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        url_view_base const& u)
    {
        return os << u.string();
    }
};

//------------------------------------------------

/** Format the encoded URL to the output stream

    This function serializes the encoded URL
    to the output stream.

    @par Example
    @code
    url_view_base u( "http://www.example.com/index.htm" );

    std::cout << u << std::endl;
    @endcode

    @return A reference to the output stream, for chaining

    @param os The output stream to write to.

    @param u The URL to write.
*/
std::ostream&
operator<<(
    std::ostream& os,
    url_view_base const& u);

} // urls
} // boost

#endif
