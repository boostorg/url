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

/** Common observer functions for all URLs

    This base class is used by the library to
    provide all of the observer member functions
    of URL containers. Users should not use this
    class directly. Instead, construct an instance
    of one of the containers or call a parsing
    function:

    @par Containers
        @li @ref url
        @li @ref url_view
        @li @ref static_url

    @par Parsing Functions
        @li @ref parse_absolute_uri
        @li @ref parse_origin_form
        @li @ref parse_relative_ref
        @li @ref parse_uri
        @li @ref parse_uri_reference
*/
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

    BOOST_URL_DECL
    url_view_base() noexcept;
    BOOST_URL_DECL
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

    /** Return the maximum number of characters possible in a URL

        Currently the limit is either 2^32-2
        characters or 2^64-2 characters,
        depending on the system architecture.
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

    /** Return the number of characters in the URL

        This function returns the number of
        characters in the encoded form of the
        URL, not including any null terminator,
        if present.

        @par Example
        @code
        url_view u( "file:///Program%20Files" );

        assert( u.size() == 23 );
        @endcode

        @par Exception Safety
        Throws nothing.
    */
    std::size_t
    size() const noexcept
    {
        return u_.offset(id_end);
    }

    /** Return true if the URL is empty

        An empty URL is a <em>relative-ref</em> with
        zero path segments.

        @par Example
        @code
        url_view u;
        assert( u.empty() );
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://www.rfc-editor.org/rfc/rfc3986.html#section-4.2"
            >4.2.  Relative Reference (rfc3986)</a>
    */
    bool
    empty() const noexcept
    {
        return size() == 0;
    }

    /** Return a pointer to the URL's character buffer

        This function returns a pointer to
        the first character of the URL, which
        is not guaranteed to be null-terminated.

        @par Exception Safety
        Throws nothing.
    */
    char const*
    data() const noexcept
    {
        return u_.cs_;
    }

    /** Return the URL string

        This function returns the entire URL,
        with any percent-escaped characters
        preserved.

        @par Exception Safety
        Throws nothing.
    */
    string_view
    string() const noexcept
    {
        return string_view(
            data(), size());
    }

    /** Return a shared, persistent copy of the URL

        This function returns a read-only copy of
        the URL, with shared lifetime. The returned
        value owns (persists) the underlying string.
        The algorithm used to create the value
        minimizes the number of individual memory
        allocations, making it more efficient than
        when using direct standard library functions.

        @par Example
        @code
        std::shared_ptr< url_view const > sp;
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
        contains a scheme.

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
        url_view u( "http://www.example.com" );
        
        assert( u.scheme() == "http" );
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

        @par Example
        @code
        url_view u( "wss://www.example.com/crypto.cgi" );

        assert( u.scheme_id() == scheme::wss );
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
            @ref urls::scheme
    */
    BOOST_URL_DECL
    urls::scheme
    scheme_id() const noexcept;

    //--------------------------------------------
    //
    // Authority
    //
    //--------------------------------------------

    /** Return true if an authority is present

        This function returns `true` if the URL
        contains an authority. The authority is
        always preceded by a double slash ("//").

        @par Example
        @code
        url_view u( "http://www.example.com/index.htm" );

        assert( u.has_authority() );
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
            @ref authority,
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
        url_view u( "file://Network%20Drive/My%2DFiles" );

        assert( u.encoded_authority() == "Network%20Drive" );
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
            @ref authority,
            @ref has_authority.
    */
    BOOST_URL_DECL
    string_view
    encoded_authority() const noexcept;

    /** Return the authority

        This function returns the authority as a
        an @ref authority_view.

        @par Example
        @code
        url_view u( "https://www.example.com:8080/index.htm" );

        authority_view a = u.authority();
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
            @ref encoded_authority,
            @ref has_authority.
    */
    BOOST_URL_DECL
    authority_view
    authority() const noexcept;

    //--------------------------------------------

    /** Return true if a userinfo is present

        This function returns true if this
        contains a userinfo.

        @par Example
        @code
        url_view u( "http://jane%2Ddoe:pass@example.com" );

        assert( u.has_userinfo() );
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
        url_view u( "http://jane%2Ddoe:pass@example.com" );

        assert( u.encoded_userinfo() == "jane%2Ddoe:pass" );
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
        url_view u( "http://jane%2Ddoe:pass@example.com" );

        assert( u.userinfo() == "jane-doe" );
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
            @ref encoded_userinfo,
            @ref has_userinfo.
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
        url_view u( "http://jane%2Ddoe:pass@example.com" );

        assert( u.encoded_user() == "jane%2Ddoe" );
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
        url_view u( "http://jane%2Ddoe:pass@example.com" );

        assert( u.user() == "jane-doe" );
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
        url_view u( "http://jane%2Ddoe:pass@example.com" );

        assert( u.has_password() );
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
        url_view u( "http://jane%2Ddoe:pass@example.com" );

        assert( u.encoded_password() == "pass" );
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
            @ref encoded_user,
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

        @par Example
        @code
        url_view u( "http://jane%2Ddoe:pass@example.com" );

        assert( u.password() == "pass" );
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
            @ref user.
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

    /** Return the type of host present, or none

        This function returns a
            @ref urls::host_type
        constant representing the type of
        host this contains, which may be
        @ref urls::host_type::none.

        @par Example
        @code
        url_view u( "https://192.168.0.1/local.htm" );

        assert( u.host_type() == host_type::ipv4 );
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
            @ref encoded_hostname,
            @ref encoded_host_and_port,
            @ref has_port,
            @ref host,
            @ref hostname,
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
        url_view u( "https://www%2droot.example.com/" );

        assert( u.encoded_host() == "www%2droot.example.com" );
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
            @ref encoded_hostname,
            @ref encoded_host_and_port,
            @ref has_port,
            @ref host,
            @ref hostname,
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

        @par Example
        @code
        url_view u( "wss://[2001:0db8::0370:7334]/index.htm" );

        assert( u.encoded_hostname() == "2001:0db8::0370:7334" );
        @endcode

        @par BNF
        @code
        host        = IP-literal / IPv4address / reg-name

        IP-literal  = "[" ( IPv6address / IPvFuture  ) "]"
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>

        @see
            @ref encoded_host,
            @ref encoded_host_and_port,
            @ref has_port,
            @ref host,
            @ref hostname,
            @ref host_type,
            @ref port,
            @ref port_number.
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

        @par Example
        @code
        url_view u( "https://www%2droot.example.com/" );

        assert( u.hostname() == "www-root.example.com" );
        @endcode

        @par BNF
        @code
        host        = IP-literal / IPv4address / reg-name

        IP-literal  = "[" ( IPv6address / IPvFuture  ) "]"
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>

        @see
            @ref encoded_host,
            @ref encoded_hostname,
            @ref encoded_host_and_port,
            @ref has_port,
            @ref host,
            @ref host_type,
            @ref port,
            @ref port_number.
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
        url_view u( "https://www%2droot.example.com/" );

        assert( u.host() == "www-root.example.com" );
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
            @ref encoded_hostname,
            @ref encoded_host_and_port,
            @ref has_port,
            @ref hostname,
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

        @par Example
        @code
        url_view u( "http://127.0.0.1/index.htm?user=win95" );

        ipv4_address ip = u.ipv4_address();
        @endcode

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
            @ref encoded_hostname,
            @ref hostname,
            @ref host_type,
            @ref ipv6_address,
            @ref ipvfuture,
            @ref urls::ipv4_address.
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

        @par Example
        @code
        url_view u( "ftp://[::1]" );

        ipv6_address ip = u.ipv6_address();

        assert( ip.is_loopback() );
        @endcode

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
            @ref encoded_hostname,
            @ref hostname,
            @ref host_type,
            @ref ipv4_address,
            @ref ipvfuture,
            @ref urls::ipv6_address.
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

        @par Example
        @code
        url_view u( "http://[v1fe.d:9]" );

        assert( u.ipvfuture() == "v1fe.d:9" );
        @endcode

        @par BNF
        @code
        IPvFuture  = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2. Host (rfc3986)</a>

        @see
            @ref encoded_hostname,
            @ref hostname,
            @ref host_type,
            @ref ipv4_address,
            @ref ipv6_address,
    */
    BOOST_URL_DECL
    string_view
    ipvfuture() const noexcept;

    /** Return true if the URL contains a port

        This function returns true if the
        authority contains a port.

        @par Example
        @code
        url_view u( "wss://www.example.com:443" );

        assert( u.has_port() );
        @endcode

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
            @ref encoded_hostname,
            @ref encoded_host_and_port,
            @ref host,
            @ref hostname,
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

        @par Example
        @code
        url_view u( "http://localhost.com:8080" );

        assert( u.port() == "8080" );
        @endcode

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
            @ref encoded_hostname,
            @ref encoded_host_and_port,
            @ref has_port,
            @ref host,
            @ref hostname,
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

        @par Example
        @code
        url_view u( "http://localhost.com:8080" );

        assert( u.port_number() == 8080 );
        @endcode

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
            @ref encoded_hostname,
            @ref encoded_host_and_port,
            @ref has_port,
            @ref host,
            @ref hostname,
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

        @par Example
        @code
        url_view u( "http://www.example.com:8080/index.htm" );

        assert( u.encoded_host_and_port() == "www.example.com:8080" );
        @endcode

        @par BNF
        @code
        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
            >3.2.2.  Host (rfc3986)</a>
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3"
            >3.2.3. Port (rfc3986)</a>

        @see
            @ref encoded_host,
            @ref encoded_hostname,
            @ref encoded_host_and_port,
            @ref has_port,
            @ref host,
            @ref hostname,
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

        @par Example
        @code
        url_view u( "http://www.example.com:8080/index.htm?text=none#h1" );

        assert( u.encoded_origin() == "http://www.example.com:8080" );
        @endcode

        @par Exception Safety
        Throws nothing.
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

        @par Example
        @code
        url_view u( "/path/to/file.txt" );

        assert( u.is_path_absolute() );
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
            >3.3.  Path (rfc3986)</a>
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

        @par Example
        @code
        url_view u( "file:///Program%20Files/Games/config.ini" );

        assert( u.encoded_path() == "/Program%20Files/Games/config.ini" );
        @endcode

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

        @par Example
        @code
        url_view u( "file:///Program%20Files/Games/config.ini" );

        assert( u.path() == "/Program Files/Games/config.ini" );
        @endcode

        @par BNF
        @code
        query           = *( pchar / "/" / "?" )

        query-part      = [ "?" query ]
        @endcode

        @par Exception Safety
        Throws nothing

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
            >3.3. Path (rfc3986)</a>

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

        @par Example
        @code
        url_view u( "/sql?id=42&col=name&page-size=20" );

        assert( u.has_query() );
        @endcode

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

        @par Example
        @code
        url_view u( "/sql?id=42&name=jane%2Ddoe&page+size=20" );

        assert( u.encoded_query() == "id=42&name=jane%2Ddoe&page+size=20" );
        @endcode

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

        @par Example
        @code
        url_view u( "/sql?id=42&name=jane%2Ddoe&page+size=20" );

        assert( u.query() == "id=42&name=jane-doe&page size=20" );
        @endcode

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

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
            >3.4. Query (rfc3986)</a>
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

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
            >3.4. Query (rfc3986)</a>
    */
    BOOST_URL_DECL
    params_view
    params() const noexcept;

    //--------------------------------------------
    //
    // Fragment
    //
    //--------------------------------------------

    /** Return true if a fragment exists

        This function returns true if this
        contains a fragment.

        @par Example
        @code
        url_view u( "http://www.example.com/index.htm#a%2D1" );

        assert( u.has_fragment() );
        @endcode

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

    /** Return the fragment

        This function returns the fragment as a
        percent-encoded string.

        @par Example
        @code
        url_view u( "http://www.example.com/index.htm#a%2D1" );

        assert( u.encoded_fragment() == "a%2D1" );
        @endcode

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

    /** Return the fragment

        This function returns the fragment as a
        string with percent-decoding applied.

        @par Example
        @code
        url_view u( "http://www.example.com/index.htm#a%2D1" );

        assert( u.fragment() == "a-1" );
        @endcode

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

    /** Return the result of comparing this with another URL

        This function compares two URLs
        according to Syntax-Based comparison
        algorithm.

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2"
            >6.2.2 Syntax-Based Normalization (rfc3986)</a>

        @return -1 if `*this < other`, 0 if
            `this == other`, and 1 if `this > other`.
    */
    BOOST_URL_DECL
    int
    compare(url_view_base const& other) const noexcept;

    /** Return the result of comparing two URLs

        The URLs are compared character
        by character as if they were first
        normalized.

        @par Effects
        @code
        return url( u0 ).normalize() == url( u1 ).normalize();
        @endcode

        @par Complexity
        Linear in `min( u0.size(), u1.size() )`

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator==(
        url_view_base const& u0,
        url_view_base const& u1) noexcept
    {
        return u0.compare(u1) == 0;
    }

    /** Return the result of comparing two URLs

        The URLs are compared character
        by character as if they were first
        normalized.

        @par Effects
        @code
        return url( u0 ).normalize() != url( u1 ).normalize();
        @endcode

        @par Complexity
        Linear in `min( u0.size(), u1.size() )`

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator!=(
        url_view_base const& u0,
        url_view_base const& u1) noexcept
    {
        return ! (u0 == u1);
    }

    /** Return the result of comparing two URLs

        The URLs are compared character
        by character as if they were first
        normalized.

        @par Effects
        @code
        return url( u0 ).normalize() < url( u1 ).normalize();
        @endcode

        @par Complexity
        Linear in `min( u0.size(), u1.size() )`

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator<(
        url_view_base const& u0,
        url_view_base const& u1) noexcept
    {
        return u0.compare(u1) < 0;
    }

    /** Return the result of comparing two URLs

        The URLs are compared character
        by character as if they were first
        normalized.

        @par Effects
        @code
        return url( u0 ).normalize() <= url( u1 ).normalize();
        @endcode

        @par Complexity
        Linear in `min( u0.size(), u1.size() )`

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator<=(
        url_view_base const& u0,
        url_view_base const& u1) noexcept
    {
        return u0.compare(u1) <= 0;
    }

    /** Return the result of comparing two URLs

        The URLs are compared character
        by character as if they were first
        normalized.

        @par Effects
        @code
        return url( u0 ).normalize() > url( u1 ).normalize();
        @endcode

        @par Complexity
        Linear in `min( u0.size(), u1.size() )`

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator>(
        url_view_base const& u0,
        url_view_base const& u1) noexcept
    {
        return u0.compare(u1) > 0;
    }

    /** Return the result of comparing two URLs

        The URLs are compared character
        by character as if they were first
        normalized.

        @par Effects
        @code
        return url( u0 ).normalize() >= url( u1 ).normalize();
        @endcode

        @par Complexity
        Linear in `min( u0.size(), u1.size() )`

        @par Exception Safety
        Throws nothing
    */
    friend
    bool
    operator>=(
        url_view_base const& u0,
        url_view_base const& u1) noexcept
    {
        return u0.compare(u1) >= 0;
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

/** Format the URL to the output stream

    This function serializes the URL to
    the specified output stream. Any
    percent-escapes are emitted as-is;
    no decoding is performed.

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
