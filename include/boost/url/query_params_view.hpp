//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_QUERY_PARAMS_VIEW_HPP
#define BOOST_URL_QUERY_PARAMS_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/char_type.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <boost/url/rfc/pct_encoding.hpp>
#include <utility>

namespace boost {
namespace urls {

/** A ForwardRange view of read-only query parameters
*/
class query_params_view
{
    string_view s_;
    std::size_t n_;

    friend class url;
    friend class url_view;

    query_params_view(
        string_view s,
        std::size_t n)
        : s_(s)
        , n_(n)
    {
    }

public:
    class value_type;
    class iterator;

    query_params_view(
        query_params_view const&) = default;
    query_params_view& operator=(
        query_params_view const&) = default;

    BOOST_URL_DECL
    query_params_view() noexcept;

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

    /** Return true if the key exists
    */
    BOOST_URL_DECL
    bool
    contains(string_view key) const noexcept;

    /** Return the number of matching keys
    */
    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    /** Find the first occurrence of a key
    */
    BOOST_URL_DECL
    iterator
    find(string_view key) const noexcept;

    /** Find the next occurrence of a key
    */
    BOOST_URL_DECL
    iterator
    find(
        iterator after,
        string_view key) const noexcept;

    /** Return the value for a key, or the empty string
    */
    BOOST_URL_DECL
    std::string
    operator[](string_view key) const;

    /** Return the value for the first matching key if it exists, otherwise throw
    */
    template<class Allocator =
        std::allocator<char>>
    string_type<Allocator>
    at( string_view key,
        Allocator const& a = {}) const;

    /** Parse the query params string and return a view

        The query string should not include a
        leading question mark ('?').
    */
    BOOST_URL_DECL
    friend
    query_params_view
    parse_query_params(
        string_view s,
        error_code& ec);

    /** Parse the query params string and return a view

        The query string should not include a
        leading question mark ('?').
    */
    BOOST_URL_DECL
    friend
    query_params_view
    parse_query_params(
        string_view s);
};

BOOST_URL_DECL
query_params_view
parse_query_params(
    string_view s,
    error_code& ec);

BOOST_URL_DECL
query_params_view
parse_query_params(
    string_view s);

//----------------------------------------------------------

/** The value type for query parameters
*/
class query_params_view::value_type
{
    rfc::pct_encoded_str k_;
    rfc::pct_encoded_str v_;
    bool has_value_ = false;

    friend class iterator;
    friend class query_params_view;

public:
    value_type() = default;
    value_type& operator=(
        value_type const&) = default;
    value_type(
        value_type const&) = default;

    /** Return the key

        This function returns the key as
        a percent-encoded string.

        @see key
    */
    string_view
    encoded_key() const noexcept
    {
        return k_.str;
    }

    /** Return the key

        This function returns the key as a
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
    key(Allocator const& a = {}) const
    {
        return rfc::pct_decode_unchecked(
            k_.str, k_.decoded_size, a);
    }

    /** Return true if a value exists for this element
    */
    bool
    has_value() const noexcept
    {
        return has_value_;
    }

    /** Return the value if it exists, or an empty string
    */
    string_view
    encoded_value() const noexcept
    {
        return v_.str;
    }

    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    value(Allocator const& a = {}) const
    {
        return rfc::pct_decode_unchecked(
            v_.str, v_.decoded_size, a);
    }

    value_type const*
    operator->() const noexcept
    {
        return this;
    }

    operator
    std::pair<
        std::string const,
        std::string>() const
    {
        return { key(), value() };
    }
};

} // urls
} // boost

#include <boost/url/impl/query_params_view.hpp>

#endif
