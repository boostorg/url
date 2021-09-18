//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_HPP
#define BOOST_URL_SEGMENTS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/pct_encoding.hpp>

namespace boost {
namespace urls {

/** A RandomAccessContainer representing segments in a URL

    @see
        @ref url::segments.
*/
class segments
{
    friend class url;

    url* u_ = nullptr;

    explicit
    segments(url& u) noexcept
        : u_(&u)
    {
    }

public:
    class value_type;
    class iterator;

    segments(
        segments const&) = default;
    segments& operator=(
        segments const&) = default;

    BOOST_URL_DECL
    segments() noexcept;

    /** Return true if there are no segments in the path
    */
    BOOST_URL_DECL
    bool
    empty() const noexcept;

    /** Return the number of segments in the path
    */
    BOOST_URL_DECL
    std::size_t
    size() const noexcept;

    /** Return an iterator to the first segment in the path
    */
    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    /** Return an iterator to one past the last segment in the path
    */
    BOOST_URL_DECL
    iterator
    end() const noexcept;
};

//----------------------------------------------------------

/** The value type for a segments container
*/
class segments::value_type
{
    url* u_ = nullptr;
    std::size_t i_ = 0;
    string_view s_;

    friend class iterator;
    friend class segments;

    value_type(
        url* u,
        std::size_t i,
        string_view s) noexcept
        : u_(u)
        , i_(i)
        , s_(s)
    {
    }

public:
    value_type() = default;
    value_type& operator=(
        value_type const&) = default;
    value_type(
        value_type const&) = default;

    /** Return the segment

        This function returns the key as
        a percent-encoded string.

        @see key
    */
    string_view
    encoded_segment() const noexcept
    {
        return s_;
    }

    /** Return the segment

        This function returns the segment as a
        string with percent-decoding applied.

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
    segment(Allocator const& a = {}) const
    {
        return pct_decode(
            s_, {}, {}, a);
    }

    value_type const*
    operator->() const noexcept
    {
        return this;
    }

    operator
    std::string() const
    {
        return segment();
    }
};

} // urls
} // boost

#include <boost/url/impl/segments.hpp>

#endif
