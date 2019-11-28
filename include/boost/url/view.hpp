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
#include <boost/url/detail/buffer.hpp>
#include <string>

namespace boost {
namespace url {

/*

URL:    scheme    authority        path        query        fragment
        foo:   // example.com:8042 /over/there ?name=ferret #nose
        urn:                       example:animal:ferret:nose
*/

class view
{
protected:
    using size_type = uint32_t;
    using id_type = unsigned;

    char const* view_begin_ = nullptr;
    size_type const* view_end_ = nullptr;
    unsigned n_path_ = 1;
    unsigned n_query_ = 1;
    optional<unsigned short> port_;

    enum
    {
        // one-based

        id_scheme = 1,
        id_username,
        id_password, // trailing ':'
        id_hostname,
        id_path
    };

    bool
    is_reset() const noexcept
    {
        return view_begin_ == nullptr;
    }

    id_type
    id_query() const noexcept
    {
        return id_path + n_path_;
    }

    id_type
    id_fragment() const noexcept
    {
        return id_query() + n_query_;
    }

    id_type
    id_end() const noexcept
    {
        return id_fragment() + 1;
    }

public:
    /// The string alias template return type for allocating member functions.
    template<class Allocator>
    using string_type =
        std::basic_string<char,
            std::char_traits<char>, Allocator>;

    //------------------------------------------------------

    /** Return the scheme.
    */
    string_view
    scheme() const noexcept
    {
        if(is_reset())
            return "";
        return get(id_scheme);
    }

    //------------------------------------------------------

    /** Return the username, if any.
    */
    string_view
    username() const noexcept
    {
        if(is_reset())
            return "";
        return get(id_username);
    }

    /** Return the percent-encoded username.
    */
    template<class Allocator = std::allocator<char>>
    string_type<Allocator>
    encoded_username(
        Allocator const& a = {}) const
    {
        return get_encoded(id_username, a,
            detail::userinfo_pct_set());
    }

    string_view
    password() const noexcept
    {
        if(is_reset())
            return "";
        return get(id_password);
    }

    template<class Allocator = std::allocator<char>>
    string_type<Allocator>
    encoded_password(
        Allocator const& a = {}) const
    {
        return get_encoded(id_password, a,
            detail::userinfo_pct_set());
    }

    /** Return the encoded userinfo.
    */
    template<class Allocator = std::allocator<char>>
    string_type<Allocator>
    encoded_userinfo(
        Allocator const& a = {}) const
    {
        detail::string_buffer<
            Allocator> b(a);
        if(! is_reset())
        {
            detail::counting_buffer b0;
            serialize_userinfo(b0);
            b.prepare(b0.result());
            serialize_userinfo(b);
        }
        return b.result();
    }

    //------------------------------------------------------

    string_view
    hostname() const noexcept
    {
        if(is_reset())
            return "";
        return get(id_hostname);
    }

    template<class Allocator = std::allocator<char>>
    string_type<Allocator>
    encoded_hostname(
        Allocator const& a = {}) const
    {
        return get_encoded(id_hostname, a,
            detail::reg_name_pct_set());
    }

    optional<unsigned short>
    port() const noexcept
    {
        return port_;
    }

    template<class Allocator = std::allocator<char>>
    string_type<Allocator>
    encoded_host(
        Allocator const& a = {}) const
    {
        detail::string_buffer<
            Allocator> b(a);
        detail::counting_buffer b0;
        serialize_host(b0);
        b.prepare(b0.result());
        serialize_host(b);
        return b.result();
    }

    //------------------------------------------------------

    template<class Allocator = std::allocator<char>>
    string_type<Allocator>
    encoded_authority(
        Allocator const& a = {}) const
    {
        detail::string_buffer<
            Allocator> b(a);
        if(! is_reset())
        {
            detail::counting_buffer b0;
            serialize_authority(b0);
            b.prepare(b0.result());
            serialize_authority(b);
        }
        return b.result();
    }

