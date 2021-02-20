//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_URL_BASE_HPP
#define BOOST_URL_URL_BASE_HPP

#include <boost/url/config.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/detail/char_type.hpp>
#include <boost/url/detail/parts.hpp>
#include <boost/url/detail/storage.hpp>
#include <memory>
#include <string>
#include <utility>

namespace boost {
namespace urls {

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

    @see value, basic_url, static_url

    @see @li <a href="https://tools.ietf.org/html/rfc3986">Uniform Resource Identifier (URI): Generic Syntax</a>
*/
class url_base
{
    friend class url_view::segments_type;
    friend class url_view::params_type;

    detail::parts_string pt_;

private:
    template<class Allocator>
    friend class basic_url;

    template<std::size_t>
    friend class static_url;

    /** Construct an empty URL with the specified storage.
    */
    BOOST_URL_DECL
    url_base(
        detail::storage& a) noexcept;

    /** Construct a parsed URL with the specified storage.

        If `s` is not a valid URL (a <em>URI-absolute</em>
        or a <em>relative-ref</em>), an exception is thrown.

        @param a The storage to use.

        @param s The URL to parse.

        @throw std::exception parse error.
    */
    BOOST_URL_DECL
    url_base(
        detail::storage& a,
        string_view s);

public:
    class segments_type;
    class params_type;

    //
    // Observers
    //

    /** Return the number of characters in the URL.

        The value returned does not include the
        null terminator.

        @return The number of characters in the
        URL.
    */
    std::size_t
    size() const noexcept
    {
        return pt_.length_all();
    }

    /** Return a pointer to the characters in the URL.

        The string is null terminated.
    */
    char const*
    data() const noexcept
    {
        return pt_.c_str();
    }

    /** Return the number of characters that may be stored without a reallocation.

        This function returns the maximum number of
        characters which may be stored in the URL before
        a reallocation is necessary.
    */
    std::size_t
    capacity() const noexcept
    {
        return pt_.capacity();
    }

    //------------------------------------------------------

    /** Return the URL.

        All special characters appearing in corresponding
        parts of the URL will appear percent-encoded.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_url() const;

    /** Return the origin.

        The origin consists of the everything from the
        beginning of the URL up to but not including
        the path. Any special or reserved characters in
        the origin will be returned in percent-encoded
        form.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_origin() const noexcept;

    /** Set the URL.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The URL to set. The contents must
        meet the syntactic requirements of a
        <em>URI-reference</em>.

        @throw std::exception parsing error.
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_url(
        string_view s);

    /** Set the origin to the specified value.

        The origin consists of the everything from the
        beginning of the URL up to but not including
        the path.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The origin to set. Special characters
        must be percent-encoded, or an exception is
        thrown.
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_origin(
        string_view s);

    //------------------------------------------------------
    //
    // scheme
    //
    //------------------------------------------------------

    /** Return the scheme.

        If there is no scheme, an empty string is
        returned. Otherwise the scheme is returned,
        without a trailing colon (':').

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    scheme() const noexcept;

    /** Set the scheme.

        This function sets the scheme to the specified
        string:

        @li If the string is empty, any existing scheme
        is removed along with the trailing colon (':'),
        otherwise:

        @li The scheme is set to the string, which must
        contain a valid scheme. A trailing colon is
        automatically added.

        @par ABNF
        @code
        scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The scheme to set. This string must
        not include a trailing colon, otherwise an
        exception is thrown.

        @throw std::exception invalid scheme.
    */
    BOOST_URL_DECL
    url_base&
    set_scheme(string_view s);

    //------------------------------------------------------
    //
    // authority
    //
    //------------------------------------------------------

    /** Return true if an authority is present.

        This function returns
        @code
        !this->encoded_authority().empty();
        @endcode

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    bool
    has_authority() const noexcept;

    /** Return the authority.

        @par Exception Safety

        No-throw guarantee.

        @return The authority string, with special
        characters escaped using percent-encoding.
    */
    BOOST_URL_DECL
    string_view
    encoded_authority() const noexcept;

    /** Set the authority.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The authority to set. This string
        must meed the syntactic requirements for
        the components of the authority, otherwise
        an exception is thrown.

        @throw std::exception invalid authority.
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_authority(
        string_view s);

    //------------------------------------------------------
    //
    // userinfo
    //
    //------------------------------------------------------

    /** Return `true` if a userinfo is present.

        This function returns `true` if there are
        any characters in the URL's userinfo, including
        the at sign ('@') separator.
    */
    BOOST_URL_DECL
    bool
    has_userinfo() const noexcept;

