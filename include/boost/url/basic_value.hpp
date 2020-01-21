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
    host_type
    host() const noexcept
    {
        return pt_.host;
    }

    /** Return the encoded host.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_host() const noexcept;

    BOOST_URL_DECL
    basic_value&
    set_encoded_host(
        string_view s);

    /** Return the hostname.

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
    hostname(
        Allocator const& a = {}) const
    {
        return detail::decode(
            encoded_hostname(), a);
    }

    /** Return the encoded hostname.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_hostname() const noexcept;

    /** Set the hostname.
    */
    BOOST_URL_DECL
    basic_value&
    set_hostname(
        string_view s);

    /** Set the hostname.
    */
    BOOST_URL_DECL
    basic_value&
    set_encoded_hostname(
        string_view s);

    /** Return the port.

        @par Exception Safety

        No-throw guarantee.

        @return An optional value with the port
        number if the port is set, otherwise the
        nullopt value.
    */
    optional<unsigned short>
    port() const noexcept
    {
        return pt_.port;
    }

    /** Return the port.

        If present, the port is returned as a string.
        Otherwise, the empty string is returned.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    port_string() const noexcept;

    BOOST_URL_DECL
    basic_value&
    set_port(
        optional<unsigned short> num);

    /** Set the port.

        This function sets the port to the given
        string, which must represent a valid unsigned
        integer in the range of values allowed for
        ports (0 to 65535).

    */
    BOOST_URL_DECL
    basic_value&
    set_port_string(string_view s);

    //------------------------------------------------------
    //
    // path
    //
    //------------------------------------------------------

    /** Return `true` if this is a relative-ref.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    bool
    is_relative() const noexcept;

    /** Return the path.

        If the URL has a path, it is returned in
        encoded form. Otherwise, an empty string
        is returned.

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

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The encoded oath. If this string
        is not syntactically correct, an exception is
        thrown.

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

        If the URL has a query, it is returned in
        decoded form, with a leading question mark ('?').
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

        If the URL has a query, it is returned in
        encoded form, with a leading question mark ('?').
        Otherwise, an empty string is returned.

        @par Exception Safety

        No-throw guarantee.

        @return The encoded query.
    */
    BOOST_URL_DECL
    string_view
    encoded_query() const noexcept;

    /** Set the query.

        Sets the query of the URL to the specified
        plain string:

        @li If the string is empty, any existing query
        is removed including the leading question mark ('?'),
        otherwise:

        @li If the string starts with a question mark it
        is first removed. Then the query is set to the string.
        The URL will always include at least a question mark
        if the passed string is not empty.

        Any special or reserved characters in the
        passed string are automatically percent-encoded
        into the URL's query.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The query. This string may contain
        any characters, including nulls.
    */
    BOOST_URL_DECL
    basic_value&
    set_query(
        string_view s);

    /** Set the query.

        Sets the query of the URL to the specified
        encoded string:

        @li If the string is empty, any existing query
        is removed including the leading question mark ('?'),
        otherwise:

        @li If the string starts with a question mark it
        is first removed. Then the query is set to the string.
        The URL will always include at least a question mark
        if the passed string is not empty.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The encoded query. If this string
        is not syntactically correct, an exception is
        thrown.

        @throws std::exception invalid query.
    */
    BOOST_URL_DECL
    basic_value&
    set_encoded_query(
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

        If the URL has a fragment, it is returned in
        decoded form, with a leading hash mark ('#').
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

        If the URL has a fragment, it is returned in
        encoded form, with a leading hash mark ('#').
        Otherwise, an empty string is returned.

        @par Exception Safety

        No-throw guarantee.

        @return The encoded fragment.
    */
    BOOST_URL_DECL
    string_view
    encoded_fragment() const noexcept;

    /** Set the fragment.

        Sets the fragment of the URL to the specified
        plain string:

        @li If the string is empty, the fragment is
        cleared including the leading hash mark ('#'),
        otherwise:
        
        @li If the string starts with a hash mark it
        is first removed. Then the fragment is set to
        the string. The URL will always include at
        least a hash mark if the passed string is
        not empty.

        Any special or reserved characters in the
        passed string are automatically percent-encoded
        into the URL's fragment.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The fragment. This string may contain
        any characters, including nulls.
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
        
        @li If the string starts with a hash mark it
        is first removed. Then the fragment is set to
        the string. The URL will always include at
        least a hash mark if the passed string is
        not empty.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param s The encoded fragment. If this string
        is not syntactically correct, an exception is
        thrown.

        @throws std::exception invalid fragment.
    */
    BOOST_URL_DECL
    basic_value&
    set_encoded_fragment(
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
