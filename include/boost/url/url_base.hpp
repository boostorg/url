//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_URL_BASE_HPP
#define BOOST_URL_URL_BASE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/ipv4_address.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/params_view.hpp>
#include <boost/url/params_encoded_view.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/segments.hpp>
#include <boost/url/segments_encoded.hpp>
#include <boost/url/url_view_base.hpp>
#include <boost/url/detail/any_params_iter.hpp>
#include <boost/url/detail/any_path_iter.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
namespace detail {
struct any_path_iter;
struct params_iter_impl;
}
namespace grammar {
class lut_chars;
}
#endif

/** Common functions for all modifiable URLs

    This base class is used by the library to
    provide all of the member functions of
    modifiable URL containers. Users should
    not use this class directly. Instead,
    construct an instance of one of the
    containers or call a parsing function:

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
    url_base
    : public url_view_base
{
    char* s_ = nullptr;
    std::size_t cap_ = 0;

    friend class url;
    friend class static_url_base;
    friend class urls::segments;
    friend class urls::params_view;
    friend class segments_encoded;
    friend class params_encoded_view;

    url_base() noexcept = default;
    url_base(detail::url_impl const&) noexcept;
    explicit url_base(string_view);
    BOOST_URL_DECL void copy(url_view_base const&);
    BOOST_URL_DECL virtual void clear_impl() noexcept = 0;
    BOOST_URL_DECL virtual void reserve_impl(std::size_t) = 0;

public:
    //--------------------------------------------

    /** Return the encoded URL as a null-terminated string
    */
    char const*
    c_str() const noexcept
    {
        return u_.cs_;
    }

    /** Return the number of characters that can be stored without reallocating

        This does not include the null terminator,
        which is always present.
    */
    std::size_t
    capacity() const noexcept
    {
        return cap_;
    }

    /** Clear the contents while preserving the capacity
    
        @par Exception Safety
        No-throw guarantee.
    */
    void
    clear() noexcept
    {
        this->clear_impl();
    }

    /** Adjust the capacity without changing the size

        This function adjusts the capacity
        of the container in characters, without
        affecting the current contents.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @throw bad_alloc Allocation failure

        @param n The capacity in characters,
        excluding any null terminator.
    */
    void
    reserve(std::size_t n)
    {
        this->reserve_impl(n);
    }

    //--------------------------------------------
    //
    // Scheme
    //
    //--------------------------------------------

private:
    void set_scheme_impl(string_view, urls::scheme);
public:

    /** Remove the scheme

        This function removes the scheme if it
        is present.

        @par BNF
        @code
        URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>

        @see @ref set_scheme.
    */
    BOOST_URL_DECL
    url_base&
    remove_scheme() noexcept;

    /** Set the scheme

        This function sets the scheme to the specified
        string, which must contain a valid scheme
        without a trailing colon (':'). If the scheme
        is invalid, an exception is thrown.

        @par Example
        @code
        url u = parse_uri( "http://www.example.com" );

        u.set_scheme( "https" );         // u is now "https://www.example.com"

        assert( u.string() == "https://www.example.com" );

        u.set_scheme( "1forall");       // throws, invalid scheme
        @endcode

        @par BNF
        @code
        scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The scheme to set.

        @throw std::invalid_argument invalid scheme.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>

        @see
            @ref remove_scheme.
    */
    BOOST_URL_DECL
    url_base&
    set_scheme(string_view s);

    /** Set the scheme

        This function sets the scheme to the specified
        known @ref urls::scheme id, which may not be
        @ref scheme::unknown or else an exception is
        thrown. If the id is @ref scheme::none, this
        function behaves as if @ref remove_scheme
        were called.

        @par Example
        @code
        url u;
        u.set_scheme( scheme::http );           // produces "http:"
        u.set_scheme( scheme::none );           // produces ""
        u.set_scheme( scheme::unknown);         // throws, invalid scheme
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param id The scheme to set.

        @throw std::invalid_argument invalid scheme.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>
    */
    BOOST_URL_DECL
    url_base&
    set_scheme(urls::scheme id);

    //--------------------------------------------
    //
    // Authority
    //
    //--------------------------------------------

private:
    char* set_user_impl(std::size_t n);
