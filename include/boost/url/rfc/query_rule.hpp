//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_QUERY_RULE_HPP
#define BOOST_URL_RFC_QUERY_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/query_param.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/char_rule.hpp>
#include <boost/url/grammar/sequence_rule.hpp>
#include <boost/url/grammar/optional_rule.hpp>
#include <boost/url/grammar/range.hpp>
#include <cstddef>

namespace boost {
namespace urls {

/** Rule for query

    @par BNF
    @code
    query           = *( pchar / "/" / "?" )

    query-params    = query-param *( "&" query-param )
    query-param     = key [ "=" value ]
    key             = *qpchar
    value           = *( qpchar / "=" )
    qpchar          = unreserved
                    / pct-encoded
                    / "!" / "$" / "'" / "(" / ")"
                    / "*" / "+" / "," / ";"
                    / ":" / "@" / "/" / "?"
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
        >3.4. Query (rfc3986)</a>

    @see
        @ref query_part_rule.
*/
struct query_rule
{
    using value_type =
        grammar::range<
            query_param_view>;

    BOOST_URL_DECL
    result<value_type>
    parse(
        char const*& it,
        char const* end
            ) const noexcept;

private:
    BOOST_URL_DECL
    result<query_param_view>
    begin(
        char const*& it,
        char const* end
            ) const noexcept;

    BOOST_URL_DECL
    result<query_param_view>
    increment(
        char const*& it,
        char const* end
            ) const noexcept;

private:
    result<query_param_view>
    parse_query_param(
        char const*& it,
        char const* end
            ) const noexcept;
};

//------------------------------------------------

/** Rule for query-part

    @par BNF
    @code
    query-part      = [ "?" query ]

    query           = *( pchar / "/" / "?" )
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.4"
        >3.4. Query (rfc3986)</a>

    @see
        @ref query_rule.
*/
auto constexpr query_part_rule =
    grammar::optional_rule(
        grammar::sequence_rule(
            grammar::char_rule('?'),
            query_rule{}));

} // urls
} // boost

#endif
