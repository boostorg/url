//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_QUERY_PARAM_HPP
#define BOOST_URL_QUERY_PARAM_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/const_string.hpp>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
struct query_param;
namespace detail
{
    struct params_iterator_impl;
}
#endif

/** A query parameter view.

    Objects of this type represent a single key/value
    pair in a query string.
    The presence of a value is indicated by
    `has_value == true`.
    A value that is present with an empty string
    is distinct from a value that is absent.
    Ownership of the underlying character buffers
    representing the key and value is not transferred.
    The caller is responsible for ensuring that the
    buffers assigned to these members remains valid while
    the object exists.
*/
struct query_param_view
{
    /** The percent-encoded key
    */
    string_view key;

    /** The percent-encoded value

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty string
        is distinct from a value that is absent.
    */
    string_view value;

    /** True if the value is present

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty string
        is distinct from a value that is absent.
    */
    bool has_value = false;

#ifndef BOOST_URL_DOCS
#ifdef BOOST_NO_CXX14_AGGREGATE_NSDMI
    constexpr
    query_param_view(
        string_view key_ = {},
        string_view value_ = {},
        bool has_value_ = false ) noexcept
        : key(key_)
        , value(value_)
        , has_value(has_value_)
    {
    }
#endif
#endif
};

//------------------------------------------------

/** A query parameter value.

    Objects of this type represent a single
    key/value pair, possibly percent-encoded,
    in a query string.
    The presence of a value is indicated by
    `has_value == true`.
    A value that is present with an empty string
    is distinct from a value that is absent.
    Whether the strings are percent-encoded
    is determined by the container from which
    the value is obtained.
*/
struct query_param
{
    /** The key.
    */
    const_string key;

    /** The value.

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty string
        is distinct from a value that is absent.
    */
    const_string value;

    /** True if the value is present

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty string
        is distinct from a value that is absent.
    */
    bool has_value = false;

    /** Constructor
    */
    query_param() = default;

    /** Constructor
    */
    template<class Allocator>
    query_param(
        query_param_view const& v,
        Allocator const& a)
        : key(v.key, a)
        , value(v.has_value ?
            v.value :
            string_view(), a)
        , has_value(v.has_value)
    {
    }

    /** Constructor
    */
    query_param(
        const_string const& key_,
        const_string const& value_,
        bool has_value_)
        : key(key_)
        , value(value_)
        , has_value(has_value_)
    {
    }

    /** Conversion
    */
    operator query_param_view() const noexcept
    {
        if(has_value)
            return { key, value, true };
        return { key, string_view(), false };
    }

private:
    friend class params_view;
    friend class params;
    friend struct detail::params_iterator_impl;

    BOOST_URL_DECL
    query_param(
        char const* s,
        std::size_t nk,
        std::size_t nv,
        const_string::factory const& a);
};

} // urls
} // boost

#endif
