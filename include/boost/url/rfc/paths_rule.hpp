//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PATHS_RULE_HPP
#define BOOST_URL_RFC_PATHS_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/grammar/not_empty_rule.hpp>
#include <boost/url/grammar/range.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <cstddef>

namespace boost {
namespace urls {

/** Information about a parsed path
*/
struct parsed_path
{
    /** The encoded string representing the path
    */
    string_view path;

    /** The number of segments in the path
    */
    std::size_t count = 0;
};

/** Rule for path-abempty grammar

    @par BNF
    @code
    path-abempty  = *( "/" segment )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>
*/
#ifdef BOOST_URL_DOCS
using path_abempty_rule = __see_below__;
#else
struct path_abempty_rule
{
    using value_type =
        grammar::range<
            pct_encoded_view>;

    BOOST_URL_DECL
    result<value_type>
    parse(
        char const*& it,
        char const* end
            ) const noexcept;

private:
    BOOST_URL_DECL
    result<pct_encoded_view>
    begin(
        char const*& it,
        char const* end
            ) const noexcept;

    BOOST_URL_DECL
    result<pct_encoded_view>
    increment(
        char const*& it,
        char const* end
            ) const noexcept;
};
#endif

//------------------------------------------------

/** Rule for path-absolute grammar.

    @par BNF
    @code
    path-absolute = "/" [ segment-nz *( "/" segment ) ]
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>
*/
#ifdef BOOST_URL_DOCS
using path_absolute_rule = __see_below__;
#else
struct path_absolute_rule
{
    using value_type =
        grammar::range<
            pct_encoded_view>;

    BOOST_URL_DECL
    result<value_type>
    parse(
        char const*& it,
        char const* end
            ) const noexcept;

private:
    BOOST_URL_DECL
    result<pct_encoded_view>
    begin(
        char const*& it,
        char const* end
            ) const noexcept;

    BOOST_URL_DECL
    result<pct_encoded_view>
    increment(
        char const*& it,
        char const* end
            ) const noexcept;
};
#endif

//------------------------------------------------

/** Rule for path-noscheme grammar.

    @par BNF
    @code
    path-noscheme = segment-nz-nc *( "/" segment )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>
*/
#ifdef BOOST_URL_DOCS
using path_noscheme_rule = __see_below__;
#else
struct path_noscheme_rule
{
    using value_type =
        grammar::range<
            pct_encoded_view>;

    BOOST_URL_DECL
    result<value_type>
    parse(
        char const*& it,
        char const* end
            ) const noexcept;

private:
    BOOST_URL_DECL
    result<pct_encoded_view>
    begin(
        char const*& it,
        char const* end
            ) const noexcept;

    BOOST_URL_DECL
    result<pct_encoded_view>
    increment(
        char const*& it,
        char const* end
            ) const noexcept;
};
#endif

//------------------------------------------------

/** Rule for path-rootless grammar.

    @par Example
    @code
    path_rootless_rule t;
    bool success = parse( it, end, ec, t);
    @endcode

    @par BNF
    @code
    path-rootless = segment-nz *( "/" segment )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3"
        >3.3. Path (rfc3986)</a>
*/
#ifdef BOOST_URL_DOCS
using path_rootless_rule = __see_below__;
#else
struct path_rootless_rule
{
    using value_type =
        grammar::range<
            pct_encoded_view>;

    BOOST_URL_DECL
    result<value_type>
    parse(
        char const*& it,
        char const* end
            ) const noexcept;

#ifndef BOOST_URL_DOCS
//private:
    //friend struct detail::segments_iterator_impl;
    //friend struct detail::segments_encoded_iterator_impl;

    BOOST_URL_DECL
    result<pct_encoded_view>
    begin(
        char const*& it,
        char const* end
            ) const noexcept;

    BOOST_URL_DECL
    result<pct_encoded_view>
    increment(
        char const*& it,
        char const* end
            ) const noexcept;
#endif
};
#endif

} // urls
} // boost

#endif