public:

    /** Set the user.

        The user is set to the specified string,
        replacing any previous user. If a userinfo
        was not present it is added, even if the
        user string is empty. The resulting URL
        will have an authority if it did not have
        one previously.

        Any special or reserved characters in the
        string are automatically percent-encoded.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1">
            3.2.1. User Information (rfc3986)</a>
    */
    BOOST_URL_DECL
    url_base&
    set_user(
        string_view s);

    /// @copydoc set_user()
    BOOST_URL_DECL
    url_base&
    set_user(
        decode_view s);

    /** Set the user.

        The user is set to the specified string,
        replacing any previous user. If a userinfo
        was not present it is added, even if the
        user string is empty. The resulting URL
        will have an authority if it did not have
        one previously.

        The string must be a valid percent-encoded
        string for the user field, otherwise an
        exception is thrown.

        @par BNF
        @code
        user          = *( unreserved / pct-encoded / sub-delims )
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_user(
        string_view s);

private:
    char* set_password_impl(std::size_t n);
public:

    /** Remove the password
    */
    BOOST_URL_DECL
    url_base&
    remove_password() noexcept;

    /** Set the password.

        This function sets the password to the specified
        string, replacing any previous password:

        @li If the string is empty, the password is
        cleared, and the first occurring colon (':') is
        removed from the userinfo if present, otherwise

        @li If ths string is not empty then the password
        is set to the new string.
        Any special or reserved characters in the
        string are automatically percent-encoded.
        If the URL previously did not have an authority
        (@ref has_authority returns `false`), a double
        slash ("//") is prepended to the userinfo.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.
    */
    BOOST_URL_DECL
    url_base&
    set_password(
        string_view s);

    /// @copydoc set_password()
    BOOST_URL_DECL
    url_base&
    set_password(
        decode_view s);

    /** Set the password.

        The password is set to the encoded string `s`,
        replacing any previous password:

        @li If the string is empty, the password is
        cleared, and the first occurring colon (':') is
        removed from the userinfo if present, otherwise

        @li If ths string is not empty then the password
        is set to the new string.
        If the URL previously did not have an authority
        (@ref has_authority returns `false`), a double
        slash ("//") is prepended to the userinfo.
        The string must meet the syntactic requirements
        of <em>password</em> otherwise an exception is
        thrown.

        @par ANBF
        @code
        password      = *( unreserved / pct-encoded / sub-delims / ":" )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_password(
        string_view s);

private:
    char* set_userinfo_impl(std::size_t n);
public:

    /** Remove the userinfo
    */
    BOOST_URL_DECL
    url_base&
    remove_userinfo() noexcept;

    /** Set the userinfo.

        Sets the userinfo of the URL to the given
        encoded string:

        @li If the string is empty, the userinfo is
        cleared, else

        @li If the string is not empty, then the userinfo
        is set to the given string. The user is set to
        the characters up to the first colon if any,
        while the password is set to the remaining
        characters if any.
        If the URL previously did not have an authority
        (@ref has_authority returns `false`), a double
        slash ("//") is prepended to the userinfo.
        The string must meet the syntactic requirements
        of <em>userinfo</em> otherwise an exception is
        thrown.

        @par BNF
        @code
        userinfo      = [ [ user ] [ ':' password ] ]

        user          = *( unreserved / pct-encoded / sub-delims )
        password      = *( unreserved / pct-encoded / sub-delims / ":" )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throw std::exception Thrown on error
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_userinfo(
        string_view s);

    /** Set the userinfo.

        The userinfo is set to the specified string,
        replacing any previous userinfo. If a userinfo
        was not present it is added, even if the
        userinfo string is empty. The resulting URL
        will have an authority if it did not have
        one previously.

        Any special or reserved characters in the
        string are automatically percent-encoded.

        The interpretation of userinfo as
        individual user and password components
        is scheme-dependent. Transmitting
        passwords in URLs is deprecated.

        This function treats userinfo as a unit
        independent of the user and password
        components.

        If the input string has a ":" character,
        its first occurrence will be considered
        the separator between the user and the
        password fields.

        If the username might contain a ":", the
        functions @ref set_user and
        @ref set_password should be used
        separately so that the character can be
        unambiguously encoded.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1">
            3.2.1. User Information (rfc3986)</a>
    */
    BOOST_URL_DECL
    url_base&
    set_userinfo(
        string_view s);

    /// @copydoc set_userinfo()
    BOOST_URL_DECL
    url_base&
    set_userinfo(
        decode_view s);

    //--------------------------------------------

private:
    char* set_host_impl(std::size_t n);
