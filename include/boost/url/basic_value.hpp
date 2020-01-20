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

class basic_value
{
    friend class view::segments_type;
    friend class view::params_type;

    detail::storage& a_;
    detail::parts pt_;
    char* s_ = nullptr;

protected:
    BOOST_URL_DECL
    basic_value(
        detail::storage& a);

    BOOST_URL_DECL
    basic_value(
        detail::storage& a,
        string_view s);

public:
    class segments_type;
    class params_type;

    /** Return the number of characters in the URL.
    */
    std::size_t
    size() const noexcept
    {
        return pt_.offset[
            detail::id_end];
    }

    /** Return a pointer to the characters in the URL.

        The string is null terminated.
    */
    char const*
    data() const noexcept
    {
        if(s_)
            return s_;
        return "";
    }

    /** Return the number of characters that may be stored without a reallocation.
    */
    std::size_t
    capacity() const noexcept
    {
        return a_.capacity();
    }

    //------------------------------------------------------

    /** Return the complete serialized URL.
    */
    BOOST_URL_DECL
    string_view
    encoded_href() const;

    /** Return the origin.
    */
    BOOST_URL_DECL
    string_view
    encoded_origin() const noexcept;

    BOOST_URL_DECL
    basic_value&
    set_encoded_uri_reference(
        string_view s);

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
    */
    BOOST_URL_DECL
    string_view
    scheme() const noexcept;

    /** Set the scheme.

        No escapes, only [a-z0-9.+-] are valid.
    */
    BOOST_URL_DECL
    basic_value&
    set_scheme(string_view s);

    //------------------------------------------------------
    //
    // authority
    //
    //------------------------------------------------------

    /** Return the encoded authority.
    */
    BOOST_URL_DECL
    string_view
    encoded_authority() const noexcept;

    BOOST_URL_DECL
    basic_value&
    set_encoded_authority(
        string_view s);

    //
    // userinfo
    //

    /** Return the userinfo.
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
    */
    BOOST_URL_DECL
    basic_value&
    set_encoded_userinfo(
        string_view s);

    /** Return the username.
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
    */
    host_type
    host() const noexcept
    {
        return pt_.host;
    }

    /** Return the encoded host.
    */
    BOOST_URL_DECL
    string_view
    encoded_host() const noexcept;

    BOOST_URL_DECL
    basic_value&
    set_encoded_host(
        string_view s);

    /** Return the hostname.
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
    */
    BOOST_URL_DECL
    string_view
    encoded_hostname() const noexcept;

    BOOST_URL_DECL
    basic_value&
    set_hostname(
        string_view s);

    BOOST_URL_DECL
    basic_value&
    set_encoded_hostname(
        string_view s);

    /** Return the port, if any.
    */
    optional<unsigned short>
    port() const noexcept
    {
        return pt_.port;
    }

    /** Return the port as a string, or "" if no port.
    */
    BOOST_URL_DECL
    string_view
    port_string() const noexcept;

    BOOST_URL_DECL
    basic_value&
    set_port(
        optional<unsigned short> num);

    BOOST_URL_DECL
    basic_value&
    set_port_string(string_view s);

    //------------------------------------------------------
    //
    // path
    //
    //------------------------------------------------------

    /** Return `true` if this is a relative-ref.
    */
    BOOST_URL_DECL
    bool
    is_relative() const noexcept;

    /** Return the encoded path.
    */
    BOOST_URL_DECL
    string_view
    encoded_path() const noexcept;

    BOOST_URL_DECL
    basic_value&
    set_encoded_path(
        string_view s);

    /** Return the path segments as a read-only container.
    */
    inline
    view::segments_type
    segments() const noexcept;

    /** Return the path segments as a modifiable container.
    */
    inline
    segments_type
    segments() noexcept;

    //------------------------------------------------------
    //
    // query
    //
    //------------------------------------------------------

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

    BOOST_URL_DECL
    string_view
    encoded_query() const noexcept;

    /** Return the query parameters as a read-only container.
    */
    inline
    view::params_type
    params() const noexcept;

    /** Return the query parameters as a modifiable container.
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
    */
    BOOST_URL_DECL
    string_view
    encoded_fragment() const noexcept;

    BOOST_URL_DECL
    basic_value&
    set_fragment(
        string_view s);

    BOOST_URL_DECL
    basic_value&
    set_encoded_fragment(
        string_view s);

    //------------------------------------------------------
    //
    // normalization
    //
    //------------------------------------------------------

    /** Normalize everything.
    */
#if 0
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

    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    key(Allocator const& a = {}) const
    {
        return detail::decode(
            encoded_key(), a);
    }

    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    basic_value(Allocator const& a = {}) const
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
        return { key(), basic_value() };
    }
};

} // url
} // boost

#include <boost/url/impl/basic_value.hpp>
#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/basic_value.ipp>
#endif

#endif
