//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_VIEW_BASE_HPP
#define BOOST_URL_DETAIL_VIEW_BASE_HPP

#include <boost/url/detail/char_type.hpp>
#include <boost/url/detail/parts.hpp>

namespace boost {
namespace url {
namespace detail {

struct view_base
{
    using id_type = unsigned;

    char const* view_begin_ = nullptr;
    std::size_t const* view_end_ = nullptr;
    unsigned nseg_ = 0;
    unsigned nparam_ = 0;
    optional<unsigned short> port_;

    view_base() = default;
    view_base(view_base const&) = default;

    bool
    is_reset() const noexcept
    {
        return view_begin_ == nullptr;
    }

    // returns offset of piece
    std::size_t const&
    offset(id_type id) const noexcept
    {
        return *(view_end_ - id);
    }

    // returns length of piece
    std::size_t
    length(id_type id) const noexcept
    {
        return
            offset(id + 1) -
            offset(id);
    }

    // returns length of range
    std::size_t
    length(
        id_type first,
        id_type last) const noexcept
    {
        BOOST_ASSERT(first <= last);
        BOOST_ASSERT(last <= detail::id_end);
        if(! view_begin_)
            return 0;
        return
            offset(last) - offset(first);
    }

    // returns true if piece is empty
    bool
    empty(id_type id) const noexcept
    {
        return
            offset(id) ==
            offset(id + 1);
    }

    // returns true if range is empty
    bool
    empty(
        id_type first,
        id_type last) const noexcept
    {
        BOOST_ASSERT(first <= last);
        BOOST_ASSERT(last <= detail::id_end);
        if(! view_begin_)
            return true;
        while(first < last)
            if(! empty(first++))
                return false;
        return true;
    }

    // return id as string
    string_view
    get(id_type id) const noexcept
    {
        if(is_reset())
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
        BOOST_ASSERT(first <= last);
        BOOST_ASSERT(last <= detail::id_end);
        if(is_reset())
            return "";
        return {
            view_begin_ + offset(first),
            offset(last) - offset(first) };
    }

    template<class Allocator>
    string_type<Allocator>
    get_decoded(
        id_type id,
        Allocator const& a) const
    {
        if(! is_reset())
            return decode(get(id), a);
        return string_type<Allocator>(a);
    }
};

} // detail
} // url
} // boost

#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/detail/view_base.ipp>
#endif

#endif
