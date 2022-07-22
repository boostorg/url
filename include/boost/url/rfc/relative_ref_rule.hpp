//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_RELATIVE_REF_RULE_HPP
#define BOOST_URL_RFC_RELATIVE_REF_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/result.hpp>
#include <boost/url/rfc/fragment_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/rfc/relative_part_rule.hpp>

namespace boost {
namespace urls {

/** Rule for relative-ref

    @par BNF
    @code
    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.2
        >4.2. Relative Reference (rfc3986)</a>

    @see
        @ref fragment_part_rule,
        @ref query_part_rule,
        @ref relative_part_rule.
*/
struct relative_ref_rule_t
{
    struct value_type
    {
        decltype(relative_part_rule)::value_type relative_part;
        decltype(query_part_rule)::value_type query_part;
        decltype(fragment_part_rule)::value_type fragment_part;
    };

    BOOST_URL_DECL
    auto
    parse(
        char const*& it,
        char const* end
            ) const noexcept ->
        result<value_type>;
};

constexpr relative_ref_rule_t relative_ref_rule{};

} // urls
} // boost

#endif
