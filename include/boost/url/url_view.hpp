//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_URL_VIEW_HPP
#define BOOST_URL_URL_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/ipv4_address.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/path_view.hpp>
#include <boost/url/query_params_view.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/detail/parts.hpp>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>

namespace boost {
namespace urls {

/** A parsed reference to a URL string.
*/
class url_view
{
    char const* s_ = "";
    detail::parts pt_;

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
    friend class url;
    struct shared_impl;

    // shortcuts
    string_view get(int id) const noexcept;
    string_view get(int id0, int id1) const noexcept;
    std::size_t len(int id) const noexcept;
    std::size_t len(int id0, int id1) const noexcept;

    url_view(
        char const* s,
        detail::parts const& pt) noexcept
        : s_(s)
        , pt_(pt)
    {
    }

public:
    /** Constructor

        Default constructed views represent a
        relative-ref with an empty path. That is,
        their representation is a string of
        zero length.
    */
    BOOST_URL_DECL
    url_view() noexcept;

    /** Return a copy of the view with extended lifetime.

        This function makes a copy of the storage
        pointed to by this, and attaches it to a
        new constant view returned in a shared
        pointer. The lifetime of the storage for
        the characters will extend for the lifetime
        of the shared object. This allows the new
        view to be copied and passed around after
        the original string buffer is destroyed.

        @par Example
        @code
        std::shared_ptr<url_view const> sp;
        {
            std::string s( "http://example.com" );
            url_view u = parse_uri( s ); // u references characters in s
            sp = u.collect();

            // s is destroyed and thus u
            // becomes invalid, but sp remains valid.
        }
        std::cout << *sp; // works
        @endcode
    */
    BOOST_URL_DECL
    std::shared_ptr<
        url_view const>
    collect() const;

    //--------------------------------------------
    //--------------------------------------------
    //--------------------------------------------

    //--------------------------------------------
    //
    // classification
    //
    //--------------------------------------------

    /** An integer for the maximum size string that can be represented
    */
    static
    constexpr
    std::size_t
    max_size()
    {
        return 0x7ffffffe;
    }

    /** Return true if the URL is empty

        An empty URL is a relative-ref with
        zero path segments.
    */
    BOOST_URL_DECL
    bool
    empty() const noexcept;

    //--------------------------------------------

    /** Return the origin
    */
    BOOST_URL_DECL
    string_view
    encoded_origin() const noexcept;

    //--------------------------------------------
    //
    // scheme
    //
    //--------------------------------------------

    /** Return true if a scheme exists

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>
    */
    BOOST_URL_DECL
    bool
    has_scheme() const noexcept;

    /** Return the scheme

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>
    */
    BOOST_URL_DECL
    string_view
    scheme() const noexcept;

    /** Return a known-scheme constant if a scheme is present

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1">
            3.1. Scheme (rfc3986)</a>
    */
    BOOST_URL_DECL
    urls::scheme
    scheme_id() const noexcept;

    //--------------------------------------------
    //
    // authority
    //
    //--------------------------------------------

    /** Return true if an authority exists

        This function returns true if an authority is
        present, even if the authority is an empty
        string. Its presence
        in a URL is determined by a leading double-slash
        ("//") in the 
        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_URL_DECL
    bool
    has_authority() const noexcept;

    /** Return the authority if it exists, or an empty string

        @see has_authority
    */
    BOOST_URL_DECL
    string_view
    encoded_authority() const noexcept;

    /** Return true if a userinfo exists

        This function returns true if the authority
        is present and contains an at-sign ('@')
        which is not percent encoded.
    */
    BOOST_URL_DECL
    bool
    has_userinfo() const noexcept;

    /** Return the userinfo if it exists, or an empty string

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

    /** Return the userinfo if it exists, or an empty string

        This function returns the userinfo part
        of the URL if present, as a decoded string.

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
    userinfo(
        Allocator const& a = {}) const
    {
        return detail::pct_decode_unchecked(
            encoded_userinfo(), {}, a);
    }

    /** Return the user if it exists, or an empty string

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

    /** Return the user if it exists, or an empty string

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
        return detail::pct_decode_unchecked(
            encoded_user(), {}, a);
    }

    /** Return true if a password exists
    */
    BOOST_URL_DECL
    bool
    has_password() const noexcept;

    /** Return the password if it exists, or an empty string
    */
    BOOST_URL_DECL
    string_view
    encoded_password() const noexcept;

    /** Return the password if it exists, or an empty string
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    password(
        Allocator const& a = {}) const
    {
        return detail::pct_decode_unchecked(
            encoded_password(), {}, a);
    }

    //--------------------------------------------

    /** Return the type of host present, if any

        @par Exception Safety

        No-throw guarantee.
    */
    urls::host_type
    host_type() const noexcept
    {
        return pt_.host_type;
    }

    /** Return the host

        This function returns the host portion of
        the authority as an encoded string if present,
        otherwise it returns an empty string.

        @par Exception Safety
        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_host() const noexcept;

    /** Return the host

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
        auto const s0 =
            encoded_host();
        if(pt_.host_type !=
            urls::host_type::name)
        {
            // no decoding
            return string_type<Allocator>(
                s0.data(), s0.size(), a);
        }
        return detail::pct_decode_unchecked(
            s0, pt_.decoded[id_host], {}, a);
    }

    /** Return the ipv4 address if it exists, or return the unspecified address (0.0.0.0)
    */
    BOOST_URL_DECL
    urls::ipv4_address
    ipv4_address() const noexcept;

