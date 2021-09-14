//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_URL_HPP
#define BOOST_URL_URL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/ipv4_address.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/path_view.hpp>
#include <boost/url/query_params_view.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/detail/parts.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view;
#endif

/** A modifiable container for a URL.

    Objects of this type hold URLs which may be
    inspected and modified. The derived class
    is responsible for providing storage.

    The underlying string stored in the container
    is always null-terminated.

    @par Exception Safety

    @li Functions marked `noexcept` provide the
    no-throw guarantee, otherwise:

    @li Functions which throw offer the strong
    exception safety guarantee.

    @par Specification
    @li <a href="https://tools.ietf.org/html/rfc3986">
        Uniform Resource Identifier (URI): Generic Syntax (rfc3986)</a>
*/
class BOOST_SYMBOL_VISIBLE
    url : public url_view
{
#ifndef BOOST_URL_DOCS
protected:
#endif
    
    char* s_ = nullptr;
    std::size_t cap_ = 0;

    // VFALCO This has to be kept in
    // sync with other declarations
    enum
    {
        id_scheme = 0,  // trailing ':'
        id_user,        // leading "//"
        id_pass,        // leading ':', trailing '@'
        id_host,
        id_port,        // leading ':'
        id_path,
        id_query,       // leading '?'
        id_frag,        // leading '#'
        id_end          // one past the end
    };

    friend class static_url_base;

    void check_invariants() const noexcept;

    BOOST_URL_DECL url(
        char* buf, std::size_t cap) noexcept;
    BOOST_URL_DECL void copy(
        char const* s, detail::parts const& pt);

    BOOST_URL_DECL virtual char* alloc_impl(
        std::size_t new_cap);
    BOOST_URL_DECL virtual void free_impl(char* s);
    BOOST_URL_DECL virtual std::size_t growth_impl(
        std::size_t cap, std::size_t new_size);

public:
    class params_type;
    class segments_type;

    /** Destructor
    */
    BOOST_URL_DECL
    virtual
    ~url();

    /** Constructor
    */
    BOOST_URL_DECL
    url() noexcept;

    /** Constructor
    */
    BOOST_URL_DECL
    url(url&& u) noexcept;

    /** Constructor
    */
    BOOST_URL_DECL
    url(url const& u);

    /** Constructor
    */
    BOOST_URL_DECL
    url(url_view const& u);

    /** Assignment
    */
    BOOST_URL_DECL
    url&
    operator=(url&& u) noexcept;

    /** Assignment
    */
    BOOST_URL_DECL
    url&
    operator=(url const& u);

    /** Assignment
    */
    BOOST_URL_DECL
    url&
    operator=(url_view const& u);

    //--------------------------------------------

    /** Return the complete serialized URL
    */
    BOOST_URL_DECL
    string_view
    str() const noexcept;

    /** Return the encoded URL as a null-terminated string
    */
    BOOST_URL_DECL
    char const*
    c_str() const noexcept;

    /** Return the number of characters that may be stored without a reallocation.

        This function returns the maximum number of
        characters, excluding the null terminator,
        which may be stored in the container before
        a reallocation is necessary.
    */
    std::size_t
    capacity() const noexcept
    {
        return cap_;
    }

    /** Clear the contents.
    
        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_URL_DECL
    void
    clear() noexcept;

    /** Adjust the capacity
    */
    BOOST_URL_DECL
    void
    reserve(std::size_t n);

    //--------------------------------------------
    //
    // scheme
    //
    //--------------------------------------------

private:
    void set_scheme_impl(string_view s, urls::scheme id);
public:

    /** Remove the scheme

        This function removes the scheme if it
        is present.

        @par BNF
        @code
        URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
        @endcode

        @par Exception Safety
        Does not throw.

        @return A reference to the object, for chaining.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>

        @see @ref set_scheme.
    */
    BOOST_URL_DECL
    url&
    remove_scheme() noexcept;

    /** Set the scheme

        This function sets the scheme to the specified
        string, which must contain a valid scheme
        without a trailing colon (':'). If the scheme
        is invalid, an exception is thrown.

        @par Example
        @code
        url u;
        u.set_scheme( "http" );         // produces "http:"
        u.set_scheme( "1forall");       // throws, invalid scheme
        @endcode

        @par BNF
        @code
        scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        @endcode

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @return A reference to the object, for chaining.

        @param s The scheme to set.

        @throw std::invalid_argument invalid scheme.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>
    */
    BOOST_URL_DECL
    url&
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

        @return A reference to the object, for chaining.

        @param id The scheme to set.

        @throw std::invalid_argument invalid scheme.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>
    */
    BOOST_URL_DECL
    url&
    set_scheme(urls::scheme id);

    //--------------------------------------------
    //
    // authority
    //
    //--------------------------------------------

private:
    char* set_user_impl(std::size_t n);
public:

    /** Remove the user

        If a user is present, it is removed. If the
        user was the only component present in the
        userinfo, then the userinfo is removed without
        removing the authority.

        @par Exception Safety
        Does not throw.

        @return A reference to the object, for chaining.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1">
            3.2.1. User Information (rfc3986)</a>
    */
    BOOST_URL_DECL
    url&
    remove_user() noexcept;

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

        @return A reference to the object, for chaining.

        @param s The string to set. This string may
        contain any characters, including nulls.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1">
            3.2.1. User Information (rfc3986)</a>
    */
    BOOST_URL_DECL
    url&
    set_user(
        string_view s);

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

        @return A reference to the object, for chaining.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url&
    set_encoded_user(
        string_view s);

private:
    char* set_password_impl(std::size_t n);
public:

    /** Remove the password
    */
    BOOST_URL_DECL
    url&
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
    url&
    set_password(
        string_view s);

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
    url&
    set_encoded_password(
        string_view s);

private:
    char* set_userinfo_impl(std::size_t n);
public:

    /** Remove the userinfo
    */
    BOOST_URL_DECL
    url&
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
    */
    BOOST_URL_DECL
    url&
    set_encoded_userinfo(
        string_view s);

    BOOST_URL_DECL
    url&
    set_userinfo(
        string_view s);

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

        @return A reference to the object, for chaining.

        @param addr The address to set.
    */
#ifdef BOOST_URL_DOCS
    url& set_host( ipv4_address const& addr );
#else
    BOOST_URL_DECL
    url&
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

        @return A reference to the object, for chaining.

        @param addr The address to set.
    */
#ifdef BOOST_URL_DOCS
    url& set_host( urls::ipv6_address const& addr );
#else
    BOOST_URL_DECL
    url&
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

        @return A reference to the object, for chaining.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url&
    set_host(
        string_view s);

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

        @return A reference to the object, for chaining.

        @param s The string to set.

        @throw std::invalid_argument the percent-encoding is invalid
    */
    BOOST_URL_DECL
    url&
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
        Does not throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.3">
            3.2.3. Port (rfc3986)</a>

        @return A reference to the object, for chaining.
    */
    BOOST_URL_DECL
    url&
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

        @return A reference to the object, for chaining.
    */
    BOOST_URL_DECL
    url&
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

        @return A reference to the object, for chaining.

        @throw std::invalid_argument `s` is not a valid port string.
    */
    BOOST_URL_DECL
    url&
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

        @return A reference to the object, for chaining.
    */
    BOOST_URL_DECL
    url&
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

        @return A reference to the object, for chaining.

        @throw std::invalid_argument `s` is not a valid authority.
    */
    BOOST_URL_DECL
    url&
    set_encoded_authority(
        string_view s);

    //--------------------------------------------

    /** Remove the origin component

        The origin consists of the everything from the
        beginning of the URL up to but not including
        the path.

        @par Exception Safety
        Does not throw.
    */
    BOOST_URL_DECL
    url&
    remove_origin() noexcept;

private:
    //--------------------------------------------
    //
    // path
    //
    //--------------------------------------------

    /** Set the path.

        Sets the path of the URL to the specified
        encoded string. If this string is empty,
        any existing path is removed.

        The string must meet the syntactic requirements,
        which vary depending on the existing contents
        of the URL:

        @li If an authority is present (@ref has_authority
        returns `true`), the path syntax must match
        <em>path-abempty</em>, else

        @li If the new path starts with a forward
        slash ('/'), the path syntax must match
        <em>path-absolute</em>, else

        @li If a scheme is present, the path syntax
        must match <em>path-rootless</em>, otherwise

        @li The path syntax must match <em>path-noscheme</em>.

        If the path does not meet the syntactic
        requirements, an exception is thrown.

        @par BNF
        @code
        path          = path-abempty    ; begins with "/" or is empty
                      / path-absolute   ; begins with "/" but not "//"
                      / path-noscheme   ; begins with a non-colon segment
                      / path-rootless   ; begins with a segment
                      / path-empty      ; zero characters

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

        @throws std::exception invalid path.

    */
    BOOST_URL_DECL
    url&
    set_encoded_path(
        string_view s);

    /** Return the path.

        This function returns the path segments
        as a lightweight, non-owning reference to
        the existing data, with the interface of
        a modifiable container.

        @par Exception Safety

        No-throw guarantee.

        @see segments_type
    */
    BOOST_URL_DECL
    segments_type
    path() noexcept;

    //--------------------------------------------
    //
    // query
    //
    //--------------------------------------------

    /** Set the query.

        Sets the query of the URL to the specified
        plain string:

        @li If the string is empty, the query is
        cleared including the leading question
        mark ('?'), otherwise:

        @li If the string is not empty, the query
        is set to the given string, with a leading
        question mark added.
        Any special or reserved characters in the
        string are automatically percent-encoded.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.
    */
    BOOST_URL_DECL
    url&
    set_query(
        string_view s);

    /** Set the query.

        Sets the query of the URL to the specified
        encoded string:

        @li If the string is empty, the query is
        cleared including the leading question mark ('?'),
        otherwise:

        @li If the string is not empty, the fragment
        is set to given string, with a leading question
        mark added.
        The string must meet the syntactic requirements
        of <em>query</em> otherwise an exception is
        thrown.

        @par BNF
        @code
        query         = *( pchar / "/" / "?" )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::exception invalid string.
    */
    BOOST_URL_DECL
    url&
    set_encoded_query(
        string_view s);

    /** Set the query.

        Sets the query of the URL to the specified
        encoded string.

        @li If the string is empty, the query is
        cleared including the leading question
        mark ('?'), otherwise:

        @li If the string is not empty, the query
        is set to given string.
        The string must meet the syntactic requirements
        of <em>query-part</em> otherwise an exception
        is thrown.

        @par BNF
        @code
        query-part    = [ "#" *( pchar / "/" / "?" ) ]
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::exception invalid string.
    */
    BOOST_URL_DECL
    url&
    set_query_part(
        string_view s);

    /** Return the query.

        This function returns the query parameters
        as a lightweight, non-owning reference to
        the existing data, with the interface of
        a modifiable associative container.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    params_type
    query_params() noexcept;

    //--------------------------------------------
    //
    // fragment
    //
    //--------------------------------------------

    /** Set the fragment.

        Sets the fragment of the URL to the specified
        plain string:

        @li If the string is empty, the fragment is
        cleared including the leading hash mark ('#'),
        otherwise:

        @li If the string is not empty, the fragment
        is set to given string, with a leading hash
        mark added.
        Any special or reserved characters in the
        string are automatically percent-encoded.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.

        @see set_encoded_fragment, set_fragment_part
    */
    BOOST_URL_DECL
    url&
    set_fragment(
        string_view s);

    /** Set the fragment.

        Sets the fragment of the URL to the specified
        encoded string:

        @li If the string is empty, the fragment is
        cleared including the leading hash mark ('#'),
        otherwise:

        @li If the string is not empty, the fragment
        is set to given string, with a leading hash
        mark added.
        The string must meet the syntactic requirements
        of <em>fragment</em> otherwise an exception is
        thrown.

        @par BNF
        @code
        fragment      = *( pchar / "/" / "?" )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::exception invalid string.

        @see set_fragment, set_fragment_part
    */
    BOOST_URL_DECL
    url&
    set_encoded_fragment(
        string_view s);

    /** Set the fragment.

        Sets the fragment of the URL to the specified
        encoded string.

        @li If the string is empty, the fragment is
        cleared including the leading hash mark ('#'),
        otherwise:

        @li If the string is not empty, the fragment
        is set to given string.
        The string must meet the syntactic requirements
        of <em>fragment-part</em> otherwise an exception
        is thrown.

        @par BNF
        @code
        fragment-part = [ "#" *( pchar / "/" / "?" ) ]
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.

        @throws std::exception invalid string.

        @see set_fragment, set_encoded_fragment
    */
    BOOST_URL_DECL
    url&
    set_fragment_part(
        string_view s);

    //--------------------------------------------
    //
    // normalization
    //
    //--------------------------------------------

    /** Normalize everything.
    */
    BOOST_URL_DECL
    url&
    normalize();

    BOOST_URL_DECL
    url&
    normalize_scheme() noexcept;

private:
    inline void resize_impl(
        std::size_t new_size);
    inline char* resize_impl(int id,
        std::size_t new_size);
    inline char* resize_impl(
        int first, int last,
        std::size_t new_size);
};

