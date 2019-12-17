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
#include <boost/url/detail/view_base.hpp>
#include <string>

namespace boost {
namespace url {

class path : protected detail::view_base
{
    friend class view;

    path(detail::view_base const& v) noexcept
    {
        static_cast<
            detail::view_base&
                >(*this) = v;
    }

public:
    class const_iterator
    {
        friend path;

        path const* path_ = nullptr;
        unsigned i_ = 0;

        const_iterator(
            path const& p,
            unsigned i)
            : path_(&p)
            , i_(i)
        {
        }

    public:
        using value_type = string_view;

        const_iterator() = default;

        value_type
        operator*() const noexcept
        {
            return path_->get(
                id_path + i_);
        }

        bool
        operator==(
            const_iterator other) const noexcept
        {
            return
                path_ == other.path_ &&
                i_ == other.i_;
        }

        bool
        operator!=(
            const_iterator other) const noexcept
        {
            return !(*this == other);
        }

        const_iterator&
        operator++() noexcept
        {
            ++i_;
            return *this;
        }

        const_iterator
        operator++(int) noexcept
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    using iterator = const_iterator;

    std::size_t
    size() const noexcept
    {
        return n_seg_;
    }

    const_iterator
    begin() const noexcept
    {
        return const_iterator(*this, 0);
    }

    const_iterator
    end() const noexcept
    {
        return const_iterator(
            *this, n_seg_);
    }
};

    //------------------------------------------------------

class view
#ifndef BOOST_URL_DOCS
    : protected detail::view_base
#endif
{
public:
    /** Return the encoded URL as a null-terminated string.
    */
    char const*
    c_str() const noexcept
    {
        if(! is_reset())
            return "";
        BOOST_ASSERT(
            view_begin_[id_end()] == 0);
        return view_begin_;
    }

    /** Return the complete serialized URL.
    */
    string_view
    encoded_href() const
    {
        return get(id_scheme, id_end());
    }

    /** Return the encoded origin.
    */
    BOOST_URL_DECL
    string_view
    encoded_origin() const noexcept
    {
        return get(id_scheme, id_path);
    }

    string_view
    encoded_resource() const noexcept
    {
        return get(
            id_path, id_end());
    }

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
        return decode(
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
        return decode(
            encoded_password(), a);
    }

    /** Return the password.
    */
    BOOST_URL_DECL
    string_view
    encoded_password() const noexcept;

    //
    // host
    //

    /** Return the encoded host.
    */
    string_view
    encoded_host() const noexcept
    {
        return get(
            id_hostname,
            id_path);
    }


    /** Return the hostname.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    hostname(
        Allocator const& a = {}) const
    {
        return decode(
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
        return port_;
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

    /** Return the encoded path.
    */
    string_view
    encoded_path() const noexcept
    {
        return get(id_path, id_query());
    }

    /** Return the path as a range.
    */
    url::path
    segments() const noexcept
    {
        return url::path(static_cast<
            view_base const&>(*this));
    }

    /** Return a path segment.
    */
    template<
        class Allocator =
            std::allocator<char>>
    BOOST_URL_DECL
    string_type<Allocator>
    segment(
        int pos,
        Allocator const& a = {}) const
    {
        return decode(
            encoded_segment(pos, a));
    }

    /** Return an encoded path segment.
    */
    BOOST_URL_DECL
    string_view
    encoded_segment(int pos) const;

    //------------------------------------------------------
    //
    // query
    //
    //------------------------------------------------------

    string_view
    encoded_query() const noexcept
    {
        auto s = get(
            id_query(), id_fragment());
        if(! s.empty())
            s = s.substr(1);
        return s;
    }

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
        return decode(
            encoded_fragment(), a);

    }

    /** Return the fragment.
    */
    string_view
    encoded_fragment() const noexcept
    {
        return get(
            id_fragment(), id_end());
    }
};

} // url
} // boost

#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/view.ipp>
#endif

#endif
