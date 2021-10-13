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
#include <boost/url/detail/except.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <boost/assert.hpp>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <memory>
#include <utility>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
struct authority_bnf;
struct fragment_part_bnf;
struct host_bnf;
struct parsed_path;
struct query_part_bnf;
struct scheme_part_bnf;
#endif

/** A read-only view to a URL

    Objects of this type represent valid URLs
    whose storage is managed externally. That
    is, it acts like a `std::string_view` in
    terms of ownership. Callers are responsible
    for ensuring that the lifetime of the
    underlying string extends until the
    `url_view` is no longer in use.
    To construct from an existing string it is
    necessary to use one of the parsing
    functions. Each function parses against
    a particular URL grammar:

    @par Example
    @code
    url_view u;

    u = parse_uri( "http://www.example.com/index.html" ).value();

    u = parse_relative_ref( "/path/to/file.txt" ).value();
    @endcode

    @par BNF
    @code
    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

    URI-reference = URI / relative-ref

    absolute-URI  = scheme ":" hier-part [ "?" query ]

    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
    @endcode

    @see
        @ref parse_absolute_uri,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref parse_uri_reference.
*/
class BOOST_SYMBOL_VISIBLE url_view
    : protected detail::parts_base
{
#ifndef BOOST_URL_DOCS
protected:
#endif
    char const* cs_ = empty_;
    pos_t offset_[id_end + 1] = {};
    pos_t decoded_[id_end] = {};
    pos_t nseg_ = 0;
    pos_t nparam_ = 0;
    unsigned char ip_addr_[16] = {};
    // VFALCO don't we need a bool?
    std::uint16_t port_number_ = 0;
    urls::host_type host_type_ =
        urls::host_type::none;
    urls::scheme scheme_ =
        urls::scheme::none;

    friend class url;
    friend class static_url_base;
    struct shared_impl;

    inline url_view& base() noexcept;
    inline url_view const& base() const noexcept;
    inline std::size_t table_bytes() const noexcept;
    inline pos_t len(int first, int last) const noexcept;
    inline void set_size(int id, pos_t n) noexcept;
    inline void split(int id, std::size_t n) noexcept;
    inline void adjust(int first, int last,
        std::size_t n) noexcept;
    inline void collapse(int first, int last,
        std::size_t n) noexcept;

    BOOST_URL_DECL
    explicit
    url_view(char const* cs) noexcept;

    BOOST_URL_DECL
    url_view(
        url_view const& u,
        char const* cs) noexcept;

    // return offset of id
    BOOST_URL_CONSTEXPR
    auto
    offset(int id) const noexcept ->
        pos_t
    {
        return
            id == id_scheme ?
            zero_ : offset_[id];
    }

    // return length of part
    BOOST_URL_CONSTEXPR
    auto
    len(int id) const noexcept ->
        pos_t
    {
        return
            offset(id + 1) -
            offset(id);
    }

    // return id as string
    BOOST_URL_CONSTEXPR
    string_view
    get(int id) const noexcept
    {
        return {
            cs_ + offset(id), len(id) };
    }

    // return [first, last) as string
    BOOST_URL_CONSTEXPR
    string_view
    get(int first,
        int last) const noexcept
    {
        return { cs_ + offset(first),
            offset(last) - offset(first) };
    }

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

        Any params, segments, or iterators
        which reference this object are
        invalidated. The ownership and lifetime
        of the underlying character buffer
        remains unchanged.
    */
    BOOST_URL_DECL
    virtual ~url_view();

    /** Constructor

        Default constructed views refer to
        a string with zero length, which
        always remains valid. This matches
        the grammar for a relative-ref with
        an empty path and no query or
        fragment.

        @par BNF
        @code
        relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.2"
            >4.2. Relative Reference (rfc3986)</a>
    */
    BOOST_URL_DECL
    url_view() noexcept;

    /** Constructor
    */
    BOOST_URL_DECL
    url_view(url_view const&) noexcept;

    /** Assignment
    */
    BOOST_URL_DECL
    url_view&
    operator=(url_view const&) noexcept;

    //--------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------

    /** Return the maximum number of characters allowed in a URL.

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
        return offset(id_end);
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

    /** Return a pointer to the first character of the view

        This function returns a pointer to the
        first character of the view, which is
        not guaranteed to be null-terminated.

        @par Exception Safety
        Throws nothing.
    */
    char const*
    data() const noexcept
    {
        return cs_;
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
        return cs_[pos];
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
        return cs_[pos];
    }

    /** Return an iterator to the beginning

        This function returns a constant iterator
        to the first character of the URL, or
        one past the last element if the URL is
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
        the URL. This character acts as a placeholder,
        attempting to access it results in undefined
        behavior.
    */
    char const*
    end() const noexcept
    {
        return data() + size();
    }

    /** Return the complete encoded URL

        This function returns the URL as a
        percent-encoded string.

        @par Exception Safety
        Throws nothing.
    */
    string_view
    encoded_url() const noexcept
    {
        return string_view(
            data(), size());
    }

    /** Return a read-only copy of the URL, with shared lifetime.

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
            url_view u = parse_uri( s ).value();    // u references characters in s

            assert( u.data() == s.data() );         // same buffer

            sp = u.collect();

            assert( sp->data() != s.data() );       // different buffer
            assert( sp->encoded_url() == s);        // same contents

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
        url_view u = parse_uri( "http://www.example.com" ).value();

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
        assert( parse_uri( "http://www.example.com" ).scheme() == "http" );
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
        assert( parse_uri( "http://www.example.com/index.htm" ).value().has_authority() == true );

        assert( parse_relative_ref( "//" ).value().has_authority() == true );

        assert( parse_relative_ref( "/file.txt" ).value().has_authority() == false );
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
    BOOST_URL_DECL
    bool
    has_authority() const noexcept;

    /** Return the authority.

        This function returns the authority as a
        percent-encoded string.

        @par Example
        @code
        assert( parse_uri( "http://www.example.com/index.htm" ).encoded_authority() == "www.example.com" );
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

    //--------------------------------------------

    /** Return true if a userinfo is present.

        This function returns true if this
        contains a userinfo.

        @par Example
        @code
        url_view u = parse_uri( "http://user@example.com" ).value();

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
        assert( parse_uri( "http://user:pass@example.com" ).encoded_userinfo() == "user:pass" );
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
        string with percent-decoding applied, using
        the optionally specified allocator.

        @par Example
        @code
        url_view u = parse_uri( "http://user:pass@example.com" ).value();

        assert( u.userinfo() == "user:pass" );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        authority   = [ userinfo "@" ] host [ ":" port ]
        @endcode

        @par Exception Safety
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used.

        @return A @ref string_value using the
        specified allocator.

        @par Specification
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref has_userinfo,
            @ref encoded_userinfo.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_value
    userinfo(
        Allocator const& a = {}) const
    {
        pct_decode_opts opt;
        opt.plus_to_space = false;
        return pct_decode_unchecked(
            encoded_userinfo(), opt, a);
    }

    //--------------------------------------------

    /** Return the user

        This function returns the user portion of
        the userinfo as a percent-encoded string.

        @par Example
        @code
        url_view u = parse_uri( "http://user:pass@example.com" ).value();

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
    encoded_user() const noexcept;

    /** Return the user

        This function returns the user portion of
        the userinfo as a string with percent-decoding
        applied, using the optionally specified
        allocator.

        @par Example
        @code
        assert( parse_uri( "http://user:pass@example.com" ).user() == "user" );
        @endcode

        @par BNF
        @code
        userinfo    = user [ ":" [ password ] ]

        user        = *( unreserved / pct-encoded / sub-delims )
        password    = *( unreserved / pct-encoded / sub-delims / ":" )
        @endcode

        @par Exception Safety
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used.

        @return A @ref string_value using the
        specified allocator.

        @par Specification
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_password,
            @ref encoded_user,
            @ref has_password,
            @ref password.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_value
    user(
        Allocator const& a = {}) const
    {
        pct_decode_opts opt;
        opt.plus_to_space = false;
        return pct_decode_unchecked(
            encoded_user(), opt, a);
    }

    /** Return true if this contains a password

        This function returns true if the userinfo
        contains a password (which may be empty).

        @par Example
        @code
        assert( parse_uri( "http://user@example.com" ).value().has_password() == false );

        assert( parse_uri( "http://user:pass@example.com" ).value().has_password() == true );

        assert( parse_uri( "http://:@" ).value().has_password() == true );
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
        url_view u = parse_uri( "http://user:pass@example.com" ).value();

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
        userinfo with percent-decoding applied,
        using the optionally specified allocator.

        @par Exception Safety
        Calls to allocate may throw.

        @param a An allocator to use for the string.
        If this parameter is omitted, the default
        allocator is used, and the return type of
        the function becomes `std::string`.

        @return A @ref string_value using the
        specified allocator.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.1"
            >3.2.1. User Information (rfc3986)</a>

        @see
            @ref encoded_password,
            @ref encoded_user,
            @ref has_password,
            @ref password.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_value
    password(
        Allocator const& a = {}) const
    {
        pct_decode_opts opt;
        opt.plus_to_space = false;
        return pct_decode_unchecked(
            encoded_password(), opt, a);
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
        assert( parse_relative_ref( "/favicon.png" ).value().host_type() == host_type::none );

        assert( parse_uri( "http://example.com" ).value().host_type() == host_type::name );

        assert( parse_uri( "http://192.168.0.1" ).value().host_type() == host_type::ipv4 );
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
        return host_type_;
    }

    /** Return the host

        This function returns the host portion of
        the authority as a percent-encoded string,

        @par Example
        @code
        assert( parse_relative_ref( "/favicon.png" ).value().encoded_host() == "" );

        assert( parse_uri( "http://example.com" ).value().encoded_host() == "example.com" );

        assert( parse_uri( "http://192.168.0.1" ).value().encoded_host() == "192.168.0.1" );
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
        applied, using the optionally specified
        allocator.

        @par Example
        @code
        assert( parse_relative_ref( "/favicon.png" ).value().host() == "" );

        assert( parse_uri( "http://example.com" ).value().host() == "example.com" );

        assert( parse_uri( "http://192.168.0.1" ).value().host() == "192.168.0.1" );
        @endcode

        @par BNF
        @code
        host        = IP-literal / IPv4address / reg-name

        IP-literal  = "[" ( IPv6address / IPvFuture  ) "]"

        IPvFuture   = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )

        reg-name    = *( unreserved / pct-encoded / "-" / ".")
        @endcode

        @par Exception Safety
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A @ref string_value using the
        specified allocator.

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
    template<
        class Allocator =
            std::allocator<char>>
    string_value
    host(
        Allocator const& a = {}) const
    {
        auto const s0 =
            encoded_host();
        if(host_type_ !=
            urls::host_type::name)
        {
            // no decoding
            return string_value(s0, a);
        }
        pct_decode_opts opt;
        opt.plus_to_space = false;
        return pct_decode_unchecked(
            s0, opt, a, decoded_[id_host]);
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
    ipv_future() const noexcept;

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

    /** Return the path

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
        return get(id_path);
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
            encoded_path(), nseg_);
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
    template<class Allocator =
        std::allocator<char>>
    segments_view
    segments(Allocator const& alloc = {}) const noexcept
    {
        return segments_view(
            encoded_path(), nseg_, alloc);
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
        string with percent-decoding applied,
        using the optionally specified allocator.

        @par BNF
        @code
        query           = *( pchar / "/" / "?" )

        query-part      = [ "?" query ]
        @endcode

        @par Exception Safety
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A @ref string_value using the
        specified allocator.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
            >3.4. Query (rfc3986)</a>

        @see
            @ref encoded_query,
            @ref has_query.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_value
    query(
        Allocator const& a = {}) const
    {
        pct_decode_opts opt;
        opt.plus_to_space = true;
        return pct_decode_unchecked(
            encoded_query(), opt, a);
    }

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

        @param alloc The allocator the container
        will use when returning string with
        percent-decoding applied.

        @endcode
    */
    template<
        class Allocator =
            std::allocator<char>>
    params_view
    params(Allocator const&
        alloc = {}) const noexcept;

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
        fragment        = *( pchar / "/" / "?" )

        fragment-part   = [ "#" fragment ]
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

        fragment-part   = [ "#" fragment ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.5"
            >3.5. Fragment (rfc3986)</a>

        @see
            @ref fragment,
            @ref has_fragment.
    */
    BOOST_URL_DECL
    string_view
    encoded_fragment() const noexcept;

    /** Return the fragment.

        This function returns the fragment as a
        string with percent-decoding applied,
        using the optionally specified allocator.

        @par BNF
        @code
        fragment        = *( pchar / "/" / "?" )

        fragment-part   = [ "#" fragment ]
        @endcode

        @par Exception Safety
        Calls to allocate may throw.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.5"
            >3.5. Fragment (rfc3986)</a>

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used.

        @return A @ref string_value using the
        specified allocator.

        @see
            @ref encoded_fragment,
            @ref has_fragment.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_value
    fragment(
        Allocator const& a = {}) const
    {
        pct_decode_opts opt;
        opt.plus_to_space = false;
        return pct_decode_unchecked(
            encoded_fragment(),
            opt, a, decoded_[id_frag]);
    }

    //--------------------------------------------
    //
    // Resolution
    //
    //--------------------------------------------

    /** Return true if the URL satisfies the absolute-uri grammar.
    */
    BOOST_URL_DECL
    bool
    is_absolute_uri() const noexcept;

    //--------------------------------------------
    //
    // Parsing
    //
    //--------------------------------------------

    BOOST_URL_DECL friend result<url_view>
        parse_absolute_uri(string_view s) noexcept;
    BOOST_URL_DECL friend result<url_view>
        parse_relative_ref(string_view s) noexcept;
    BOOST_URL_DECL friend result<url_view>
        parse_uri(string_view s) noexcept;
    BOOST_URL_DECL friend result<url_view>
        parse_uri_reference(string_view s) noexcept;

private:
    void apply(scheme_part_bnf const& t) noexcept;
    void apply(host_bnf const& h) noexcept;
    void apply(authority_bnf const& t) noexcept;
    void apply(parsed_path const& path) noexcept;
    void apply(query_part_bnf const& t) noexcept;
    void apply(fragment_part_bnf const& t) noexcept;
};

//------------------------------------------------

/** Parse an absolute-URI

    This function parses a string according
    to the absolute-URI grammar below, and
    returns a @ref url_view referencing the string.
    Ownership of the string is not transferred;
    the caller is responsible for ensuring that
    the lifetime of the string extends until the
    view is no longer being accessed.

    @par BNF
    @code
    absolute-URI    = scheme ":" hier-part [ "?" query ]

    hier-part       = "//" authority path-abempty
                    / path-absolute
                    / path-rootless
                    / path-empty
    @endcode

    @par Exception Safety
    Throws nothing.

    @return A view to the parsed URL

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.3"
        >4.3. Absolute URI (rfc3986)</a>

    @see
        @ref parse_absolute_uri,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref parse_uri_reference,
        @ref url_view.
*/
BOOST_URL_DECL
result<url_view>
parse_absolute_uri(
    string_view s) noexcept;

/** Parse a URI

    This function parses a string according
    to the URI grammar below, and returns a
    @ref url_view referencing the string.
    Ownership of the string is not transferred;
    the caller is responsible for ensuring that
    the lifetime of the string extends until the
    view is no longer being accessed.

    @par BNF
    @code
    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

    hier-part     = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty
    @endcode

    @par Exception Safety
    Throws nothing.

    @return A view to the parsed URL

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3"
        >3. Syntax Components (rfc3986)</a>

    @see
        @ref parse_absolute_uri,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref parse_uri_reference,
        @ref url_view.
*/
BOOST_URL_DECL
result<url_view>
parse_uri(
    string_view s) noexcept;

/** Parse a relative-ref

    This function parses a string according
    to the relative-ref grammar below, and
    returns a @ref url_view referencing the string.
    Ownership of the string is not transferred;
    the caller is responsible for ensuring that
    the lifetime of the string extends until the
    view is no longer being accessed.

    @par BNF
    @code
    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]

    relative-part = "//" authority path-abempty
                  / path-absolute
                  / path-noscheme
                  / path-abempty
                  / path-empty
    @endcode

    @par Exception Safety
    Throws nothing.

    @return A view to the parsed URL

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.2
        >4.2. Relative Reference (rfc3986)</a>
    @li <a href="https://www.rfc-editor.org/errata/eid5428"
        >Errata ID: 5428 (rfc3986)</a>

    @see
        @ref parse_absolute_uri,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref parse_uri_reference,
        @ref url_view.
*/
BOOST_URL_DECL
result<url_view>
parse_relative_ref(
    string_view s) noexcept;

/** Parse a URI-reference

    This function parses a string according
    to the URI-reference grammar below, and
    returns a @ref url_view referencing the string.
    Ownership of the string is not transferred;
    the caller is responsible for ensuring that
    the lifetime of the string extends until the
    view is no longer being accessed.

    @par BNF
    @code
    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

    hier-part     = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty

    URI-reference = URI / relative-ref

    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]

    relative-part = "//" authority path-abempty
                  / path-absolute
                  / path-noscheme
                  / path-abempty
                  / path-empty
    @endcode

    @par Exception Safety
    Throws nothing.

    @return A view to the parsed URL

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.1"
        >4.1. URI Reference (rfc3986)</a>
    @li <a href="https://www.rfc-editor.org/errata/eid5428"
        >Errata ID: 5428 (rfc3986)</a>

    @see
        @ref parse_absolute_uri,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref parse_uri_reference,
        @ref url_view.
*/
BOOST_URL_DECL
result<url_view>
parse_uri_reference(
    string_view s) noexcept;

//------------------------------------------------

/** Format the encoded URL to the output stream

    This function serializes the encoded URL
    to the output stream.

    @par Example
    @code
    url_view u = parse_uri( "http://www.example.com/index.htm" );

    std::cout << u << std::endl;
    @endcode

    @return A reference to the output stream, for chaining

    @param os The output stream to write to

    @param u The URL to write
*/
BOOST_URL_DECL
std::ostream&
operator<<(
    std::ostream& os,
    url_view const& u);

} // urls
} // boost

// These includes are here
// because of circular dependencies
#include <boost/url/impl/params_view.hpp>
#include <boost/url/impl/params_encoded_view.hpp>

#include <boost/url/impl/url_view.hpp>

#endif
