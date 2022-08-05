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
#include <boost/url/pct_encoding.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/url_impl.hpp>
#include <boost/assert.hpp>
#include <cstddef>
#include <iosfwd>
#include <utility>

namespace boost {
namespace urls {

/** A read-only view to an authority.

    Objects of this type represent a valid
    authority whose storage is managed externally.
    That is, it acts like a `std::string_view` in
    terms of ownership. Callers are responsible
    for ensuring that the lifetime of the
    underlying string extends until the
    `authority_view` is no longer in use.
    To construct from an existing string it is
    necessary to use one of the parsing
    functions. Each function parses against
    a particular URL grammar:

    @par Example
    @code
    authority_view a;

    a = parse_authority( "www.example.com:443" );
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
    : protected detail::parts_base
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
    /** The type of elements.
    */
    using value_type        = char;

    /** The type of pointers to elements.
    */
    using pointer           = char const*;

    /** The type of const pointers to elements.
    */
    using const_pointer     = char const*;

    /** The type of reference to elements.
    */
    using reference         = char const&;

    /** The type of const references to elements.
    */
    using const_reference   = char const&;

    /** The type of const iterator to elements.
    */
    using const_iterator    = char const*;

    /** The type of iterator to elements.
    */
    using iterator          = char const*;

    /** An unsigned integer type to represent sizes.
    */
    using size_type         = std::size_t;

    /** A signed integer type to represent differences.
    */
    using difference_type   = std::ptrdiff_t;

    //--------------------------------------------
    //
    // Special Members
    //
    //--------------------------------------------

    /** Destructor
    */
    BOOST_URL_DECL
    virtual ~authority_view();

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

    BOOST_URL_DECL
    authority_view(
        authority_view const&) noexcept;

    BOOST_URL_DECL
    authority_view&
    operator=(
        authority_view const&) noexcept;

    //--------------------------------------------
    //
    // Capacity
    // Element Access
    //
    //--------------------------------------------

    /** Return the maximum number of characters allowed in the authority.

        This includes any null terminator, if present

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

    /** Return the number of characters in the authority.

        This function returns the number
        of characters in the authority, not
        including any null terminator.

        @par Exception Safety
        Throws nothing.
    */
    std::size_t
    size() const noexcept
    {
        return u_.offset(id_end);
    }

    /** Return true if the contents are empty.

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

    /** Access the specified character

        This function returns a reference to
        the character at the specified zero-based
        position. If `pos` is out of range, an
        exception if thrown.

        @param pos The zero-based character
        position to access.

        @throw std::out_of_range pos >= size()
    */
    char const&
    at(std::size_t pos) const
    {
        if(pos >= size())
            detail::throw_out_of_range(
                BOOST_CURRENT_LOCATION);
        return u_.cs_[pos];
    }

    /** Access the specified character

        This function returns a reference to
        the character at the specified zero-based
        position. The behavior is undefined if
        `pos` is out of range.

        @par Preconditions
        @code
        pos < this->size()
        @endcode
    */
    char const&
    operator[](
        std::size_t pos) const noexcept
    {
        BOOST_ASSERT(pos < size());
        return u_.cs_[pos];
    }

    /** Return an iterator to the beginning

        This function returns a constant iterator
        to the first character of the view, or
        one past the last element if the view is
        empty.
    */
    char const*
    begin() const noexcept
    {
        return data();
    }

    /** Return an iterator to the end

        This function returns a constant iterator to
        the character following the last character of
        the view. This character acts as a placeholder,
        attempting to access it results in undefined
        behavior.
    */
    char const*
    end() const noexcept
    {
        return data() + size();
    }

    //--------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------

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
    encoded_authority() const noexcept
    {
        return string_view(
            data(), size());
    }

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

    /** Return the userinfo

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

        @par Specification
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
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
        return detail::access::construct(
            encoded_userinfo(),
            u_.decoded_[id_user] +
                has_password() + u_.decoded_[id_pass],
            opt);
    }

    //--------------------------------------------

    /** Return the user

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

        @par Specification
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
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

    /** Return the password

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
        return detail::access::construct(
            encoded_password(), u_.decoded_[id_pass], opt);
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

    /** Return the host

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
    pct_encoded_view
    host() const noexcept
    {
        pct_decode_opts opt;
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

    /** Return the port as an intege

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

    // hidden friend
    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        authority_view const& a)
    {
        return os << a.encoded_authority();
    }
};

/** Format the encoded authority to the output stream

    This function serializes the encoded URL
    to the output stream.

    @par Example
    @code
    url_view u = parse_uri( "http://www.example.com/index.htm" );

    std::cout << u << std::endl;
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
