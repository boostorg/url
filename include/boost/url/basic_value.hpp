//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_BASIC_VALUE_HPP
#define BOOST_URL_BASIC_VALUE_HPP

#include <boost/url/config.hpp>
#include <boost/url/view.hpp>
#include <boost/url/detail/char_type.hpp>
#include <boost/url/detail/parts.hpp>
#include <boost/url/detail/storage.hpp>
#include <memory>
#include <string>
#include <utility>

namespace boost {
namespace url {

/** A modifiable container for a URL.

    Objects of this type hold URLs which may be
    inspected and modified. The derived class
    is responsible for providing storage.

    The underlying string stored in the container
    is always null-terminated.

    @see value, dynamic_value, static_value

    @see
        @li <a href="https://tools.ietf.org/html/rfc3986">Uniform Resource Identifier (URI): Generic Syntax</a>
*/
class basic_value
{
    friend class view::segments_type;
    friend class view::params_type;

    detail::storage& a_;
    detail::parts pt_;
    char* s_ = nullptr;

private:
    template<class Allocator>
    friend class dynamic_value;

    template<std::size_t>
    friend class static_value;

    /** Construct an empty URL with the specified storage.
    */
    BOOST_URL_DECL
    basic_value(
        detail::storage& a) noexcept;

    /** Construct a parsed URL with the specified storage.

        If `s` is not a valid URL (a <em>URI-absolute</em>
        or a <em>relative-ref</em>), an exception is thrown.

        @param a The storage to use.
        
        @param s The URL to parse.

        @throw std::exception parse error.
    */
    BOOST_URL_DECL
    basic_value(
        detail::storage& a,
        string_view s);

public:
    class segments_type;
    class params_type;

    /** Return the number of characters in the URL.

        The value returned does not include the
        null terminator.

        @par Exception Safety

        No-throw guarantee.

        @return The number of characters in the
        URL.
    */
    std::size_t
    size() const noexcept
    {
        return pt_.offset[
            detail::id_end];
    }

    /** Return a pointer to the characters in the URL.

        The string is null terminated.

        @par Exception Safety

        No-throw guarantee.
    */
    char const*
    data() const noexcept
    {
        if(s_)
            return s_;
        return "";
    }