    /** Return the userinfo.

        Returns the userinfo of the URL as an encoded
        string. The userinfo includes the user and
        password, with a colon separating the components
        if the password is not empty.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_userinfo() const noexcept;

    /** Return the userinfo.

        Returns the userinfo part of the URL as an
        encoded string. The userinfo part includes the
        user and password, with a colon separating the
        components if the password is not empty, and
        a trailing at sign ('@') if either component
        is not empty.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    userinfo_part() const noexcept;

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

        @par ABNF
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
    url_base&
    set_encoded_userinfo(
        string_view s);

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
        of <em>userinfo-part</em> otherwise an exception
        is thrown.

        @par ABNF
        @code
        userinfo-part = [ [ user ] [ ':' password ] '@' ]
        user          = *( unreserved / pct-encoded / sub-delims )
        password      = *( unreserved / pct-encoded / sub-delims / ":" )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url_base&
    set_userinfo_part(
        string_view s);

    /** Return the user.

        This function returns the user portion of
        the userinfo if present, as a decoded string.
        The user portion is defined by all of the
        characters in the userinfo up to but not
        including the first colon (':"), or the
        entire userinfo if no colon is present.

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
    user(
        Allocator const& a = {}) const
    {
        return detail::decode(
            encoded_user(), a);
    }

    /** Return the user.

        This function returns the user portion of
        the userinfo if present, as an encoded string.
        The user portion is defined by all of the
        characters in the userinfo up to but not
        including the first colon (':"), or the
        entire userinfo if no colon is present.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_user() const noexcept;

    /** Set the user.

        The user is set to the specified string,
        replacing any previous user:

        @li If the string is empty, the user is cleared.

        @li If the string is not empty then the
        user is set to the new string.
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
    set_user(
        string_view s);

    /** Set the user.

        The user is set to the specified encoded
        string, replacing any previous user:

        @li If the string is empty, the user is cleared.

        @li If the string is not empty then the
        user is set to the given string.
        If the URL previously did not have an authority
        (@ref has_authority returns `false`), a double
        slash ("//") is prepended to the userinfo.
        The string must meet the syntactic requirements
        of <em>user</em> otherwise an exception is
        thrown.

        @li
        @par ABNF
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

    /** Return the password.

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
    password(
        Allocator const& a = {}) const
    {
        return detail::decode(
            encoded_password(), a);
    }

    /** Return the password.

        This function returns the password portion of
        the userinfo if present, as an encoded string.
        The password portion is defined by all the
        characters in the userinfo after the first
        colon (':').

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_password() const noexcept;

    /** Return the password.

        This function returns the password part of
        the userinfo if present, as an encoded string.
        This will include any leading colon (':')
        The password portion is defined by all the
        characters in the userinfo after the first
        colon (':').

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    password_part() const noexcept;

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

    /** Set the password.

        The password part is set to the encoded string
        `s`, replacing any previous password:

        @li If the string is empty, the password is
        cleared, and the first occurring colon (':') is
        removed from the userinfo if present, otherwise

        @li If ths string is not empty then the password
        is set to the new string, which must include a
        leading colon.
        If the URL previously did not have an authority
        (@ref has_authority returns `false`), a double
        slash ("//") is prepended to the userinfo.
        The string must meet the syntactic requirements
        of <em>password-part</em> otherwise an exception is
        thrown.

        @par ANBF
        @code
        password-part = [ ':' *( unreserved / pct-encoded / sub-delims / ":" ) ]
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url_base&
    set_password_part(
        string_view s);

    //------------------------------------------------------
    //
    // host
    //
    //------------------------------------------------------

    /** Return the type of host present, if any.

        @par Exception Safety

        No-throw guarantee.
    */
    urls::host_type
    host_type() const noexcept
    {
        return pt_.host;
    }