public:

    /** Set the host

        The host is set to the specified IPv4,
        address, replacing any previous host. If
        an authority was not present, it is added.

        @par Postconditions
        @code
        this->host_type() == host_type::ipv4 && this->ipv4_address() == addr
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2">
            3.2.2. Host (rfc3986)</a>

        @param addr The address to set.
    */
#ifdef BOOST_URL_DOCS
    url_base& set_host( ipv4_address const& addr );
#else
    BOOST_URL_DECL
    url_base&
    set_host(
        urls::ipv4_address const& addr);
#endif

    /** Set the host

        The host is set to the specified IPv6,
        address, replacing any previous host.
        If an authority did not
        previously exist it is added by prepending
        a double slash ("//") at the beginning of
        the URL or after the scheme if a scheme is
        present.

        @par Postconditions
        @code
        this->host_type() == host_type::ipv6 && this->ipv6_address() == addr
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2">
            3.2.2. Host (rfc3986)</a>

        @param addr The address to set.
    */
#ifdef BOOST_URL_DOCS
    url_base& set_host( ipv6_address const& addr );
#else
    BOOST_URL_DECL
    url_base&
    set_host(
        urls::ipv6_address const& addr);
#endif

    /** Set the host

        The host is set to the specified plain
        string, subject to the following rules:

        @li If the string is a valid IPv4 address,
        the address is parsed and the host is set
        as if an instance of the equivalent
        @ref urls::ipv4_address were passed instead.
        In this case @ref url::host_type will return
        @ref host_type::ipv4. Otherwise,

        @li The plain string is percent-encoded and
        the result is set as the reg-name for the
        host. In this case @ref url::host_type will
        return @ref host_type::name.

        In all cases, if an authority did not
        previously exist it is added by prepending
        a double slash ("//") at the beginning of
        the URL or after the scheme if a scheme is
        present.

        @par Postconditions
        @code
        this->encoded_host() == s
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2">
            3.2.2. Host (rfc3986)</a>

        @param s The string to set.
    */
    BOOST_URL_DECL
    url_base&
    set_host(
        string_view s);

    /// @copydoc set_host(string_view)
    BOOST_URL_DECL
    url_base&
    set_host(
        decode_view s);

    /** Set the host

        The host is set to the specified percent-
        encoded string, subject to the following
        rules:

        @li If the string is a valid IPv4 address,
        the address is parsed and the host is set
        as if an instance of the equivalent
        @ref urls::ipv4_address were passed instead. In
        this case @ref url::host_type will return
        @ref host_type::ipv4. Or,

        @li If the string is a valid IPv6 address
        enclosed in square brackets ('[' and ']'),
        the address is parsed and the host is set
        as if an instance of the equivalent
        @ref urls::ipv6_address were passed instead. In
        this case @ref url::host_type will return
        @ref host_type::ipv4. Or,

        @li If the string is a valid IPvFuture address
        enclosed in square brackets ('[' and ']'),
        the address is parsed and the host is set
        to the specified string. In this case
        @ref url::host_type will return
        @ref host_type::ipvfuture. Or,

        @li If the string is a valid percent-encoded
        string with no characters from the reserved
        character set, then it is set as the encoded
        host name. In this case @ref url::host_type
        will return @ref host_type::name. Otherwise,

        @li If the string does not contain a valid
        percent-encoding for the host field, an
        exception is thrown.

        In all cases, if an authority did not
        previously exist it is added by prepending
        a double slash ("//") at the beginning of
        the URL or after the scheme if a scheme is
        present.
        
        @par Postconditions
        @code
        this->encoded_host() == s
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2">
            3.2.2. Host (rfc3986)</a>

        @param s The string to set.

        @throw std::exception the percent-encoding is invalid
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_host(string_view s);

private:
    char* set_port_impl(std::size_t n);
public:

    /** Remove the port

        If a port is present, it is removed.
        The remainder of the authority component
        is left unchanged including the leading
        double slash ("//").

        @par Postconditions
        @code
        this->has_port() == false && this->port_number() == 0 && this->port() == ""
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3">
            3.2.3. Port (rfc3986)</a>
    */
    BOOST_URL_DECL
    url_base&
    remove_port() noexcept;

    /** Set the port

        The port of the URL is set to the specified
        integer, replacing any previous port.
        If an authority did not
        previously exist it is added by prepending
        a double slash ("//") at the beginning of
        the URL or after the scheme if a scheme is
        present.

        @par Postconditions
        @code
        this->has_port() == true && this->port_number() == n && this->port() == std::to_string(n)
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3">
            3.2.3. Port (rfc3986)</a>

        @param n The port number to set.
    */
    BOOST_URL_DECL
    url_base&
    set_port(std::uint16_t n);

    /** Set the port

        The port of the URL is set to the specified
        string, replacing any previous port. The string
        must meet the syntactic requirements for PORT,
        which consists only of digits. The string
        may be empty. In this case the port is still
        defined, however it is the empty string. To
        remove the port call @ref remove_port.
        If an authority did not
        previously exist it is added by prepending
        a double slash ("//") at the beginning of
        the URL or after the scheme if a scheme is
        present.

        @par Postconditions
        @code
        this->has_port() == true && this->port_number() == n && this->port() == std::to_string(n)
        @endcode

        @par BNF
        @code
        port          = *DIGIT
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3">
            3.2.3. Port (rfc3986)</a>

        @param s The port string to set.

        @throw std::exception `s` is not a valid port string.
    */
    BOOST_URL_DECL
    url_base&
    set_port(string_view s);

    //--------------------------------------------

    /** Remove the authority

        The full authority component is removed
        if present, which includes the leading
        double slashes ("//"), the userinfo,
        the host, and the port.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2">
            3.2. Authority (rfc3986)</a>
    */
    BOOST_URL_DECL
    url_base&
    remove_authority() noexcept;

    /** Set the authority

        This function sets the authority component
        to the specified encoded string. If a
        component was present it is replaced.
        Otherwise, the authority is added
        including leading double slashes ("//").

        The encoded string must be a valid
        authority or else an exception is thrown.

        @par BNF
        @code
        authority     = [ userinfo "@" ] host [ ":" port ]

        userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
        host          = IP-literal / IPv4address / reg-name
        port          = *DIGIT
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2">
            3.2. Authority (rfc3986)</a>

        @param s The authority string to set.

        @throw std::exception `s` is not a valid authority.
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_authority(
        string_view s);

    //--------------------------------------------

    /** Remove the origin component

        The origin consists of the everything from the
        beginning of the URL up to but not including
        the path.

        @par Exception Safety
        Throws nothing.
    */
    BOOST_URL_DECL
    url_base&
    remove_origin() noexcept;

    //--------------------------------------------
    //
    // Path
    //
    //--------------------------------------------