    template<class Allocator = std::allocator<char>>
    string_type<Allocator>
    encoded_origin(
        Allocator const& a = {}) const
    {
        detail::string_buffer<
            Allocator> b(a);
        if(! is_reset())
        {
            detail::counting_buffer b0;
            serialize_origin(b0);
            b.prepare(b0.result());
            serialize_origin(b);
        }
        return b.result();
    }

    // "example.org"
    string_view
    domain(bool ignoreSLD = false) const noexcept;

    // "www"
    string_view
    subdomain() const noexcept;

    // "org"
    string_view
    tld(bool ignoreSLD = false) const noexcept;

    // "/foo/hello.html"
    // VFALCO returns decoded
    string_view
    path() const noexcept
    {
        return get(id_path, id_query());
    }

    // "/foo"
    // VFALCO returns decoded
    string_view
    directory() const noexcept;

    // "hello.html"
    // VFALCO returns decoded
    string_view
    filename() const noexcept;

    // "html"
    // VFALCO returns decoded
    string_view
    suffix() const noexcept;

    string_view
    segment(int pos) const;

    string_view
    segment_coded(int pos) const;

    // ?cols=80
    string_view
    search() const noexcept
    {
        return get(
            id_query(), id_fragment());
    }

    // cols=80
    string_view
    query() const noexcept
    {
        auto sv = search();
        if(sv.size() < 2)
            return "";
        return sv.substr(1);
    }

    // #frag
    string_view
    hash() const noexcept
    {
        return get(
            id_fragment(), id_end());
    }

    // frag
    string_view
    fragment() const noexcept
    {
        auto sv = hash();
        if(sv.size() < 2)
            return "";
        return sv.substr(1);
    }

    // /foo/hello.html?cols=80#frag
    string_view
    resource() const noexcept
    {
        return get(
            id_path, id_end());
    }

    //------------------------------------------------------

    /** Return the complete serialized URL.
    */
    template<class Allocator = std::allocator<char>>
    string_type<Allocator>
    encoded_href(Allocator const& a = {}) const
    {
        detail::string_buffer<
            Allocator> b(a);
        if(! is_reset())
        {
            detail::counting_buffer b0;
            serialize_href(b0);
            b.prepare(b0.result());
            serialize_href(b);
        }
        return b.result();
    }

    //------------------------------------------------------

protected:
    // returns offset of piece
    size_type
    offset(id_type id) const noexcept
    {
        return *(view_end_ - id);
    }

    // returns length of piece
    size_type
    length(id_type id) const noexcept
    {
        return
            offset(id + 1) -
            offset(id);
    }

    // returns true if piece is empty
    bool
    empty(id_type id) const noexcept
    {
        return
            offset(id) ==
            offset(id + 1);
    }

    // return piece as string
    string_view
    get(id_type id) const noexcept
    {
        if(! view_begin_)
            return "";
        return {
            view_begin_ + offset(id),
            length(id) };
    }

    // return [first, last) as string
    string_view
    get(id_type first,
        id_type last) const noexcept
    {
        BOOST_ASSERT(first <= id_end());
        BOOST_ASSERT(last <= id_end());
        if(! view_begin_)
            return "";
        return {
            view_begin_ + offset(first),
            offset(last) - offset(first) };
    }

    template<class Allocator>
    string_type<Allocator>
    get_encoded(
        id_type id,
        Allocator const& a,
        detail::pct_encoding e) const
    {
        detail::string_buffer<
            Allocator> b(a);
        if(! is_reset())
        {
            auto const s = get(id);
            b.prepare(e.encoded_size(s));
            b.encode(s, e);
        }
        return b.result();
    }

private:
    BOOST_URL_DECL
    void
    serialize_userinfo(
        detail::any_buffer& b) const;

    BOOST_URL_DECL
    void
    serialize_host(
        detail::any_buffer& b) const;

    BOOST_URL_DECL
    void
    serialize_authority(
        detail::any_buffer& b) const;

    BOOST_URL_DECL
    void
    serialize_origin(
        detail::any_buffer& b) const;

    BOOST_URL_DECL
    void
    serialize_href(
        detail::any_buffer& b) const;
};

} // url
} // boost

#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/view.ipp>
#endif

#endif