    /** Return the host and port.

        This function returns the encoded host and port,
        or an empty string if there is no host or port.
        The returned value includes both the host if present,
        and a port, with a colon separating the host and port
        if either component is non-empty.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_host_and_port() const noexcept;

    /** Return the host.

        This function returns the host portion of
        the authority as a decoded string if present,
        otherwise it returns an empty string.

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
    host(
        Allocator const& a = {}) const
    {
        if(pt_.host != urls::host_type::name)
        {
            auto const s =  pt_.get(
                detail::id_host);
            return string_type<Allocator>(
                s.data(), s.size(), a);
        }
        return detail::decode(
            encoded_host(), a);
    }

    /** Return the host.

        This function returns the host portion of
        the authority as an encoded string if present,
        otherwise it returns an empty string.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_host() const noexcept;

    /** Set the host.

        The host is set to the specified string,
        replacing any previous host:

        @li If the string is empty, the host is cleared.
        If there are no more remaining elements in the
        authority, then the authority is removed including
        the leading double slash ("//"), else

        @li If the string is a valid <em>IPv4Address</em>,
        the host is set to the new string and
        @ref host_type will return
        @ref host_type::ipv4, otherwise

        @li If the string is a valid <em>IPv6Address</em>,
        the host is set to the new string and
        @ref host_type will return
        @ref host_type::ipv6, else

        @li If the string is a valid <em>IPv6Future</em>,
        the host is set to the new string and
        @ref host_type will return
        @ref host_type::ipvfuture, else

        @li The host is set to the new string.
        Any special or reserved characters in the
        string are automatically percent-encoded.

        In all cases where the string is valid and not empty,
        if the URL previously did not contain an
        authority (@ref has_authority returns `false`),
        and `s` is not empty, then the authority is added
        including a leading double slash ("//").

        @par ABNF
        @code
        IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet

        IPv6address   =                            6( h16 ":" ) ls32
                      /                       "::" 5( h16 ":" ) ls32
                      / [               h16 ] "::" 4( h16 ":" ) ls32
                      / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
                      / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
                      / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
                      / [ *4( h16 ":" ) h16 ] "::"              ls32
                      / [ *5( h16 ":" ) h16 ] "::"              h16
                      / [ *6( h16 ":" ) h16 ] "::"

        IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set. This string may
        contain any characters, including nulls.
    */
    BOOST_URL_DECL
    url_base&
    set_host(
        string_view s);

    /** Set the host.

        The host is set to the specified encoded string,
        replacing any previous host:

        @li If the string is empty, the host is cleared.
        If there are no more remaining elements in the
        authority, then the authority is removed including
        the leading double slash ("//"), else

        @li If the string is not empty, the host
        is set to the new string.
        The string must meet the syntactic requirements
        of <em>host</em> otherwise an exception is
        thrown.
        If the URL previously did not contain an
        authority (@ref has_authority returns `false`),
        then the authority is added including the
        leading double slash ("//").

        @par ABNF
        @code
        host          = IP-literal / IPv4address / reg-name

        reg-name      = *( unreserved / pct-encoded / sub-delims )

        IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"

        IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet

        IPv6address   =                            6( h16 ":" ) ls32
                      /                       "::" 5( h16 ":" ) ls32
                      / [               h16 ] "::" 4( h16 ":" ) ls32
                      / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
                      / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
                      / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
                      / [ *4( h16 ":" ) h16 ] "::"              ls32
                      / [ *5( h16 ":" ) h16 ] "::"              h16
                      / [ *6( h16 ":" ) h16 ] "::"

        IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )

        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url_base&
    set_encoded_host(
        string_view s);

    /** Return the port.

        If the URL contains a port, this function
        returns the port string without a leading
        colon (':'). Otherwise, an empty string
        is returned.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    port() const noexcept;

    /** Return the port.

        If the URL contains a port, this function
        returns the port string including a leading
        colon (':'). Otherwise, an empty string
        is returned.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    port_part() const noexcept;

    /** Set the port.

        The port of the URL is set to the specified
        integer, replacing any previous port.

        If the URL previously did not contain an
        authority (@ref has_authority returns `false`),
        then the authority is added including the
        leading double slash ("//").

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param n The port number to set.
    */
    BOOST_URL_DECL
    url_base&
    set_port(unsigned n);