private:
    pos_t
    segment(
        std::size_t i) const noexcept;

    char*
    edit_segments(
        std::size_t i0,
        std::size_t i1,
        std::size_t n,
        std::size_t nseg);

    BOOST_URL_DECL
    void
    edit_segments(
        std::size_t i0,
        std::size_t i1,
        detail::any_path_iter&& it0,
        detail::any_path_iter&& it1,
        int abs_hint = -1);
public:

    /** Set whether the path is absolute.

        This modifies the path as needed to
        make it absolute or relative.

        @return true on success.
    */
    BOOST_URL_DECL
    bool
    set_path_absolute(bool absolute);

    /** Set the path.

        This function validates the given percent-encoded
        path according to the allowed grammar based
        on the existing contents of the URL. If the
        path is valid, the old path is replaced with
        the new path. Otherwise, an exception is
        thrown. The requirements for `s` are thus:

        @li If `s` matches <em>path-empty</em>, that is
        if `s.empty() == true`, the path is valid. Else,

        @li If an authority is present (@ref has_authority
        returns `true`), the path syntax must match
        <em>path-abempty</em>. Else, if there is no
        authority then:

        @li If the new path starts with a forward
        slash ('/'), the path syntax must match
        <em>path-absolute</em>. Else, if the
        path is rootless (does not start with '/'),
        then:

        @li If a scheme is present, the path syntax
        must match <em>path-rootless</em>, otherwise

        @li The path syntax must match <em>path-noscheme</em>.

        @par BNF
        @code
        path-abempty  = *( "/" segment )
        path-absolute = "/" [ segment-nz *( "/" segment ) ]
        path-noscheme = segment-nz-nc *( "/" segment )
        path-rootless = segment-nz *( "/" segment )
        path-empty    = 0<pchar>
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::invalid_argument invalid path.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
            >3.3. Path (rfc3986)</a>
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_path(
        string_view s);

    /** Set the path.

        Sets the path of the URL to the specified
        plain string. Any reserved characters in the
        string will be automatically percent-encoded.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
            >3.3. Path (rfc3986)</a>

        @see
            @ref encoded_path,
            @ref encoded_segments,
            @ref is_path_absolute,
            @ref normalize_path,
            @ref set_encoded_path,
            @ref set_path_absolute.
    */
    BOOST_URL_DECL
    url_base&
    set_path(
        string_view s);

    /// @copydoc set_path()
    BOOST_URL_DECL
    url_base&
    set_path(
        decode_view s);

    /** Return the path segments

        This function returns the path segments as
        a bidirectional range.

        @par BNF
        @code
        path          = [ "/" ] segment *( "/" segment )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
            >3.3. Path (rfc3986)</a>

        @see
            @ref urls::segments_encoded,
            @ref segments.

    */
    BOOST_URL_DECL
    segments_encoded
    encoded_segments() noexcept;

    /** Return the path segments

        This function returns the path segments as
        a bidirectional range.

        @par BNF
        @code
        path          = [ "/" ] segment *( "/" segment )
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
            >3.3. Path (rfc3986)</a>

        @see
            @ref urls::segments,
            @ref encoded_segments.
    */
    urls::segments
    segments() noexcept
    {
        return {*this};
    }

    //--------------------------------------------
    //
    // Query
    //
    //--------------------------------------------

    /** Remove the query.

        If a query is present (@ref has_query
        returns `true`), then the query is
        removed including the leading `?`.

        @par Exception Safety
        Throws nothing.

        @see
            @ref has_query,
            @ref set_encoded_query,
            @ref set_query.
    */
    BOOST_URL_DECL
    url_base&
    remove_query() noexcept;

    /** Set the query.

        Sets the query of the URL to the specified
        encoded string. The string must contain a
        valid percent-encoding or else an exception
        is thrown. When this function returns,
        @ref has_query will return `true`.

        @par BNF
        @code
        query           = *( pchar / "/" / "?" )
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::invalid_argument bad encoding.

        @see
            @ref has_query,
            @ref remove_query,
            @ref set_query.
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_query(
        string_view s);

    /** Set the query.

        Sets the query of the URL to the specified
        plain string. Any reserved characters in the
        string will be automatically percent-encoded.
        When this function returns, @ref has_query
        will return `true`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.

        @see
            @ref has_query,
            @ref remove_query,
            @ref set_encoded_query.
    */
    BOOST_URL_DECL
    url_base&
    set_query(
        string_view s);

    /// @copydoc set_query()
    BOOST_URL_DECL
    url_base&
    set_query(
        decode_view s);

    /** Return the query parameters

        This function returns the query
        parameters as a forward range of
        key/value pairs.

        Each string returned by the container
        is percent-encoded.

        @par BNF
        @code
        query-params_view    = [ query-param ] *( "&" [ query-param ] )

        query-param     = key [ "=" value ]

        @endcode

        @see
            @ref urls::params_encoded_view,
            @ref params_view.
    */
    urls::params_encoded_view
    encoded_params() noexcept
    {
        return urls::params_encoded_view(*this);
    }

    /** Return the query parameters

        This function returns the query
        parameters as a forward range of
        key/value pairs where each returned
        string has percent-decoding applied.

        @par BNF
        @code
        query-params_view    = [ query-param ] *( "&" [ query-param ] )

        query-param     = key [ "=" value ]
        @endcode

        @see
            @ref urls::params_view,
            @ref encoded_params.

    */
    urls::params_view
    params() noexcept
    {
        return urls::params_view(*this);
    }

    //--------------------------------------------
    //
    // Fragment
    //
    //--------------------------------------------

