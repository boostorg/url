//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_SCHEME_RULE_HPP
#define BOOST_URL_RFC_SCHEME_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/parse_tag.hpp>

namespace boost {
namespace urls {

/** BNF for scheme

    @par BNF
    @code
    scheme      = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1"
        >3.1. Scheme (rfc3986)</a>

    @see
        @ref scheme.
*/
struct scheme_rule
{
    string_view scheme;
    urls::scheme scheme_id;

    BOOST_URL_DECL
    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        error_code& ec,
        scheme_rule& t) noexcept;
};

/** BNF for scheme-part

    @par BNF
    @code
    scheme-part     = scheme ":"

    scheme          = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.1"
        >3.1. Scheme (rfc3986)</a>

    @see
        @ref scheme.
*/
struct scheme_part_rule
{
    string_view scheme;
    urls::scheme scheme_id =
        urls::scheme::none;
    string_view scheme_part;

    BOOST_URL_DECL
    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        error_code& ec,
        scheme_part_rule& t) noexcept;
};

} // urls
} // boost

#endif
