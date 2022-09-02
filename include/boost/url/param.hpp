//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAM_HPP
#define BOOST_URL_PARAM_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/pct_string_view.hpp>
#include <boost/url/string_view.hpp>
#include <string>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
struct param;
struct param_view;
#endif

/** A query parameter
*/
struct param_pct_view
{
    /** The query parameter key

        For most usages, key comparisons are
        case-sensitive and duplicate keys in
        a query are possible. However, it is
        the authority that has final control
        over how the query is interpreted.

        @note
        A @ref pct_string_view references a
        character buffer which may contain
        percent-escapes; ownership of the
        buffer is not transferred.
    */
    pct_string_view key;

    /** The query parameter value

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty
        string is distinct from a value that
        is absent.

        @note
        A @ref pct_string_view references a
        character buffer which may contain
        percent-escapes; ownership of the
        buffer is not transferred.
    */
    pct_string_view value;

    /** True if a value is present

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty string
        is distinct from a value that is absent.
    */
    bool has_value = false;

    /** Constructor

        Default constructed query parameters
        have an empty key and no value.

        @par Exception Safety
        Throws nothing.
    */
    param_pct_view() = default;

    /** Constructor

        This constructs a query parameter with
        the specified key, and no value. That
        is, @ref has_value will be `false`,
        and @ref value will be empty.
        Ownership of the underlying character
        buffers is not transferred.

        @par Exception Safety
        Exceptions thrown on invalid input.

        @throw system_error
        `key` contains an invalid percent-encoding.

        @param key The key to use
    */
    param_pct_view(
        pct_string_view key) noexcept
        : key(key)
    {
    }

    /** Constructor

        This constructs a query parameter with
        the specified key and value. After
        construction, @ref has_value will be
        `true`.
        Ownership of the underlying character
        buffers is not transferred.

        @par Exception Safety
        Exceptions thrown on invalid input.

        @throw system_error
        `key` or `value` contain an
        invalid percent-encoding.

        @param key The key to use

        @param value The value to use
    */
    param_pct_view(
        pct_string_view key,
        pct_string_view value) noexcept
        : key(key)
        , value(value)
        , has_value(true)
    {
    }

#if 1
    /** Conversion

        This function returns the query parameter
        converted to its encoded representation.
        No encoding or decoding actually takes
        place.
        The returned result references the
        original character buffers; ownership
        is not transferred.

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.

        @return The encoded query parameter
    */
    operator
    param_view() const noexcept;
#endif

#ifndef BOOST_URL_DOCS
    /** Arrow operator support
    */
    param_pct_view const*
    operator->() const noexcept
    {
        return this;
    }

    /** Internal construction
    */
    param_pct_view(
        pct_string_view key,
        pct_string_view value,
        bool has_value) noexcept
        : key(key)
        , value(has_value
            ? value
            : pct_string_view())
        , has_value(has_value)
    {
    }
#endif
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
struct param_view
{
    /** The key
    */
    string_view key;

    /** The value

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

    /** Constructor

        A default constructed query parameter
        has an empty key and no value.

        @par Postconditions
        @code
        key.empty() && value.empty() && ! has_value
        @endcode

        @par Exception Safety
        Throws nothing.
    */
    param_view() = default;

    /// Constructor
    param_view(
        string_view key) noexcept
        : key(key)
    {
    }

    /// Constructor
    param_view(
        string_view key,
        string_view value) noexcept
        : key(key)
        , value(value)
        , has_value(true)
    {
    }

    /** Conversion
    */
    operator param_pct_view() const noexcept
    {
        if(has_value)
            return {
                pct_string_view(key),
                pct_string_view(value)};
        return {
            pct_string_view(key)};
    }

    /** Arrow operator support
    */
    param_view const*
    operator->() const noexcept
    {
        return this;
    }

#ifndef BOOST_URL_DOCS
    param_view(
        string_view key,
        string_view value,
        bool has_value) noexcept
        : key(key)
        , value(has_value
            ? value
            : string_view())
        , has_value(has_value)
    {
    }
#endif
};

//------------------------------------------------

/** A query parameter

    This represents a key and value pair that
    is commonly used in the query part of a URL.
    The key is always present, and may be empty
    The value may be present, denoted when
    @ref has_value equals `false`. The absence
    of a value is distinct from a value equal
    to the empty string.
    <br>
    This container
*/
struct param
{
    /** The query parameter key

        For most usages, key comparisons are
        case-sensitive and duplicate keys in
        a query are possible. However, it is
        the authority that has final control
        over how the query is interpreted.
    */
    std::string key;

    /** The query parameter value

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty
        string is distinct from a value that
        is absent.
    */
    std::string value;

    /** True if a value is present

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty string
        is distinct from a value that is absent.
    */
    bool has_value = false;

    /** Constructor

        Default constructed query parameters
        have an empty key and no value.

        @par Exception Safety
        Throws nothing.
    */
    param() = default;

    /** Constructor

        This constructs a query parameter with
        a key and no value.
        The string may be
        plain or encoded. No validation is
        performed on encoded strings.

        @par Exception Safety
        Calls to allocate may throw.
    */
    param(
        string_view key)
        : key(key)
    {
    }

    /** Constructor

        This constructs a query parameter with
        the specified key and value. After
        construction, @ref has_value will be
        `true`. The construction makes a copy
        of the key and value, acquiring unique
        ownership of the new character buffers.

        @par Exception Safety
        Calls to allocate may throw.

        @param key The key to use.
            The key will be copied.

        @param value The value to use.
            The value will be copied.
    */
    param(
        string_view key,
        string_view value)
        : key(key)
        , value(value)
        , has_value(true)
    {
    }

    /** Constructor

        This constructs a query parameter from
        a reference, making copies of the key
        and value and acquiring unique ownership.
        The strings may be encoded. No validation
        is performed on encoded strings.
    */
    param(
        param_view const& v)
        : key(v.key)
        , value(
            v.has_value
            ? v.value
            : string_view())
        , has_value(v.has_value)
    {
    }

    //---

    /** Constructor
    */
    param(
        param_pct_view const& v)
        : key(v.key.decode_to_string())
        , value(v.has_value ?
            v.value.decode_to_string() :
            std::string())
        , has_value(v.has_value)
    {
    }

    /** Constructor
    */
    param(
        decode_view const& key_,
        decode_view const& value_,
        bool has_value_)
        : key(key_.to_string())
        , value(value_.to_string())
        , has_value(has_value_)
    {
    }

    /** Conversion
    */
    operator param_pct_view() const noexcept
    {
        if(has_value)
            return {
                pct_string_view(key),
                pct_string_view(value)};
        return {
            pct_string_view(key)};
    }

    /** Conversion
    */
    operator param_view() const noexcept
    {
        if(has_value)
            return { key, value };
        return { key };
    }

    /** Arrow operator support
    */
    param const*
    operator->() const noexcept
    {
        return this;
    }

#ifndef BOOST_URL_DOCS
    param(
        string_view key,
        string_view value,
        bool has_value) noexcept
        : key(key)
        , value(has_value
            ? value
            : string_view())
        , has_value(has_value)
    {
    }
#endif
};

} // urls
} // boost

#include <boost/url/impl/param.hpp>

#endif
