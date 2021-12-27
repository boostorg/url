//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_CHARSETS_HPP
#define BOOST_URL_RFC_CHARSETS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/bnf/lut_chars.hpp>
#include <cstdint>

namespace boost {
namespace urls {

/** The unreserved character set type.

    Objects of this type are invocable
    with this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. This type satisfies
    the <em>CharSet</em> requirements.

    @par BNF
    @code
    unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.3"
        >2.3. Unreserved Characters (rfc3986)</a>

    @see
        @ref unreserved_chars
*/
#ifdef BOOST_URL_DOCS
using unreserved_chars_t = see_below;
#else
struct unreserved_chars_t : bnf::lut_chars
{
    constexpr
    unreserved_chars_t() noexcept
        : bnf::lut_chars(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789"
            "-._~")
    {
    }
};
#endif

/** The unreserved character set.

    The object is invocable with
    this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of this
    object satisfies the <em>CharSet</em>
    requirements.

    @par BNF
    @code
    unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.3"
        >2.3. Unreserved Characters (rfc3986)</a>

    @see
        @ref unreserved_chars_t
*/
constexpr unreserved_chars_t unreserved_chars{};

//------------------------------------------------

/** The reserved character set type.

    Objects of this type are invocable
    with this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. This type satisfies
    the <em>CharSet</em> requirements.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.3"
        >2.3. Unreserved Characters (rfc3986)</a>

    @see
        @ref reserved_chars,
        @ref unreserved_chars,
        @ref unreserved_chars_t.
*/
#ifdef BOOST_URL_DOCS
using reserved_chars_t = see_below;
#else
struct reserved_chars_t : bnf::lut_chars
{
    constexpr
    reserved_chars_t() noexcept
        : bnf::lut_chars(~unreserved_chars)
    {
    }
};
#endif

/** The reserved character set.

    The object is invocable with
    this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of this
    object satisfies the <em>CharSet</em>
    requirements.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.3"
        >2.3. Unreserved Characters (rfc3986)</a>

    @see
        @ref reserved_chars_t,
        @ref unreserved_chars,
        @ref unreserved_chars_t.
*/
constexpr reserved_chars_t reserved_chars{};

//------------------------------------------------

/** The gen-delims character set type.

    Objects of this type are invocable
    with this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. This type satisfies
    the <em>CharSet</em> requirements.

    @par BNF
    @code
    gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.2"
        >2.2. Reserved Characters (rfc3986)</a>

    @see
        @ref gen_delim_chars
*/
#ifdef BOOST_URL_DOCS
using gen_delim_chars_t = see_below;
#else
struct gen_delim_chars_t : bnf::lut_chars
{
    constexpr
    gen_delim_chars_t() noexcept
        : bnf::lut_chars(
            ":/?#[]@")
    {
    }
};
#endif

/** The gen-delims character set.

    The object is invocable with
    this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of this
    object satisfies the <em>CharSet</em>
    requirements.

    @par BNF
    @code
    gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.2"
        >2.2. Reserved Characters (rfc3986)</a>

    @see
        @ref gen_delim_chars_t
*/
constexpr gen_delim_chars_t gen_delim_chars{};

//------------------------------------------------

/** The sub-delims character set type.

    Objects of this type are invocable
    with this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. This type satisfies
    the <em>CharSet</em> requirements.

    @par BNF
    @code
    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.2"
        >2.2. Reserved Characters (rfc3986)</a>

    @see
        @ref subdelim_chars
*/
#ifdef BOOST_URL_DOCS
using subdelim_chars_t = see_below;
#else
struct subdelim_chars_t : bnf::lut_chars
{
    constexpr
    subdelim_chars_t() noexcept
        : bnf::lut_chars(
            "!$&()*+,;=\x27")
    {
    }
};
#endif

/** The sub-delims character set.

    The object is invocable with
    this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of this
    object satisfies the <em>CharSet</em>
    requirements.

    @par BNF
    @code
    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.2"
        >2.2. Reserved Characters (rfc3986)</a>

    @see
        @ref subdelim_chars_t
*/
constexpr subdelim_chars_t subdelim_chars{};

//------------------------------------------------

/** The pchars character set type.

    Objects of this type are invocable
    with this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. This type satisfies
    the <em>CharSet</em> requirements.

    @par BNF
    @code
    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref pchars
*/
#ifdef BOOST_URL_DOCS
using pchars_t = see_below;
#else
struct pchars_t : bnf::lut_chars
{
    constexpr
    pchars_t() noexcept
        : bnf::lut_chars(
            unreserved_chars +
            subdelim_chars +
            ':' + '@')
    {
    }
};
#endif

/** The pchar character set.

    The object is invocable with
    this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of this
    object satisfies the <em>CharSet</em>
    requirements.

    @par BNF
    @code
    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>

    @see
        @ref pchars_t
*/
constexpr pchars_t pchars{};

//------------------------------------------------

/** The query character set type.

    Objects of this type are invocable
    with this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. This type satisfies
    the <em>CharSet</em> requirements.

    @par BNF
    @code
    query         = *( pchar / "/" / "?" )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
        >3.4. Query (rfc3986)</a>

    @see
        @ref query_chars_t
*/
#ifdef BOOST_URL_DOCS
using query_chars_t = see_below;
#else
struct query_chars_t : bnf::lut_chars
{
    constexpr
    query_chars_t() noexcept
        : bnf::lut_chars(
            pchars + '/' + '?')
    {
    }
};
#endif

/** The query character set.

    The object is invocable with
    this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of this
    object satisfies the <em>CharSet</em>
    requirements.

    @par BNF
    @code
    query         = *( pchar / "/" / "?" )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
        >3.4. Query (rfc3986)</a>

    @see
        @ref query_chars_t
*/
constexpr query_chars_t query_chars{};

//------------------------------------------------

/** The fragment character set type.

    Objects of this type are invocable
    with this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. This type satisfies
    the <em>CharSet</em> requirements.

    @par BNF
    @code
    fragment      = *( pchar / "/" / "?" )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.5"
        >3.5. Fragment (rfc3986)</a>

    @see
        @ref fragment_chars
*/
#ifdef BOOST_URL_DOCS
using fragment_chars_t = see_below;
#else
struct fragment_chars_t : bnf::lut_chars
{
    constexpr
    fragment_chars_t() noexcept
        : bnf::lut_chars(
            pchars + '/' + '?')
    {
    }
};
#endif

/** The fragment character set.

    The object is invocable with
    this equivalent signature:

    @code
    bool( char ch ) const noexcept;
    @endcode

    The function object returns `true` when
    `ch` is a member of the character set,
    and `false` otherwise. The type of this
    object satisfies the <em>CharSet</em>
    requirements.

    @par BNF
    @code
    fragment      = *( pchar / "/" / "?" )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.5"
        >3.5. Fragment (rfc3986)</a>

    @see
        @ref fragment_chars_t
*/
constexpr fragment_chars_t fragment_chars{};

} // urls
} // boost

#endif
