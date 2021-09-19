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
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <iosfwd>
#include <utility>

namespace boost {
namespace urls {

class encoded_segments_view;

/** A BidirectionalRange view of read-only path segments with percent-decoding applied
*/
template<class Allocator>
class segments_view
{
    string_view s_;
    std::size_t n_;
    Allocator a_;

    friend class url;
    friend class url_view;

    segments_view(
        string_view s,
        std::size_t n,
        Allocator const& alloc) noexcept
        : s_(s)
        , n_(n)
        , a_(alloc)
    {
    }

public:
    class iterator;

    segments_view(
        segments_view const&) noexcept = default;
    segments_view& operator=(
        segments_view const&) noexcept = default;

    explicit
    segments_view(
        encoded_segments_view const& sv,
        Allocator const& = {}) noexcept;

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
    iterator
    begin() const noexcept;

    /** Return an iterator to the end of the range
    */
    iterator
    end() const noexcept;
};

} // urls
} // boost

#include <boost/url/impl/segments_view.hpp>

#endif
