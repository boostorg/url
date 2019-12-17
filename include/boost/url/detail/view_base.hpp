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

namespace boost {
namespace url {
namespace detail {

struct view_base
{
    using size_type = uint32_t;
    using id_type = unsigned;

    char const* view_begin_ = nullptr;
    size_type const* view_end_ = nullptr;
    unsigned n_seg_ = 1;
    unsigned n_query_ = 1;
    optional<unsigned short> port_;

    enum
    {
        // one-based

        id_scheme = 1,      // trailing ':'
        id_username,        // leading "//"
        id_password,        // leading ':', trailing '@'
        id_hostname,
        id_port,            // leading ':'
        id_path
    };

    view_base() = default;
    view_base(view_base const&) = default;

    bool
    is_reset() const noexcept
    {
        return view_begin_ == nullptr;
    }

    id_type
    id_query() const noexcept
    {
        return id_path + n_seg_;
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

    inline
    id_type
    id_segment(int index) const;

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

    // returns length of range
    size_type
    length(
        id_type first,
        id_type last) const noexcept
    {
        BOOST_ASSERT(first <= last);
        BOOST_ASSERT(last <= id_end());
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
        BOOST_ASSERT(last <= id_end());
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
        BOOST_ASSERT(last <= id_end());
        if(is_reset())
            return "";
        return {
            view_begin_ + offset(first),
            offset(last) - offset(first) };
    }

    template<class Allocator>
    string_type<Allocator>
    decode(
        string_view sv,
        Allocator const& a) const
    {
        string_type<Allocator> s(a);
        s.resize(
            detail::pct_encoding::
                raw_decoded_size(sv));
        detail::pct_encoding::
            decode(&s[0], sv);
        return s;
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