//----------------------------------------------------------

/** A read/write view to the path segments.
*/
class url::segments_type
{
    url* v_;

public:
    class value_type;
    class iterator;
    using const_iterator = iterator;

private:
    BOOST_URL_DECL
    iterator
    insert_encoded_impl(
        iterator pos, string_view s );

    BOOST_URL_DECL
    iterator
    insert_impl(
        iterator pos, string_view s, std::size_t ns );

public:
    segments_type() = delete;

    segments_type(segments_type const&) = default;
    segments_type& operator=(
        segments_type const&) = default;

    explicit
    segments_type(url& v)
        : v_(&v)
    {
    }

    bool
    empty() const noexcept
    {
        return size() == 0;
    }

    std::size_t
    size() const noexcept
    {
        return (v_ == nullptr) ? 0 :
            v_->pt_.nseg;
    }

    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    BOOST_URL_DECL
    iterator
    end() const noexcept;

    /** Erase the specified sequence of path segments.

        @par Exception Safety

        No-throw guarantee.

        @param first The first segment to be erased.

        @param last One past the last segment to be erased.
    */
    BOOST_URL_DECL
    iterator
    erase( iterator first, iterator last ) noexcept;

    /** Erase the specified path segment

        @param pos The path segment to erase

        This function is equivalent to:
        @code
        auto last = pos;
        ++last;
        erase(pos, last);
        @endcode
    */
    BOOST_URL_DECL
    iterator
    erase( iterator pos ) noexcept;

