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

class value : public view
{
    char* begin_ = nullptr;
    size_type* end_ = nullptr;
    std::size_t capacity_ = 0;

public:
    value() = default;

    BOOST_URL_DECL
    explicit
    value(string_view s);

    BOOST_URL_DECL
    ~value();

    //------------------------------------------------------

    BOOST_URL_DECL
    value&
    set_encoded_uri_reference(
        string_view s);

    BOOST_URL_DECL
    value&
    set_encoded_origin(
        string_view s);

    //------------------------------------------------------
    //
    // scheme
    //
    //------------------------------------------------------

    /** Set the scheme.

        No escapes, only [a-z0-9.+-] are valid.
    */
    BOOST_URL_DECL
    value&
    set_scheme(string_view s);

    //------------------------------------------------------
    //
    // authority
    //
    //------------------------------------------------------

    BOOST_URL_DECL
    value&
    set_encoded_authority(
        string_view s);

    //
    // userinfo
    //

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

    /** Set the username.

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

    //
    // host
    //

    BOOST_URL_DECL
    value&
    set_encoded_host(
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

    //------------------------------------------------------
    //
    // path
    //
    //------------------------------------------------------

    BOOST_URL_DECL
    value&
    set_encoded_path(
        string_view s);

    BOOST_URL_DECL
    value&
    set_segment(
        int index,
        string_view s);

    BOOST_URL_DECL
    value&
    set_encoded_segment(
        int index,
        string_view s);

    //------------------------------------------------------
    //
    // query
    //
    //------------------------------------------------------

    //------------------------------------------------------
    //
    // fragment
    //
    //------------------------------------------------------

    BOOST_URL_DECL
    value&
    set_fragment(
        string_view s);

    BOOST_URL_DECL
    value&
    set_encoded_fragment(
        string_view s);

    //------------------------------------------------------
    //
    // normalization
    //
    //------------------------------------------------------

    /** Normalize everything.
    */
    BOOST_URL_DECL
    value&
    normalize();

    BOOST_URL_DECL
    value&
    normalize_scheme();

    //------------------------------------------------------

private:
    using view::offset;

    // Returns the upper limit on the
    // number of allocated bytes.
    static constexpr std::size_t max_size() noexcept;

    inline std::size_t size() const noexcept;
    inline std::size_t capacity() const noexcept;
    inline size_type& offset(id_type id) noexcept;
    inline void null_term() noexcept;
    inline void merge(id_type first, id_type last) noexcept;
    inline char* resize_and_merge(
        id_type first, id_type last, std::size_t nchar, std::size_t nid);
    inline void split(id_type id, std::size_t n) noexcept;
    inline void maybe_init();
    inline void reserve(std::size_t char_cap, std::size_t id_cap);
    inline void reserve(std::size_t char_cap);
    inline char* resize(id_type id, std::size_t needed);
    inline void erase(id_type id) noexcept;
    inline void erase(id_type first, id_type last) noexcept;
};

} // url
} // boost

#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/value.ipp>
#endif

#endif
