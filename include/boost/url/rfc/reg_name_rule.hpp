//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_REG_NAME_RULE_HPP
#define BOOST_URL_RFC_REG_NAME_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/pct_encoding_types.hpp>
#include <boost/url/rfc/charsets.hpp>
#include <boost/url/grammar/parse_tag.hpp>

namespace boost {
namespace urls {

/** Rule for reg-name

    @par BNF
    @code
    reg-name    = *( unreserved / pct-encoded / "-" / ".")
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
        >3.2.2. Host (rfc3986)</a>
    @li <a href="https://www.rfc-editor.org/errata/eid4942"
        >Errata ID: 4942</a>
*/
struct reg_name_rule
{
    pct_encoded_str v;

    BOOST_URL_DECL
    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        error_code& ec,
        reg_name_rule& t) noexcept
    {
        parse(it, end, ec, t);
    }

private:
    BOOST_URL_DECL
    static
    void
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        reg_name_rule& t) noexcept;
};

} // urls
} // boost

#endif
