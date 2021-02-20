//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_URL_VIEW_HPP
#define BOOST_URL_URL_VIEW_HPP

#include <boost/url/config.hpp>
#include <boost/url/detail/parts.hpp>
#include <boost/url/detail/char_type.hpp>
#include <memory>
#include <string>
#include <utility>

namespace boost {
namespace urls {

class url_base;

/** A parsed reference to a URL string.
*/
class url_view
{
    detail::parts_view pt_;

public:
    class segments_type;
    class params_type;

    url_view() = default;

    /** Constructor.

        @param s The string to construct from.
    */
    BOOST_URL_DECL
    explicit
    url_view(string_view s);

    /** Return the number of characters in the URL.
    */
    std::size_t
    size() const noexcept
    {
        return pt_.length_all();
    }

    /** Return a pointer to the characters in the URL.
    */
    char const*
    data() const noexcept
    {
        return pt_.data();
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
    segments_type
    segments() const noexcept;

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
    params_type
    params() const noexcept;

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
};

//----------------------------------------------------------

/** A read-only view to the path segments.
*/
class url_view::segments_type
{
    detail::parts_view const* pt_ = nullptr;

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
        : pt_(&v.pt_)
    {
    }

    inline
    explicit
    segments_type( url_base const& v) noexcept;

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

    detail::parts_view const* pt_;
    std::size_t off_;
    std::size_t n_;

    BOOST_URL_DECL
    iterator(
        detail::parts_view const* v,
        bool end) noexcept;

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

/** A read-only view to the URL query parameters.
*/
class url_view::params_type
{
    detail::parts_view const* pt_ = nullptr;

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
    params_type(url_view const& v)
        : pt_(&v.pt_)
    {
    }

    inline
    explicit
    params_type(
        url_base const& v) noexcept;

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

    template<class Allocator =
        std::allocator<char>>
    string_type<Allocator>
    at( string_view key,
        Allocator const& a = {}) const;
};

//----------------------------------------------------------

class url_view::params_type::value_type
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

class url_view::params_type::iterator
{
    friend params_type;

    detail::parts_view const* pt_;
    std::size_t off_;
    std::size_t nk_;
    std::size_t nv_;

    BOOST_URL_DECL
    iterator(
        detail::parts_view const* v,
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

} // urls
} // boost

#include <boost/url/impl/url_view.hpp>
#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/url_view.ipp>
#endif

#endif