    /** Return the number of characters that may be stored without a reallocation.

        This function returns the maximum number of
        characters which may be stored in the URL before
        a reallocation is necessary.

        @par Exception Safety

        No-throw guarantee.
    */
    std::size_t
    capacity() const noexcept
    {
        return a_.capacity();
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
    basic_value&
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
    basic_value&
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
    basic_value&
    set_scheme(string_view s);

    //------------------------------------------------------
    //
    // authority
    //
    //------------------------------------------------------

    /** Return true if an authority is present.

        This function returns
        @code
        ! this->encoded_authority().empty();
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
    basic_value&
    set_encoded_authority(
        string_view s);

    //
    // userinfo
    //

    /** Return the userinfo.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_userinfo() const noexcept;

    /** Set the userinfo.

        Sets the userinfo of the URL to the given decoded
        string. The behavior then varies depending on the
        presence or absence of a colon (':')

        @li If one or more colons exist, then everything
        up to but not including the first colon will become
        the username, and everything beyond the first colon
        will become the password (including any subsequent
        colons).

        @li If no colons exist, then the username will be
        set to the passed userinfo, and the password will
        be empty.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The userinfo to set. This string
        must meed the syntactic requirements for
        the components of the userinfo, otherwise
        an exception is thrown.

        @throw std::exception invalid userinfo.
    */
    BOOST_URL_DECL
    basic_value&
    set_encoded_userinfo(
        string_view s);

    /** Return the username.

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
    username(
        Allocator const& a = {}) const
    {
        return detail::decode(
            encoded_username(), a);
    }

    /** Return the username.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_username() const noexcept;

    /** Set the username.

        The username may not include a colon.
    */
    BOOST_URL_DECL
    basic_value&
    set_username(
        string_view s);

    /** Set the encoded username.
    */
    BOOST_URL_DECL
    basic_value&
    set_encoded_username(
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

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_password() const noexcept;

    /** Set the password.
    */
    BOOST_URL_DECL
    basic_value&
    set_password(
        string_view s);

    /** Set the encoded password.
    */
    BOOST_URL_DECL
    basic_value&
    set_encoded_password(
        string_view s);

    //
    // host
    //

    /** Return the type of host present, if any.

        @par Exception Safety

        No-throw guarantee.
    */
    url::host_type
    host_type() const noexcept
    {
        return pt_.host;
    }

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
        if(pt_.host != host_type::name)
        {
            auto const s =  pt_.get(
                detail::id_host, s_);
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
        cleared. If the host was the last remaining
        portion of the authority, then the authority
        is removed including the leading double
        slash ("//"), else

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
    basic_value&
    set_host(
        string_view s);

    /** Set the host.

        The host is set to the specified encoded string,
        replacing any previous host:

        @li If the string is empty, the host is cleared.
        If the host was the last remaining portion of
        the authority, then the entire authority is
        removed including the leading double
        slash ("//"). Otherwise,

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
    basic_value&
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
    basic_value&
    set_port(unsigned n);

    /** Set the port.

        The port of the URL is set to the specified string.

        @li If the string is empty, the port is
        cleared including the leading colon (':'). If
        the port was the last remaining portion of
        the authority, then the entire authority is
        removed including the leading double slash ("//").
        Otherwise,

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
    basic_value&
    set_port(string_view s);

    /** Set the port.

        The port of the URL is set to the specified string.

        @li If the string is empty, the port is
        cleared including the leading colon (':'). If
        the port was the last remaining portion of
        the authority, then the entire authority is
        removed including the leading double slash ("//").
        Otherwise,

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
    basic_value&
    set_port_part(string_view s);

    /** Return the host.

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
    basic_value&
    set_encoded_path(
        string_view s);

    /** Return the path.

        This function returns the path segments
        as a lightweight, non-owning reference to
        the existing data, with the interface of
        a read-only container.

        @par Exception Safety

        No-throw guarantee.

        @see view::segments_type
    */
    inline
    view::segments_type
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
    basic_value&
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
    basic_value&
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
    basic_value&
    set_query_part(
        string_view s);

    /** Return the query.

        This function returns the query parameters
        as a lightweight, non-owning reference to
        the existing data, with the interface of
        a read-only associative container.

        @par Exception Safety

        No-throw guarantee.

        @see view::params_type
    */
    inline
    view::params_type
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
    */
    BOOST_URL_DECL
    basic_value&
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
    */
    BOOST_URL_DECL
    basic_value&
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
    */
    BOOST_URL_DECL
    basic_value&
    set_fragment_part(
        string_view s);

    //------------------------------------------------------
    //
    // normalization
    //
    //------------------------------------------------------

#if 0
    /** Normalize everything.
    */
    BOOST_URL_DECL
    basic_value&
    normalize();

    BOOST_URL_DECL
    basic_value&
    normalize_scheme();
#endif

private:
    inline char* resize(int id, std::size_t new_size);
    inline char* resize(int first, int last, std::size_t new_size);
};

//----------------------------------------------------------

/** A read-only view to the path segments.
*/
class basic_value::segments_type
{
    basic_value* v_ = nullptr;

public:
    class value_type;
    class iterator;
    using const_iterator = iterator;

    segments_type() = default;
    segments_type(segments_type const&) = default;
    segments_type& operator=(
        segments_type const&) = default;

    explicit
    segments_type(basic_value& v)
        : v_(&v)
    {
    }

    inline
    bool
    empty() const noexcept
    {
        return size() == 0;
    }

    inline
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
};

//----------------------------------------------------------

class basic_value::segments_type::value_type
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

/** A modifiable view to the URL query parameters.
*/
class basic_value::params_type
{
    basic_value* v_ = nullptr;

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
    params_type(basic_value& v)
        : v_(&v)
    {
    }

    inline
    bool
    empty() const noexcept
    {
        return size() == 0;
    }

    inline
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

class basic_value::params_type::value_type
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

} // url
} // boost

#include <boost/url/impl/basic_value.hpp>
#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/basic_value.ipp>
#endif

#endif