    /** Insert an encoded path segment at the specified position.

        @par Exception Safety

        Strong guarantee. Calls to allocate may throw.

        @param pos The new path segment is inserted before this position.

        @param s The encoded path segment to be inserted.
    */
    BOOST_URL_DECL
    iterator
    insert_encoded( iterator pos, string_view s );

    /** Encode an unencoded path segment and insert it at the specified position.

        @par Exception Safety

        Strong guarantee. Calls to allocate may throw.

        @param pos The new path segment is inserted before this position.

        @param s The unencoded path segment to be inserted.
    */
    BOOST_URL_DECL
    iterator
    insert( iterator pos, string_view s );

    /** Replace the path segment at the specified position with the specified encoded path segment.

        @par Exception Safety

        Strong guarantee. Calls to allocate may throw.

        @param pos The path segment to be replaced.

        @param s The encoded to replace the segment at pos.
    */
    BOOST_URL_DECL
    iterator
    replace_encoded( iterator pos, string_view s );

    /** Replace the path segment at the specified position with the specified unencoded path segment.

        @par Exception Safety

        Strong guarantee. Calls to allocate may throw.

        @param pos The path segment to be replaced.

        @param s The unencoded segment to replace the segment at pos.
    */
    BOOST_URL_DECL
    iterator
    replace( iterator pos, string_view s );
};

