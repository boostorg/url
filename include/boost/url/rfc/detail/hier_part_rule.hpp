//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_DETAIL_HIER_PART_RULE_HPP
#define BOOST_URL_RFC_DETAIL_HIER_PART_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/rfc/authority_rule.hpp>
#include <boost/url/grammar/range_rule.hpp>

namespace boost {
namespace urls {
namespace detail {

/** Rule for hier-part

    @par BNF
    @code
    hier-part     = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3"
        >3. Syntax Components (rfc3986)</a>
*/
struct hier_part_rule_t
{
    struct value_type
    {
        bool has_authority = false;
        authority_view authority;

        // VFALCO This doesn't belong here
        grammar::range<pct_encoded_view> path;
    };

    BOOST_URL_DECL
    auto
    parse(
        char const*& it,
        char const* const end
            ) const noexcept ->
        result<value_type>;
};

constexpr hier_part_rule_t hier_part_rule{};

} // detail
} // urls
} // boost

#endif
