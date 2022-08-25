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
#include <boost/url/decode_view.hpp>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
struct query_param;
struct query_param_encoded_view;
namespace detail
{
    struct params_iterator_impl;
}
#endif

/** A query parameter view.

    Objects of this type represent a single key/value
    pair in a query string.

    A value that is present with an empty string
    is distinct from a value that is absent.

    The presence of a value is indicated by
    `has_value == true`.

    Ownership of the underlying character buffers
    representing the key and value is not transferred.

    The caller is responsible for ensuring that the
    buffers assigned to these members remains valid while
    the object exists.
*/
struct query_param_view
{
    /** The percent-decoded key
    */
    decode_view key;

    /** The percent-decoded value

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty string
        is distinct from a value that is absent.
    */
    decode_view value;

    /** True if the value is present

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty string
        is distinct from a value that is absent.
    */
    bool has_value = false;

    /// Constructor
    query_param_view() = default;

    /// Constructor
    query_param_view(
        string_view key_,
        string_view value_ = {},
        bool has_value_ = false ) noexcept
        : key(key_)
        , value(value_)
        , has_value(has_value_)
    {}

    /// Constructor
    query_param_view(
        decode_view key_,
        decode_view value_ = {},
        bool has_value_ = false ) noexcept
        : key(key_)
        , value(value_)
        , has_value(has_value_)
    {}

    /** Conversion
    */
    operator query_param_encoded_view() const noexcept;

};

//------------------------------------------------

/** A query parameter view of encoded parameteres.

    Objects of this type represent a single
    encoded key/value pair in a query string.

    A value that is present with an empty string
    is distinct from a value that is absent.

    The presence of a value is indicated by
    `has_value == true`.

    Ownership of the underlying character buffers
    representing the key and value is not transferred.
    The caller is responsible for ensuring that the
    buffers assigned to these members remains valid while
    the object exists.
*/
struct query_param_encoded_view
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

    /// Constructor
    query_param_encoded_view() = default;

    /// Constructor
    query_param_encoded_view(
        string_view key_,
        string_view value_ = {},
        bool has_value_ = false ) noexcept
        : key(key_)
        , value(value_)
        , has_value(has_value_)
    {
    }

    /// Constructor
    query_param_encoded_view(
        decode_view key_,
        decode_view value_ = {},
        bool has_value_ = false ) noexcept
        : key(key_.encoded())
        , value(value_.encoded())
        , has_value(has_value_)
    {
    }

    /** Conversion
    */
    operator query_param_view() const noexcept
    {
        if(has_value)
            return {
                decode_view(key),
                decode_view(value),
                true };
        return {
            decode_view(key),
            decode_view(),
            false };
    }

private:
    friend struct query_param;
    friend class params_view;
    friend class params;
    friend struct detail::params_iterator_impl;

    BOOST_URL_DECL
    query_param_encoded_view(
        char const* s,
        std::size_t nk,
        std::size_t nv);
};

//------------------------------------------------

/** A query parameter value.

    Objects of this type represent a single
    key/value pair, possibly percent-encoded,
    in a query string.

    A value that is present with an empty string
    is distinct from a value that is absent.
    The presence of a value is indicated by
    `has_value == true`.

    Whether the strings are percent-encoded
    is determined by the container from which
    the value is obtained.

    This type allows for making a copy of
    a parameter where ownership is retained
    in the copy.

*/
struct query_param
{
    /** The key.
    */
    std::string key;

    /** The value.

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty string
        is distinct from a value that is absent.
    */
    std::string value;

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
    query_param(
        query_param_view const& v)
        : key(v.key.to_string())
        , value(v.has_value ?
            v.value.to_string() :
            std::string())
        , has_value(v.has_value)
    {
    }

    /** Constructor
    */
    query_param(
        query_param_encoded_view const& v)
        : key(v.key)
        , value(v.has_value ?
            v.value :
            string_view())
        , has_value(v.has_value)
    {
    }

    /** Constructor
    */
    query_param(
        decode_view const& key_,
        decode_view const& value_,
        bool has_value_)
        : key(key_.to_string())
        , value(value_.to_string())
        , has_value(has_value_)
    {
    }

    /** Constructor
    */
    query_param(
        string_view const& key_,
        string_view const& value_,
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
            return {
                decode_view(key),
                decode_view(value),
                true };
        return {
            decode_view(key),
            decode_view(),
            false };
    }

    /** Conversion
    */
    operator query_param_encoded_view() const noexcept
    {
        if(has_value)
            return { key, value, true };
        return { key, string_view(), false };
    }

private:
    friend class params_view;
    friend class params;
    friend struct detail::params_iterator_impl;
};

} // urls
} // boost

#include <boost/url/impl/query_param.hpp>

#endif