//----------------------------------------------------------

class url::segments_type::value_type
{
    string_view s_;

    friend class segments_type;

    explicit
    value_type(
        string_view s) noexcept
        : s_(s)
    {
    }

public:
    value_type() = delete;
    value_type& operator=(
        value_type const&) = delete;

    value_type(
        value_type const&) = default;

    string_view
    encoded_string() const noexcept
    {
        return s_;
    }

    /** Return the segment string.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    string(Allocator const& a = {}) const
    {
        return detail::pct_decode_unchecked(
            encoded_string(), {}, a);
    }

    value_type const*
    operator->() const noexcept
    {
        return this;
    }
};

//----------------------------------------------------------

class url::segments_type::iterator
{
    friend segments_type;

    url* v_;
    std::size_t off_;
    std::size_t n_;

    BOOST_URL_DECL
    iterator(
        url* v,
        bool end) noexcept;

    inline
    void
    check(url const* v) noexcept;

public:
    using iterator_category =
        std::bidirectional_iterator_tag;

    using value_type =
        segments_type::value_type;

    /// A pointer to an element
    using pointer = value_type const*;

    /// A reference to an element
    using reference = value_type const&;

    /// The difference_type for this iterator
    using difference_type = std::ptrdiff_t;

    BOOST_URL_DECL
    iterator() noexcept;

    BOOST_URL_DECL
    value_type
    operator*() const noexcept;

    value_type
    operator->() const noexcept
    {
        return operator*();
    }

    inline
    bool
    operator==(
    iterator other) const noexcept;

    bool
    operator!=(
        iterator other) const noexcept
    {
        return !(*this == other);
    }

    BOOST_URL_DECL
    iterator&
    operator++() noexcept;

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    BOOST_URL_DECL
    iterator&
    operator--() noexcept;

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

private:
    inline
    void
    parse() noexcept;
};

//----------------------------------------------------------

/** A modifiable view to the URL query parameters.
*/
class url::params_type
{
    url* v_ = nullptr;

public:
    class value_type;
    class iterator;
    using const_iterator = iterator;

