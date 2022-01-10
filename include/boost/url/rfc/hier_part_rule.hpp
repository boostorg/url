//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_HIER_PART_HPP
#define BOOST_URL_RFC_HIER_PART_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/rfc/authority_rule.hpp>
#include <boost/url/rfc/paths_rule.hpp>
#include <boost/url/grammar/parse_tag.hpp>

namespace boost {
namespace urls {

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
struct hier_part_rule
{
    bool has_authority = false;
    authority_rule authority;
    parsed_path path;

    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        error_code& ec,
        hier_part_rule& t) noexcept
    {
        return parse(it, end, ec, t);
    }

private:
    BOOST_URL_DECL
    static
    void
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        hier_part_rule& t) noexcept;
};

} // urls
} // boost

#endif
