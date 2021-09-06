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
#include <boost/url/query_params_view.hpp>
#include <boost/url/optional.hpp>
#include <boost/url/detail/parts.hpp>
#include <boost/url/detail/char_type.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace boost {
namespace urls {

class url;

/** A parsed reference to a URL string.
*/
class url_view
{
    char const* s_ = "";
    detail::parts pt_;

    struct shared_impl;

    url_view(
        char const* s,
        detail::parts const& pt) noexcept
        : s_(s)
        , pt_(pt)
    {
    }

    // shortcuts
    string_view get(
        detail::part id) const noexcept;
    std::size_t len(
        detail::part id) const noexcept;

public:
    class segments_type;

    /** Constructor
    */
    // VFALCO DEPRECATED
    url_view() noexcept = default;

    /** Constructor

        @param s The string to construct from.
    */
    // VFALCO DEPRECATED
    BOOST_URL_DECL
    explicit
    url_view(string_view s);

    /** Return the view as a self-contained shared object
    */
    BOOST_URL_DECL
    std::shared_ptr<url_view>
    make_shared() const;

    //------------------------------------------------------

    /** Return the complete serialized URL
    */
    BOOST_URL_DECL
    string_view
    encoded_url() const;

    /** Return the origin
    */
    BOOST_URL_DECL
    string_view
    encoded_origin() const noexcept;

    //------------------------------------------------------
    //
    // scheme
    //
    //------------------------------------------------------

    /** Return true if a scheme exists
    */
    BOOST_URL_DECL
    bool
    has_scheme() const noexcept;

    /** Return the scheme
    */
    BOOST_URL_DECL
    string_view
    scheme() const noexcept;

    //------------------------------------------------------
    //
    // authority
    //
    //------------------------------------------------------

    /** Return true if an authority exists

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

    /** Return the authority if it exists, or an empty string

        @see has_authority
    */
    BOOST_URL_DECL
    string_view
    encoded_authority() const noexcept;

    /** Return true if a userinfo exists

        This function returns `true` if there are
        any characters in the URL's userinfo, including
        the at sign ('@') separator.
    */
    BOOST_URL_DECL
    bool
    has_userinfo() const noexcept;

    /** Return the userinfo if it exists, or an empty string

        Returns the userinfo of the URL as an encoded
        string. The userinfo includes the username and
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
        return detail::decode(
            encoded_userinfo(), a);
    }

    /** Return true if a username exists
    */
    BOOST_URL_DECL
    bool
    has_username() const noexcept;

    /** Return the username if it exists, or an empty string

        This function returns the username portion of
        the userinfo if present, as an encoded string.
        The username portion is defined by all of the
        characters in the userinfo up to but not
        including the first colon (':"), or the
        entire userinfo if no colon is present.

        @par Exception Safety

        No-throw guarantee.
    */
    BOOST_URL_DECL
    string_view
    encoded_username() const noexcept;

    /** Return the username if it exists, or an empty string

        This function returns the username portion of
        the userinfo if present, as a decoded string.
        The username portion is defined by all of the
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
    username(
        Allocator const& a = {}) const
    {
        return detail::decode(
            encoded_username(), a);
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
        return detail::decode(
            encoded_password(), a);
    }

    //--------------------------------------------

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

    //------------------------------------------------------
    //
    // path
    //
    //------------------------------------------------------

    /** Return the encoded path
    */
    BOOST_URL_DECL
    string_view
    encoded_path() const noexcept;

    /** Return the path segments as a read-only range
    */
    inline
    segments_type
    segments() const noexcept;

    //------------------------------------------------------
    //
    // query
    //
    //------------------------------------------------------

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
        return detail::decode(
            encoded_query(), a);
    }

    /** Return the query parameters as a read-only container.
    */
    BOOST_URL_DECL
    query_params_view
    query_params() const noexcept;

    //------------------------------------------------------
    //
    // fragment
    //
    //------------------------------------------------------

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
        return detail::decode(
            encoded_fragment(), a);
    }

    //--------------------------------------------
    //
    // free functions
    //
    //--------------------------------------------

    /** Parse a string using the URI grammar

        @par BNF
        @code
        URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
        @endcode
    */
    BOOST_URL_DECL
    friend
    optional<url_view>
    parse_uri(
        string_view s,
        error_code& ec) noexcept;

    BOOST_URL_DECL
    friend
    url_view
    parse_uri(
        string_view s);

    /** Parse a string using the absolute-URI grammar

        @par BNF
        @code
        absolute-URI  = scheme ":" hier-part [ "?" query ]
        @endcode
    */
    BOOST_URL_DECL
    friend
    optional<url_view>
    parse_relative_ref(
        string_view s,
        error_code& ec) noexcept;

    BOOST_URL_DECL
    friend
    url_view
    parse_relative_ref(
        string_view s);
};

//------------------------------------------------

BOOST_URL_DECL
optional<url_view>
parse_uri(
    string_view s,
    error_code& ec) noexcept;

BOOST_URL_DECL
url_view
parse_uri(
    string_view s);

BOOST_URL_DECL
optional<url_view>
parse_relative_ref(
    string_view s,
    error_code& ec) noexcept;

BOOST_URL_DECL
url_view
parse_relative_ref(
    string_view s);

//----------------------------------------------------------

/** A read-only view to the path segments.
*/
class url_view::segments_type
{
    char const* s_ = nullptr;
    detail::parts const* pt_ = nullptr;

public:
    class value_type;
    class iterator;
    using const_iterator = iterator;

    segments_type() = default;
    segments_type(segments_type const&) = default;
    segments_type& operator=(
        segments_type const&) = default;

    explicit
    segments_type(url_view const& v) noexcept
        : s_(v.s_)
        , pt_(&v.pt_)
    {
    }

    inline
    explicit
    segments_type( url const& v) noexcept;

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
};

//----------------------------------------------------------

class url_view::segments_type::value_type
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

class url_view::segments_type::iterator
{
    friend segments_type;

    char const* s_;
    detail::parts const* pt_;
    std::size_t off_;
    std::size_t n_;

    BOOST_URL_DECL
    iterator(
        segments_type const* v,
        bool end) noexcept;

public:
    using value_type =
        segments_type::value_type;

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

#include <boost/url/impl/url_view.hpp>

#endif
