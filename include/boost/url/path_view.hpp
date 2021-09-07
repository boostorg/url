//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PATH_VIEW_HPP
#define BOOST_URL_PATH_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/char_type.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <boost/url/rfc/pct_encoding.hpp>
#include <utility>

namespace boost {
namespace urls {

/** A ForwardRange view of read-only path segments
*/
class path_view
{
    string_view s_;
    std::size_t n_;

    friend class url;
    friend class url_view;

    path_view(
        string_view s,
        std::size_t n)
        : s_(s)
        , n_(n)
    {
    }

public:
    class value_type;
    class iterator;

    path_view(
        path_view const&) = default;
    path_view& operator=(
        path_view const&) = default;

    BOOST_URL_DECL
    path_view() noexcept;

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

    /** Parse the path string and return a view
    */
    BOOST_URL_DECL
    friend
    path_view
    parse_path(
        string_view s,
        error_code& ec);

    /** Parse the path string and return a view
    */
    BOOST_URL_DECL
    friend
    path_view
    parse_path(
        string_view s);
};

BOOST_URL_DECL
path_view
parse_path(
    string_view s,
    error_code& ec);

BOOST_URL_DECL
path_view
parse_path(
    string_view s);

//----------------------------------------------------------

/** The value type for query parameters
*/
class path_view::value_type
{
    pct_encoded_str s_;

    friend class iterator;
    friend class path_view;

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
        return s_.str;
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
        return pct_decode_unchecked(
            s_.str, s_.decoded_size, a);
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

#include <boost/url/impl/path_view.hpp>

#endif