    /** Set the port.

        The port of the URL is set to the specified string.

        @li If the string is empty, the port is
        cleared including the leading colon (':').
        If there are no more remaining elements in the
        authority, then the authority is removed including
        the leading double slash ("//"), else

        @li If the string is not empty then the port
        is set to the given string, with a leading
        colon added.
        If the URL previously did not contain an
        authority (@ref has_authority returns `false`),
        then the authority is added including the
        leading double slash ("//").
        The string must meet the syntactic requirements
        of <em>port</em> otherwise an exception is
        thrown.

        @par ABNF
        @code
        port          = *DIGIT
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url_base&
    set_port(string_view s);

    /** Set the port.

        The port of the URL is set to the specified string.

        @li If the string is empty, the port is
        cleared including the leading colon (':').
        If there are no more remaining elements in the
        authority, then the authority is removed including
        the leading double slash ("//"), else

        @li If the string is not empty then the port
        is set to the given string, which must have
        a starting colon.
        If the URL previously did not contain an
        authority (@ref has_authority returns `false`),
        then the authority is added including the
        leading double slash ("//").
        The string must meet the syntactic requirements
        of <em>port-part</em> otherwise an exception is
        thrown.

        @par ABNF
        @code
        port-part     = [ ':' *DIGIT ]
        @endcode

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The string to set.
    */
    BOOST_URL_DECL
    url_base&
    set_port_part(string_view s);

    //------------------------------------------------------
    //
    // path
    //
    //------------------------------------------------------

    /** Return the path.

        This function returns the path of the URL
        as a percent-encoded string.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_path() const noexcept;

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

        @par ABNF
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
    url_base&
    set_encoded_path(
        string_view s);

    /** Return the path.

        This function returns the path segments
        as a lightweight, non-owning reference to
        the existing data, with the interface of
        a read-only container.

        @par Exception Safety

        No-throw guarantee.

        @see url_view::segments_type
    */
    inline
    url_view::segments_type
    segments() const noexcept;

    /** Return the path.

        This function returns the path segments
        as a lightweight, non-owning reference to
        the existing data, with the interface of
        a modifiable container.

        @par Exception Safety

        No-throw guarantee.

        @see segments_type
    */
    inline
    segments_type
    segments() noexcept;

    //------------------------------------------------------
    //
    // query
    //
    //------------------------------------------------------

    /** Return the query.

        This function returns the query of the URL:

        @li If a query is present, it is returned
        in decoded form without a leading question
        mark ('?'), otherwise:

        @li If there is no query, an empty string is
        returned.

        Note that if the URL contains a question mark
        followed by an empty query string, this
        function still returns an empty string.
        To detect this case, use @ref query_part
        instead.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.

        @see encoded_query, query_part
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    query(
        Allocator const& a = {}) const
    {
        return detail::decode(
            encoded_query(), a);
    }

    /** Return the query.

        This function returns the query of the URL:

        @li If a query is present, it is returned
        in encoded form without a leading question
        mark ('#'), otherwise:

        @li If there is no query, an empty string is
        returned.

        Note that if the URL contains a question
        mark followed by an empty query string,
        this function still returns an empty string.
        To detect this case, use @ref query_part
        instead.

        @par Exception Safety

        No-throw guarantee.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.

        @see query, query_part
    */
    BOOST_URL_DECL
    string_view
    encoded_query() const noexcept;

    /** Return the query.

        This function returns the query of the URL:

        @li If a query is present, it is returned
        in encoded form including the leading hash
        mark ('?'), otherwise:

        @li If there is no query, an empty string is
        returned.

        Note that if the URL contains a question
        mark followed by an empty query string,
        this function returns "#".

        @par Exception Safety

        No-throw guarantee.

        @see query, encoded_query
    */
    BOOST_URL_DECL
    string_view
    query_part() const noexcept;

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
    url_base&
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

        @par ABNF
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
    url_base&
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

        @par ABNF
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
    url_base&
    set_query_part(
        string_view s);

    /** Return the query.

        This function returns the query parameters
        as a lightweight, non-owning reference to
        the existing data, with the interface of
        a read-only associative container.

        @par Exception Safety

        No-throw guarantee.

        @see url_view::params_type
    */
    inline
    url_view::params_type
    params() const noexcept;

    /** Return the query.

        This function returns the query parameters
        as a lightweight, non-owning reference to
        the existing data, with the interface of
        a modifiable associative container.

        @par Exception Safety

        No-throw guarantee.

        @see params_type
    */
    inline
    params_type
    params() noexcept;

    //------------------------------------------------------
    //
    // fragment
    //
    //------------------------------------------------------