    params_type() = delete;

    params_type(
        params_type const&) = default;
    params_type& operator=(
        params_type const&) = default;

    explicit
    params_type(url& v)
        : v_(&v)
    {
    }

    bool
    empty() const noexcept
    {
        return size() == 0;
    }

    std::size_t
    size() const noexcept
    {
        return (v_ == nullptr) ? 0 :
            v_->pt_.nparam;
    }

    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    BOOST_URL_DECL
    iterator
    end() const noexcept;

    BOOST_URL_DECL
    bool
    contains(string_view key) const noexcept;

    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    BOOST_URL_DECL
    iterator
    find(string_view key) const noexcept;

    BOOST_URL_DECL
    std::string
    operator[](string_view key) const;

    /** Return the param matching the given key.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param key The key to find. This parameter
        is not percent-encoded.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.
    */
    template<class Allocator =
        std::allocator<char>>
    string_type<Allocator>
    at( string_view key,
        Allocator const& a = {}) const;
};

//----------------------------------------------------------

class url::params_type::value_type
{
    string_view k_;
    string_view v_;

    friend class params_type;

    value_type(
        string_view k,
        string_view v) noexcept
        : k_(k)
        , v_(v)
    {
    }

public:
    value_type() = delete;
    value_type& operator=(
        value_type const&) = delete;

    value_type(
        value_type const&) = default;

    string_view
    encoded_key() const noexcept
    {
        return k_;
    }

    string_view
    encoded_value() const noexcept
    {
        return v_;
    }

    /** Return the key.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    key(Allocator const& a = {}) const
    {
        return detail::pct_decode_unchecked(
            encoded_key(), {}, a);
    }

    /** Return the value.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    value(Allocator const& a = {}) const
    {
        return detail::pct_decode_unchecked(
            encoded_value(), {}, a);
    }

    value_type const*
    operator->() const noexcept
    {
        return this;
    }

    operator
    std::pair<
        std::string const,
        std::string>() const
    {
        return { key(), value() };
    }
};

//----------------------------------------------------------

class url::params_type::iterator
{
    friend params_type;

    url* v_;
    std::size_t off_;
    std::size_t nk_;
    std::size_t nv_;

    BOOST_URL_DECL
    iterator(
        url* v,
        bool end) noexcept;

public:
    using value_type =
        params_type::value_type;

    BOOST_URL_DECL
    iterator() noexcept;

    BOOST_URL_DECL
    value_type
    operator*() const noexcept;

    value_type
    operator->() const noexcept
    {
        return operator*();
    }

    inline
    bool
    operator==(
        iterator other) const noexcept;

    bool
    operator!=(
        iterator other) const noexcept
    {
        return !(*this == other);
    }

    BOOST_URL_DECL
    iterator&
    operator++() noexcept;

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    BOOST_URL_DECL
    iterator&
    operator--() noexcept;

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

private:
    inline
    void
    parse() noexcept;
};

/** Format the encoded url to the output stream
*/
BOOST_URL_DECL
std::ostream&
operator<<(std::ostream& os, url const& u);

} // urls
} // boost

#endif