    /** Return the ipv6 address if it exists, or return the unspecified address (0:0:0:0:0:0:0:0)
    */
    BOOST_URL_DECL
    urls::ipv6_address
    ipv6_address() const noexcept;

    /** Return the ipvfuture address if it exists, or return an empty string
    */
    BOOST_URL_DECL
    string_view
    ipv_future() const noexcept;

    /** Return true if the URL contains a port
    */
    BOOST_URL_DECL
    bool
    has_port() const noexcept;

    /** Return the port if it exists, or an empty string

        If the URL contains a port, this function
        returns the entire port string, which may
        or may not be a decimal number.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    port() const noexcept;

    /** Return the integer port number, or 0 if no port

        If the URL contains a port string and the
        string contains a decimal value in the range
        0 to 65535, returns the number. Otherwise,
        returns zero.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    std::uint16_t
    port_number() const noexcept;

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

    //--------------------------------------------
    //
    // path
    //
    //--------------------------------------------

    /** Return the encoded path
    */
    BOOST_URL_DECL
    string_view
    encoded_path() const noexcept;

    /** Return the path segments as a read-only forward range
    */
    BOOST_URL_DECL
    path_view
    path() const noexcept;

    //--------------------------------------------
    //
    // query
    //
    //--------------------------------------------

    /** Return true if a query exists

        A query exists if the hash mark ('#') is
        present after the path, even if followed
        by an empty query string.

        @see encoded_query, query
    */
    BOOST_URL_DECL
    bool
    has_query() const noexcept;

    /** Return the query if it exists, or an empty string

        This function returns the percent-encoded
        query if it exists. Otherwise it returns
        an empty string.

        @par Exception Safety
        No-throw guarantee.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.

        @see has_query, query
    */
    BOOST_URL_DECL
    string_view
    encoded_query() const noexcept;

    /** Return the query if it exists, or an empty string

        If the query exists, it is returned as a
        string with percent-decoding applied.
        Otherwise, an empty string is returned.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.

        @see encoded_query
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    query(
        Allocator const& a = {}) const
    {
        return detail::pct_decode_unchecked(
            encoded_query(), {}, a);
    }

    /** Return the query parameters as a read-only forward range
    */
    BOOST_URL_DECL
    query_params_view
    query_params() const noexcept;

    //--------------------------------------------
    //
    // fragment
    //
    //--------------------------------------------

    /** Return true if a fragment is present
    */
    BOOST_URL_DECL
    bool
    has_fragment() const noexcept;

    /** Return the fragment if present, otherwise return an empty string

        This function returns the fragment as a
        percent-encoded string if present, otherwise
        returns an empty string. The returned string
        does not include the leading hash mark ('#').

        @par Exception Safety

        No-throw guarantee.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.

        @see fragment
    */
    BOOST_URL_DECL
    string_view
    encoded_fragment() const noexcept;

    /** Return the fragment.

        This function returns the fragment of the URL:

        * If a fragment is present, it is returned in
        decoded form without a leading hash mark ('#'),
        otherwise:

        * If there is no fragment, an empty string is
        returned.

        Note that if the URL contains a hash mark
        followed by an empty query string, this
        function still returns an empty string.
        To detect this case, use @ref fragment_bnf
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

        @see encoded_fragment, fragment_bnf
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    fragment(
        Allocator const& a = {}) const
    {
        return detail::pct_decode_unchecked(
            encoded_fragment(),
            pt_.decoded[id_frag], {}, a);
    }

    //--------------------------------------------
    //--------------------------------------------
    //--------------------------------------------

    /** Return the complete encoded URL
    */
    BOOST_URL_DECL
    string_view
    str() const;

    //--------------------------------------------
    //
    // free functions
    //
    //--------------------------------------------

    BOOST_URL_DECL friend url_view parse_uri(
        string_view s, error_code& ec) noexcept;
    BOOST_URL_DECL friend url_view parse_relative_ref(
        string_view s, error_code& ec) noexcept;
    BOOST_URL_DECL friend url_view parse_uri_reference(
        string_view s, error_code& ec) noexcept;
};

//------------------------------------------------

/** Parse a URI

    @par BNF
    @code
    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3
*/
BOOST_URL_DECL
url_view
parse_uri(
    string_view s,
    error_code& ec) noexcept;

/** Parse a URI

    @par BNF
    @code
    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3
*/
BOOST_URL_DECL
url_view
parse_uri(string_view s);

/** Parse a relative-ref

    @par BNF
    @code
    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]

    relative-part = "//" authority path-abempty
                  / path-absolute
                  / path-noscheme
                  / path-empty
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3
*/
BOOST_URL_DECL
url_view
parse_relative_ref(
    string_view s,
    error_code& ec) noexcept;

/** Parse a relative-ref

    @par BNF
    @code
    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]

    relative-part = "//" authority path-abempty
                  / path-absolute
                  / path-noscheme
                  / path-empty

    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3
*/
BOOST_URL_DECL
url_view
parse_relative_ref(
    string_view s);

/** Parse a URI-reference

    @par BNF
    @code
    URI-reference = URI / relative-ref

    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3
*/
BOOST_URL_DECL
url_view
parse_uri_reference(
    string_view s,
    error_code& ec) noexcept;

BOOST_URL_DECL
url_view
parse_uri_reference(
    string_view s);

/** Format the encoded url to the output stream
*/
BOOST_URL_DECL
std::ostream&
operator<<(
    std::ostream& os,
    url_view const& u);

} // urls
} // boost

#include <boost/url/impl/url_view.hpp>

#endif