private:
    char* set_fragment_impl(std::size_t n);
public:

    /** Remove the fragment.

        If a fragment is present (@ref has_fragment
        returns `true`), then the fragment is
        removed including the leading `#`.

        @par Exception Safety
        Throws nothing.

        @see
            @ref has_fragment,
            @ref set_encoded_fragment,
            @ref set_fragment.
    */
    BOOST_URL_DECL
    url_base&
    remove_fragment() noexcept;

    /** Set the fragment.

        Sets the fragment of the URL to the specified
        encoded string. The string must contain a
        valid percent-encoding or else an exception
        is thrown. When this function returns,
        @ref has_fragment will return `true`.

        @par BNF
        @code
        fragment      = *( pchar / "/" / "?" )
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::exception bad encoding.

        @see
            @ref has_fragment,
            @ref remove_fragment,
            @ref set_fragment.
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_fragment(
        string_view s);

    /** Set the fragment.

        Sets the fragment of the URL to the specified
        plain string. Any reserved characters in the
        string will be automatically percent-encoded.
        When this function returns, @ref has_fragment
        will return `true`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.

        @see
            @ref has_fragment,
            @ref remove_fragment,
            @ref set_encoded_fragment.
    */
    BOOST_URL_DECL
    url_base&
    set_fragment(
        string_view s);

    /// @copydoc set_fragment()
    BOOST_URL_DECL
    url_base&
    set_fragment(
        decode_view s);

    //--------------------------------------------
    //
    // Normalization
    //
    //--------------------------------------------
