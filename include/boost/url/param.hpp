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
#include <boost/url/string_view.hpp>
#include <boost/url/decode_view.hpp>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
struct param;
struct param_view;
#endif

/** A query parameter

    This represents an individual key/value
    pair commonly found in the query part
    of a URL. A parameter consists of a
    percent-encoded key followed by an
    optional percent-encoded value. Both
    of these strings may be empty; an empty
    key is distinguishable from the absence
    of a key, represented when @ref has_value
    is equal to `false`.

    The strings are represented by views which
    reference the underlying character buffers
    without taking ownership; The caller is
    responsible for ensuring that the lifetime
    of the character buffers extends until they
    are no longer referenced.
*/
struct param_decode_view
{
    /** The query parameter key

        For most usages, key comparisons are
        case-sensitive and duplicate keys in
        a query are possible. However, it is
        the authority that has final control
        over how the query is interpreted.

        @note
        A decode view references an external,
        encoded character buffer which is
        decoded upon iteration.
        Ownership of the referenced buffer is
        not transferred. See @ref decode_view
        for available operations.
    */
    decode_view key;

    /** The query parameter value

        The presence of a value is indicated by
        `has_value == true`.
        A value that is present with an empty
        string is distinct from a value that
        is absent.

        @note
        A decode view references a valid,
        external, percent-encoded character
        buffer which is decoded upon iteration.
        Ownership of the referenced buffer is
        not transferred. See @ref decode_view
        for other possible operations.
    */
    decode_view value;

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
    param_decode_view() = default;

    /** Constructor

        This constructs a query parameter with
        the specified key, and no value. That
        is, @ref has_value will be `false`,
        and @ref value will be empty.
        Ownership of the underlying character
        buffers is not transferred.

        @par Exception Safety
        Throws nothing.

        @param key The key to use
    */
    param_decode_view(
        decode_view key) noexcept
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
        Throws nothing.

        @param key The key to use

        @param value The value to use
    */
    param_decode_view(
        decode_view key,
        decode_view value) noexcept
        : key(key)
        , value(value)
        , has_value(true)
    {
    }

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

    /** Arrow operator support
    */
    param_decode_view const*
    operator->() const noexcept
    {
        return this;
    }
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
    operator param_decode_view() const noexcept
    {
        if(has_value)
            return {
                decode_view(key),
                decode_view(value)};
        return {
            decode_view(key)};
    }

    /** Arrow operator support
    */
    param_view const*
    operator->() const noexcept
    {
        return this;
    }
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
        param_decode_view const& v)
        : key(v.key.to_string())
        , value(v.has_value ?
            v.value.to_string() :
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
    operator param_decode_view() const noexcept
    {
        if(has_value)
            return {
                decode_view(key),
                decode_view(value)};
        return {
            decode_view(key)};
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
};

} // urls
} // boost

#include <boost/url/impl/param.hpp>

#endif
