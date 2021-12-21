//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_QUERY_BNF_HPP
#define BOOST_URL_RFC_QUERY_BNF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/pct_encoding_types.hpp>
#include <boost/url/params_value_type.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/bnf/range.hpp>
#include <cstddef>

namespace boost {
namespace urls {

struct query_param
{
    pct_encoded_str key;
    pct_encoded_str value;
    bool has_value = false;

    friend
    bool
    operator==(
        query_param const& t0,
        query_param const& t1) noexcept
    {
        return
            t0.key.str == t1.key.str &&
            t0.has_value == t1.has_value &&
            (! t0.has_value ||
                t0.value.str == t1.value.str);
    }
        
    friend
    bool
    operator!=(
        query_param const& t0,
        query_param const& t1) noexcept
    {
        return !(t0 == t1);
    }
};

/** BNF for query

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

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.4
*/
struct query_bnf
{
    using value_type =
        bnf::range<params_value_type>;

    value_type v;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        query_bnf& t) noexcept;

private:
    struct key_chars;
    struct value_chars;

    BOOST_URL_DECL
    static
    bool
    begin(
        char const*& it,
        char const* const end,
        error_code& ec,
        params_value_type& t) noexcept;

    BOOST_URL_DECL
    static
    bool
    increment(
        char const*& it,
        char const* const end,
        error_code& ec,
        params_value_type& t) noexcept;
};

/** BNF for query-part

    @par BNF
    @code
    query-part      = [ "?" query ]

    query           = *( pchar / "/" / "?" )
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.4
*/
struct query_part_bnf
{
    bool has_query;
    query_bnf query;
    string_view query_part;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        query_part_bnf& t);
};

} // urls
} // boost

#endif
