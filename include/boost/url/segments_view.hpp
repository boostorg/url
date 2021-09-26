//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_VIEW_HPP
#define BOOST_URL_SEGMENTS_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <cstddef>
#include <iosfwd>

namespace boost {
namespace urls {

/** A BidirectionalRange view of read-only path segments with percent-decoding applied
*/
class segments_view
{
    string_view s_;
    std::size_t n_;
    string_value::allocator a_;

    friend class url;
    friend class url_view;

    template<class Allocator>
    segments_view(
        string_view s,
        std::size_t n,
        Allocator const& a)
        : s_(s)
        , n_(n)
        , a_(a)
    {
    }

public:
    class iterator;

    BOOST_URL_DECL
    segments_view(
        segments_view const&) noexcept;

    segments_view& operator=(
        segments_view const&) noexcept = delete;

    /** Return true if the range contains no elements
    */
    bool
    empty() const noexcept
    {
        return n_ == 0;
    }

    /** Return the number of elements in the range
    */
    std::size_t
    size() const noexcept
    {
        return n_;
    }

    /** Return an iterator to the beginning of the range
    */
    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    /** Return an iterator to the end of the range
    */
    BOOST_URL_DECL
    iterator
    end() const noexcept;
};

} // urls
} // boost

#include <boost/url/impl/segments_view.hpp>

#endif