private:
    void
    normalize_octets_impl(
        int id,
        grammar::lut_chars const& cs) noexcept;

    void
    decoded_to_lower_impl(int id) noexcept;

    void
    to_lower_impl(int id) noexcept;
public:

    /** Normalize the URL components

        Applies Syntax-based normalization to
        all components of the URL.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2"
            >6.2.2 Syntax-Based Normalization (rfc3986)</a>

    */
    BOOST_URL_DECL
    url_base&
    normalize();

    /** Normalize the URL scheme

        Applies Syntax-based normalization to the
        URL scheme.

        The scheme is normalized to lowercase.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2"
            >6.2.2 Syntax-Based Normalization (rfc3986)</a>

    */
    BOOST_URL_DECL
    url_base&
    normalize_scheme();

    /** Normalize the URL authority

        Applies Syntax-based normalization to the
        URL authority.

        Percent-encoding triplets are normalized
        to uppercase letters. Percent-encoded
        octets that correspond to unreserved
        characters are decoded.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2"
            >6.2.2 Syntax-Based Normalization (rfc3986)</a>

    */
    BOOST_URL_DECL
    url_base&
    normalize_authority();

    /** Normalize the URL path

        Applies Syntax-based normalization to the
        URL path.

        Percent-encoding triplets are normalized
        to uppercase letters. Percent-encoded
        octets that correspond to unreserved
        characters are decoded. Redundant
        path-segments are removed.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2"
            >6.2.2 Syntax-Based Normalization (rfc3986)</a>

    */
    BOOST_URL_DECL
    url_base&
    normalize_path();

    /** Normalize the URL query

        Applies Syntax-based normalization to the
        URL query.

        Percent-encoding triplets are normalized
        to uppercase letters. Percent-encoded
        octets that correspond to unreserved
        characters are decoded.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2"
            >6.2.2 Syntax-Based Normalization (rfc3986)</a>

    */
    BOOST_URL_DECL
    url_base&
    normalize_query();

    /** Normalize the URL fragment

        Applies Syntax-based normalization to the
        URL fragment.

        Percent-encoding triplets are normalized
        to uppercase letters. Percent-encoded
        octets that correspond to unreserved
        characters are decoded.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-6.2.2"
            >6.2.2 Syntax-Based Normalization (rfc3986)</a>

    */
    BOOST_URL_DECL
    url_base&
    normalize_fragment();

    //--------------------------------------------
    //
    // Resolution
    //
    //--------------------------------------------

    friend
    result<void>
    resolve(
        url_view_base const& base,
        url_view_base const& ref,
        url_base& dest);

private:
    //--------------------------------------------
    //
    // implementation
    //
    //--------------------------------------------

    void check_invariants() const noexcept;
    char* resize_impl(int, std::size_t);
    char* resize_impl(int, int, std::size_t);
    char* shrink_impl(int, std::size_t);
    char* shrink_impl(int, int, std::size_t);

    char*
    resize_params(
        detail::params_iter_impl const&,
        detail::params_iter_impl const&,
        std::size_t, std::size_t);

    BOOST_URL_DECL
    auto
    edit_params(
        detail::params_iter_impl const&,
        detail::params_iter_impl const&,
        detail::any_params_iter&&) ->
            detail::params_iter_impl;

    BOOST_URL_DECL
    result<void>
    resolve_impl(
        url_view_base const& base,
        url_view_base const& ref);
};

} // urls
} // boost

// These are here because of circular references
#include <boost/url/impl/params_encoded_view.hpp>
#include <boost/url/impl/params_view.hpp>
#include <boost/url/impl/segments.hpp>
#include <boost/url/impl/segments_encoded.hpp>

#endif
