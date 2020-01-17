//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_VIEW_HPP
#define BOOST_URL_VIEW_HPP

#include <boost/url/config.hpp>
#include <boost/url/detail/parts.hpp>
#include <memory>
#include <string>
#include <utility>

namespace boost {
namespace url {

class view
{
    char const* s_ = "";
    detail::parts pt_;

public:
    class segments_type;
    class params_type;

    view() = default;

    BOOST_URL_DECL
    explicit
    view(string_view s);

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

    //
    // userinfo
    //

    /** Return the userinfo.
    */
    BOOST_URL_DECL
    string_view
    encoded_userinfo() const noexcept;

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

    //------------------------------------------------------
    //
    // host
    //
    //------------------------------------------------------

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

    /** Return the path segments as a read-only container.
    */
    inline
    segments_type
    segments() const noexcept;

    //------------------------------------------------------
    //
    // query
    //
    //------------------------------------------------------

    template<
        class Allocator =
            std::allocator<char>>
    BOOST_URL_DECL
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
    params_type
    params() const noexcept;

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
    BOOST_URL_DECL
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
};

//----------------------------------------------------------

/** A read-only view to the path segments.
*/
class view::segments_type
{
    view const* v_ = nullptr;

public:
    class value_type;
    class iterator;
    using const_iterator = iterator;

    segments_type() = default;
    segments_type(segments_type const&) = default;
    segments_type& operator=(
        segments_type const&) = default;

    explicit
    segments_type(view const& v)
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

class view::segments_type::value_type
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

/** A read-only view to the URL query parameters.
*/
class view::params_type
{
    view const* v_ = nullptr;

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
    params_type(view const& v)
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
    BOOST_URL_DECL
    string_type<Allocator>
    at( string_view key,
        Allocator const& a = {}) const;
};

//----------------------------------------------------------

class view::params_type::value_type
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

#include <boost/url/impl/view.hpp>
#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/view.ipp>
#endif

#endif
