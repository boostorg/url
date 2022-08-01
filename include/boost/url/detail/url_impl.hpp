//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_URL_IMPL_HPP
#define BOOST_URL_DETAIL_URL_IMPL_HPP

#include <boost/url/host_type.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <boost/assert.hpp>
#include <cstdint>

namespace boost {
namespace urls {

class url_view;
class authority_view;

namespace detail {

// This is the private 'guts' of a
// url_view, exposed so different parts
// of the implementation can work on it.
struct url_impl : parts_base
{
    static
    constexpr
    char const* const empty_ = "";
 
    static
    constexpr
    pos_t const zero_ = 0;

    char const* cs_ = empty_;
    pos_t offset_[id_end + 1] = {};
    pos_t decoded_[id_end] = {};
    pos_t nseg_ = 0;
    pos_t nparam_ = 0;
    unsigned char ip_addr_[16] = {};
    // VFALCO don't we need a bool?
    std::uint16_t port_number_ = 0;
    host_type host_type_ =
        urls::host_type::none;
    scheme scheme_ =
        urls::scheme::none;

    // true if this belongs to
    // authority_view. in this case id_user
    // will not have the leading "//".
    bool is_authority;

    explicit
    url_impl(
        bool b) noexcept
        : is_authority(b)
    {
    }

    // in url_view.ipp
    url_view construct() const noexcept;

    // in authority_view.ipp
    authority_view
    construct_authority() const noexcept;

    pos_t len(int, int) const noexcept;
    pos_t len(int) const noexcept;
    pos_t offset(int) const noexcept;
    string_view get(int) const noexcept;
    string_view get(int, int) const noexcept;
    void set_size(int, pos_t) noexcept;
    void split(int, std::size_t) noexcept;
    void adjust(int, int, std::size_t) noexcept;
    void collapse(int, int, std::size_t) noexcept;

    void apply_scheme(string_view) noexcept;
    void apply_userinfo(pct_encoded_view const&,
        pct_encoded_view const*) noexcept;
    void apply_host(host_type, string_view,
        unsigned char const*, pct_encoded_view const&) noexcept;
    void apply_port(string_view, unsigned short) noexcept;
    void apply_authority(authority_view const&) noexcept;
    void apply_path(string_view, std::size_t) noexcept;
    void apply_query(string_view, std::size_t) noexcept;
    void apply_frag(pct_encoded_view) noexcept;
};

//------------------------------------------------

// return length of [first, last)
inline
auto
url_impl::
len(
    int first,
    int last) const noexcept ->
        pos_t
{
    BOOST_ASSERT(first <= last);
    BOOST_ASSERT(last <= id_end);
    return offset(last) - offset(first);
}

// return length of part
inline
auto
url_impl::
len(int id) const noexcept ->
    pos_t
{
    return id == id_end
        ? zero_
        : ( offset(id + 1) -
            offset(id) );
}

// return offset of id
inline
auto
url_impl::
offset(int id) const noexcept ->
    pos_t
{
    return
        id == id_scheme
        ? zero_
        : offset_[id];
}

// return id as string
inline
string_view
url_impl::
get(int id) const noexcept
{
    return {
        cs_ + offset(id), len(id) };
}

// return [first, last) as string
inline
string_view
url_impl::
get(int first,
    int last) const noexcept
{
    return { cs_ + offset(first),
        offset(last) - offset(first) };
}

//------------------------------------------------

// change id to size n
inline
void
url_impl::
set_size(
    int id,
    pos_t n) noexcept
{
    auto d = n - len(id);
    for(auto i = id + 1;
        i <= id_end; ++i)
        offset_[i] += d;
}

// trim id to size n,
// moving excess into id+1
inline
void
url_impl::
split(
    int id,
    std::size_t n) noexcept
{
    BOOST_ASSERT(id < id_end - 1);
    //BOOST_ASSERT(n <= len(id));
    offset_[id + 1] = offset(id) + n;
}

// add n to [first, last]
inline
void
url_impl::
adjust(
    int first,
    int last,
    std::size_t n) noexcept
{
    for(int i = first;
            i <= last; ++i)
        offset_[i] += n;
}

// set [first, last) offset
inline
void
url_impl::
collapse(
    int first,
    int last,
    std::size_t n) noexcept
{
    for(int i = first + 1;
            i < last; ++i)
        offset_[i] = n;
}

} // detail
} // urls
} // boost

#endif