    /** Return the fragment.

        This function returns the fragment of the URL:

        @li If a fragment is present, it is returned in
        decoded form without a leading hash mark ('#'),
        otherwise:

        @li If there is no fragment, an empty string is
        returned.

        Note that if the URL contains a hash mark
        followed by an empty query string, this
        function still returns an empty string.
        To detect this case, use @ref fragment_part
        instead.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.

        @see encoded_fragment, fragment_part
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    fragment(
        Allocator const& a = {}) const
    {
        return detail::decode(
            encoded_fragment(), a);
    }

    /** Return the fragment.

        This function returns the fragment of the URL:

        @li If a fragment is present, it is returned in
        encoded form without a leading hash mark ('#'),
        otherwise:

        @li If there is no fragment, an empty string is
        returned.

        Note that if the URL contains a hash mark
        followed by an empty query string, this
        function still returns an empty string.
        To detect this case, use @ref fragment_part
        instead.

        @par Exception Safety

        No-throw guarantee.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.

        @see fragment, fragment_part
    */
    BOOST_URL_DECL
    string_view
    encoded_fragment() const noexcept;

    /** Return the fragment.

        This function returns the fragment of the URL:

        @li If a fragment is present, it is returned
        in encoded form including the leading hash
        mark ('#'), otherwise:

        @li If there is no fragment, an empty string is
        returned.

        Note that if the URL contains a hash mark
        followed by an empty query string, this
        function returns "#".

        @par Exception Safety

        No-throw guarantee.

        @see fragment, encoded_fragment
    */
    BOOST_URL_DECL
    string_view
    fragment_part() const noexcept;

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
    url_base&
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

        @par ABNF
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
    url_base&
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

        @par ABNF
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
    url_base&
    set_fragment_part(
        string_view s);

    //------------------------------------------------------
    //
    // normalization
    //
    //------------------------------------------------------

    /** Normalize everything.
    */
    BOOST_URL_DECL
    url_base&
    normalize();

    BOOST_URL_DECL
    url_base&
    normalize_scheme() noexcept;

private:
    inline char* resize(int id, std::size_t new_size);
    inline char* resize(int first, int last, std::size_t new_size);
};

//----------------------------------------------------------

/** A read/write view to the path segments.
*/
class url_base::segments_type
{
    detail::parts_string* pt_ = nullptr;

public:

    class value_type;
    class iterator;
    using const_iterator = iterator;

private:

    BOOST_URL_DECL
    iterator
    insert_encoded_impl( iterator pos, string_view s );

    BOOST_URL_DECL
    iterator
    insert_impl( iterator pos, string_view s, std::size_t ns );

public:

    segments_type() = default;
    segments_type(segments_type const&) = default;
    segments_type& operator=(
        segments_type const&) = default;

    explicit
    segments_type(url_base& v)
        : pt_(&v.pt_)
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
        return (pt_ == nullptr) ? 0 :
            pt_->nseg;
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

class url_base::segments_type::value_type
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
        return detail::decode(
            encoded_string(), a);
    }

    value_type const*
    operator->() const noexcept
    {
        return this;
    }
};

//----------------------------------------------------------

class url_base::segments_type::iterator
{
    friend segments_type;

    detail::parts_string* pt_;
    std::size_t off_;
    std::size_t n_;

    BOOST_URL_DECL
    iterator(
        detail::parts_string* pt,
        bool end) noexcept;

    char*
    ptr() const noexcept
    {
        BOOST_ASSERT(pt_ || off_==0);
        BOOST_ASSERT(off_<=pt_->offset(detail::id_end));
        return pt_->data() + off_;
    }

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
class url_base::params_type
{
    detail::parts_string* pt_ = nullptr;

public:
    class value_type;
    class iterator;
    using const_iterator = iterator;

    params_type() = default;
    params_type(
        params_type const&) = default;
    params_type& operator=(
        params_type const&) = default;

    explicit
    params_type(url_base& v)
        : pt_(&v.pt_)
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
        return (pt_ == nullptr) ? 0 :
            pt_->nparam;
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

class url_base::params_type::value_type
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
        return detail::decode(
            encoded_key(), a);
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
        return detail::decode(
            encoded_value(), a);
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

class url_base::params_type::iterator
{
    friend params_type;

    detail::parts_string* pt_;
    std::size_t off_;
    std::size_t nk_;
    std::size_t nv_;

    BOOST_URL_DECL
    iterator(
        detail::parts_string* pt,
        bool end) noexcept;

    char*
    ptr() const noexcept
    {
        BOOST_ASSERT(pt_ || off_==0);
        BOOST_ASSERT(off_<=pt_->offset(detail::id_end));
        return pt_->data() + off_;
    }

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

} // urls
} // boost

#include <boost/url/impl/url_base.hpp>
#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/url_base.ipp>
#endif

#endif
