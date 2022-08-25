//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_AUTHORITY_VIEW_HPP
#define BOOST_URL_AUTHORITY_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/host_type.hpp>
#include <boost/url/ipv4_address.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/decode.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/url_impl.hpp>
#include <boost/assert.hpp>
#include <cstddef>
#include <iosfwd>
#include <utility>

namespace boost {
namespace urls {

/** A non-owning reference to a valid authority

    Objects of this type represent valid authority
    strings constructed from a parsed, external
    character buffer whose storage is managed
    by the caller. That is, it acts like a
    @ref string_view in terms of ownership.
    The caller is responsible for ensuring
    that the lifetime of the underlying
    character buffer extends until it is no
    longer referenced.

    @par Example 1
    Construction from a string parses the input
    as an <em>authority</em> and throws an
    exception on error. Upon success, the
    constructed object points to the passed
    character buffer; ownership is not
    transferred.
    @code
    authority_view a( "user:pass@www.example.com:8080" );
    @endcode

    @par Example 2
    The parsing function @ref parse_authority returns
    a @ref result containing either a valid
    @ref authority_view upon succcess, otherwise it
    contain an error. The error can be converted to
    an exception by the caller if desired:
    @code
    result< authority_view > rv = parse_authority( "user:pass@www.example.com:8080" );
    @endcode

    @par BNF
    @code
    authority     = [ userinfo "@" ] host [ ":" port ]

    userinfo      = user [ ":" [ password ] ]

    user          = *( unreserved / pct-encoded / sub-delims )
    password      = *( unreserved / pct-encoded / sub-delims / ":" )

    host          = IP-literal / IPv4address / reg-name

    port          = *DIGIT
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2"
        >3.2. Authority (rfc3986)</a>

    @see
        @ref parse_authority.
*/
class BOOST_SYMBOL_VISIBLE
    authority_view
    : private detail::parts_base
{
    detail::url_impl u_;

#ifndef BOOST_URL_DOCS
    // VFALCO docca emits this erroneously
    friend struct detail::url_impl;
#endif

    explicit
    authority_view(
        detail::url_impl const& u) noexcept;

public:
    //--------------------------------------------
    //
    // Special Members
    //
    //--------------------------------------------

    /** Destructor
    */
    BOOST_URL_DECL
    virtual
    ~authority_view();

    /** Constructor

        Default constructed authorities
        refer to a string with zero length,
        which is always valid. This matches
        the grammar for a zero-length host.

        @par Exception Safety
        Throws nothing.

        @par Specification
    */
    BOOST_URL_DECL
    authority_view() noexcept;

    /** Construct from a string.

        This function attempts to construct
        an authority from the string `s`,
        which must be a valid ['authority] or
        else an exception is thrown. Upon
        successful construction, the view
        refers to the characters in the
        buffer pointed to by `s`.
        Ownership is not transferred; The
        caller is responsible for ensuring
        that the lifetime of the buffer
        extends until the view is destroyed.

        @par BNF
        @code
        authority     = [ userinfo "@" ] host [ ":" port ]

        userinfo      = user [ ":" [ password ] ]

        user          = *( unreserved / pct-encoded / sub-delims )
        password      = *( unreserved / pct-encoded / sub-delims / ":" )

        host          = IP-literal / IPv4address / reg-name

        port          = *DIGIT
        @endcode

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2"
            >3.2. Authority (rfc3986)</a>

        @see
            @ref parse_authority.
    */
    BOOST_URL_DECL
    explicit
    authority_view(string_view s);

    /** Constructor
    */
    BOOST_URL_DECL
    authority_view(
        authority_view const&) noexcept;

    /** Assignment
    */
    BOOST_URL_DECL
    authority_view&
    operator=(
        authority_view const&) noexcept;

    //--------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------

    /** Return the maximum number of characters possible

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

    /** Return the number of characters in the authority

        This function returns the number of
        characters in the authority.

        @par Example
        @code
        assert( authority_view( "user:pass@www.example.com:8080" ).size() == 30 );
        @endcode

        @par Exception Safety
        Throws nothing.
    */
    std::size_t
    size() const noexcept
    {
        return u_.offset(id_end);
    }

    /** Return true if the authority is empty

        An empty authority has an empty host,
        no userinfo, and no port.

        @par Example
        @code
        assert( authority_view( "" ).empty() );
        @endcode

        @par Exception Safety
        Throws nothing.
    */
    bool
    empty() const noexcept
    {
        return size() == 0;
    }

    /** Return a pointer to the first character

        This function returns a pointer to the
        beginning of the view, which is not
        guaranteed to be null-terminated.

        @par Exception Safety
        Throws nothing.
    */
    char const*
    data() const noexcept
    {
        return u_.cs_;
    }

    /** Return the complete authority

        This function returns the authority
        as a percent-encoded string.

        @par Example
        @code
        assert( parse_authority( "www.example.com" ).value().encoded_authority() == "www.example.com" );
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
    */
    string_view
    string() const noexcept
    {
        return string_view(data(), size());
    }

    //--------------------------------------------
    //
    // Userinfo
    //
    //--------------------------------------------

    /** Return true if this contains a userinfo

        This function returns true if this
        contains a userinfo.

        @par Example
        @code
        authority_view a = parse_authority( "user@example.com" ).value();

        assert( a.has_userinfo() == true );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_userinfo,
            @ref userinfo.
    */
    BOOST_URL_DECL
    bool
    has_userinfo() const noexcept;

    /** Return the encoded userinfo

        This function returns the userinfo
        as a percent-encoded string.

        @par Example
        @code
        assert( parse_authority( "user:pass@example.com" ).value().encoded_userinfo() == "user:pass" );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
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
        string with percent-decoding applied.

        @par Example
        @code
        authority_view a = parse_authority( "user:pass@example.com" ).value();

        assert( a.userinfo() == "user:pass" );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Throws nothing

        @par Specification
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref has_userinfo,
            @ref encoded_userinfo.
    */
    decode_view
    userinfo() const noexcept
    {
        decode_opts opt;
        opt.plus_to_space = false;
        return detail::access::construct(
            encoded_userinfo(),
            u_.decoded_[id_user] +
                has_password() + u_.decoded_[id_pass],
            opt);
    }

    //--------------------------------------------

    /** Return the encoded user

        This function returns the user portion of
        the userinfo as a percent-encoded string.

        @par Example
        @code
        authority_view a = parse_authority( "user:pass@example.com" ).value();

        assert( a.encoded_user() == "user" );
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
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_password,
            @ref has_password,
            @ref password,
            @ref user.
    */
    string_view
    encoded_user() const noexcept
    {
        return u_.get(id_user);
    }

    /** Return the user

        This function returns the user portion of
        the userinfo as a string with percent-decoding
        applied.

        @par Example
        @code
        assert( parse_authority( "user:pass@example.com" ).value().user() == "user" );
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
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_password,
            @ref encoded_user,
            @ref has_password,
            @ref password.
    */
    decode_view
    user() const noexcept
    {
        decode_opts opt;
        opt.plus_to_space = false;
        return detail::access::construct(
            encoded_user(), u_.decoded_[id_user], opt);
    }

    /** Return true if this contains a password

        This function returns true if the userinfo
        contains a password (which may be empty).

        @par Example
        @code
        assert( parse_authority( "user@example.com" ).value().has_password() == false );

        assert( parse_authority( "user:pass@example.com" ).value().has_password() == true );

        assert( parse_authority( ":@" ).value().has_password() == true );
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
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
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

    /** Return the encoded password

        This function returns the password portion
        of the userinfo as a percent-encoded string.

        @par Example
        @code
        authority_view a = parse_authority( "user:pass@example.com" ).value();

        assert( a.encoded_user() == "user" );
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
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
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

        @par Example
        @code
        authority_view a( "user:pass@example.com" );

        assert( a.password() == "pass" );
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
    decode_view
    password() const noexcept
    {
        decode_opts opt;
        opt.plus_to_space = false;
        return detail::access::construct(
            encoded_password(), u_.decoded_[id_pass], opt);
    }

    //--------------------------------------------
    //
    // Host
    //
    //--------------------------------------------

    /** Return the type of host present, if any

        This function returns a
            @ref urls::host_type
        constant representing the type of
        host this contains, which may be
        @ref urls::host_type::none.

        @par Example
        @code
        assert( authority_view().host_type() == host_type::none );

        assert( parse_authority( "example.com" ).value().host_type() == host_type::name );

        assert( parse_authority( "192.168.0.1" ).value().host_type() == host_type::ipv4 );
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

    /** Return the encoded host

        This function returns the host portion of
        the authority as a percent-encoded string,

        @par Example
        @code
        assert( parse_authority( "" ).value().encoded_host() == "" );

        assert( parse_authority( "http://example.com" ).value().encoded_host() == "example.com" );

        assert( parse_authority( "http://192.168.0.1" ).value().encoded_host() == "192.168.0.1" );
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

    /** Return the host

        This function returns the host portion of
        the authority as a string with percent-decoding
        applied.

        @par Example
        @code
        assert( parse_authority( "" ).value().host() == "" );

        assert( parse_authority( "example.com" ).value().host() == "example.com" );

        assert( parse_authority( "192.168.0.1" ).value().host() == "192.168.0.1" );
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
            @ref encoded_host_and_port
            @ref has_port,
            @ref host_type,
            @ref port,
            @ref port_number.
    */
    decode_view
    host() const noexcept
    {
        decode_opts opt;
        opt.plus_to_space = false;
        return detail::access::construct(
            encoded_host(), u_.decoded_[id_host], opt);
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
        authority_view u( "127.0.0.1" );

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

        @par Example
        @code
        authority_view a( "[::1]" );

        ipv6_address ip = a.ipv6_address();

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

        @par Example
        @code
        authority_view a( "[v1fe.d:9]" );

        assert( a.ipvfuture() == "v1fe.d:9" );
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
    */
    BOOST_URL_DECL
    string_view
    ipvfuture() const noexcept;

    //--------------------------------------------
    //
    // Port
    //
    //--------------------------------------------

    /** Return true if the URL contains a port

        This function returns true if the
        authority contains a port.

        @par Example
        @code
        authority_view a( "www.example.com:443" );

        assert( a.has_port() );
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

        @par Example
        @code
        authority_view a( "localhost.com:8080" );

        assert( a.port() == "8080" );
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
            @ref encoded_host_and_port
            @ref has_port,
            @ref host,
            @ref host_type,
            @ref port_number.
    */
    BOOST_URL_DECL
    string_view
    port() const noexcept;

    /** Return the port as an intege

        This function returns the port as an
        integer if the authority specifies
        a port and the number can be represented.
        Otherwise it returns zero.

        @par Example
        @code
        authority_view a( "localhost.com:8080" );

        assert( a.port_number() == 8080 );
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

        @par Example
        @code
        authority_view a( "www.example.com:8080" );

        assert( a.encoded_host_and_port() == "www.example.com:8080" );
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
            @ref encoded_host_and_port
            @ref has_port,
            @ref host,
            @ref host_type,
            @ref port,
    */
    BOOST_URL_DECL
    string_view
    encoded_host_and_port() const noexcept;

    // hidden friend
    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        authority_view const& a)
    {
        return os << a.string();
    }
};

/** Format the encoded authority to the output stream

    This function serializes the encoded URL
    to the output stream.

    @par Example
    @code
    authority_view a( "www.example.com" );

    std::cout << a << std::endl;
    @endcode

    @return A reference to the output stream, for chaining

    @param os The output stream to write to

    @param a The URL to write
*/
std::ostream&
operator<<(
    std::ostream& os,
    authority_view const& a);

//------------------------------------------------

/** Parse an authority

    This function parses a string according to
    the authority grammar below, and returns an
    @ref authority_view referencing the string.
    Ownership of the string is not transferred;
    the caller is responsible for ensuring that
    the lifetime of the string extends until the
    view is no longer being accessed.

    @par BNF
    @code
    authority     = [ userinfo "@" ] host [ ":" port ]

    userinfo      = user [ ":" [ password ] ]

    user          = *( unreserved / pct-encoded / sub-delims )
    password      = *( unreserved / pct-encoded / sub-delims / ":" )

    host          = IP-literal / IPv4address / reg-name

    port          = *DIGIT
    @endcode

    @par Exception Safety
    Throws nothing.

    @return A view to the parsed authority

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2"
        >3.2. Authority (rfc3986)</a>

    @see
        @ref authority_view.
*/
BOOST_URL_DECL
result<authority_view>
parse_authority(
    string_view s) noexcept;

//------------------------------------------------

} // urls
} // boost

#endif
