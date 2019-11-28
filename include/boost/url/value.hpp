//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_VALUE_HPP
#define BOOST_URL_VALUE_HPP

#include <boost/url/config.hpp>
#include <boost/url/view.hpp>
#include <boost/url/detail/char_type.hpp>
#include <cstdint>
#include <cstring>
#include <new>
#include <stdexcept>

namespace boost {
namespace url {

/*

URL:    scheme    authority        path        query        fragment
        foo:   // example.com:8042 /over/there ?name=ferret #nose
        urn:                       example:animal:ferret:nose
*/

class value : public view
{
    char* begin_ = nullptr;
    size_type* end_ = nullptr;
    std::size_t capacity_ = 0;

public:
    using view::domain;
    using view::subdomain;
    using view::tld;
    using view::path;
    using view::directory;
    using view::filename;
    using view::suffix;
    using view::segment;
    using view::segment_coded;
    using view::search;
    using view::query;
    using view::fragment;
    using view::resource;

    value() = default;

    BOOST_URL_DECL
    ~value();

    /** Set the scheme.

        No escapes, only [a-z0-9.+-] are valid.
    */
    BOOST_URL_DECL
    value&
    set_scheme(string_view s);

    /** Set the username.

        The username is 
        The username may not include a colon.
    */
    BOOST_URL_DECL
    value&
    set_username(
        string_view s);

    /** Set the encoded username.
    */
    BOOST_URL_DECL
    value&
    set_encoded_username(
        string_view s);

    /** Set the password.
    */
    BOOST_URL_DECL
    value&
    set_password(
        string_view s);

    /** Set the encoded password.
    */
    BOOST_URL_DECL
    value&
    set_encoded_password(
        string_view s);

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
    value&
    set_encoded_userinfo(
        string_view s);

    BOOST_URL_DECL
    value&
    set_hostname(
        string_view s);

    BOOST_URL_DECL
    value&
    set_encoded_hostname(
        string_view s);

    BOOST_URL_DECL
    value&
    set_port(
        optional<unsigned short> num);

    BOOST_URL_DECL
    value&
    set_port_string(string_view s);

    BOOST_URL_DECL
    value&
    set_encoded_host(
        string_view s);

    BOOST_URL_DECL
    value&
    set_encoded_authority(
        string_view s);

    // ?
    BOOST_URL_DECL
    value&
    origin(string_view s);

    BOOST_URL_DECL
    value&
    set_domain(string_view s);

    BOOST_URL_DECL
    value&
    set_subdomain(string_view s);

    BOOST_URL_DECL
    value&
    set_tld(string_view s);

    BOOST_URL_DECL
    value&
    set_path(string_view s);

    BOOST_URL_DECL
    value&
    set_directory(string_view s);

    BOOST_URL_DECL
    value&
    set_filename(string_view s);

    BOOST_URL_DECL
    value&
    set_suffix(string_view s);

    BOOST_URL_DECL
    value&
    set_segment(
        int pos,
        string_view s);

    BOOST_URL_DECL
    value&
    set_search(
        string_view s);

    BOOST_URL_DECL
    value&
    set_query(
        string_view s);

    BOOST_URL_DECL
    value&
    set_hash(
        string_view s);

    BOOST_URL_DECL
    value&
    set_fragment(
        string_view s);

    BOOST_URL_DECL
    value&
    set_resource(
        string_view s);

    /** Normalize everything.
    */
    BOOST_URL_DECL
    value&
    normalize();

    BOOST_URL_DECL
    value&
    normalize_scheme();

private:
    using view::offset;

    struct write_parts;

    static
    std::size_t
    max_size() noexcept
    {
        return size_type(-1);
    }

    inline
    size_type&
    offset(id_type id) noexcept;

    inline
    std::size_t
    size() const noexcept;

    inline
    std::size_t
    capacity() const noexcept;

    inline
    void
    merge(
        id_type first,
        id_type last) noexcept;

    inline
    void
    split(
        id_type id,
        std::size_t n) noexcept;

    inline
    void
    split_suffix(
        id_type id,
        std::size_t n) noexcept;

    inline
    void
    maybe_init();

    inline
    void
    reserve(
        std::size_t char_capacity,
        id_type offset_capacity);

    inline
    void
    reserve(
        std::size_t char_capacity);

    inline
    void
    realloc(size_type capacity);

    inline
    char*
    resize(
        id_type id,
        std::size_t needed);

    inline
    void
    set(
        id_type id,
        string_view s);

    inline
    void
    set_encoded(id_type id,
        string_view s,
        detail::pct_encoding e);

    inline
    void
    erase(id_type id) noexcept;
};

} // url
} // boost

#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/value.ipp>
#endif

#endif
