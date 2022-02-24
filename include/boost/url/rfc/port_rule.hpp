//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PORT_RULE_HPP
#define BOOST_URL_RFC_PORT_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/parse_tag.hpp>
#include <cstdint>

namespace boost {
namespace urls {

/** Rule for port

    @par BNF
    @code
    port          = *DIGIT
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
        >3.2.2. Host (rfc3986)</a>

    @see
        @ref port_part_rule.
*/
struct port_rule
{
    using number_type =
        std::uint16_t;

    string_view str;
    std::uint16_t number;
    bool has_number = false;

    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        error_code& ec,
        port_rule& t) noexcept
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
        port_rule& t) noexcept;

};

/** Rule for port-part

    @par BNF
    @code
    port-part       = [ ":" port ]

    port            = *DIGIT
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.2.2"
        >3.2.2. Host (rfc3986)</a>

    @see
        @ref port_rule.
*/
struct port_part_rule
{
    using number_type =
        std::uint16_t;

    bool has_port = false;
    string_view port;
    bool has_number = false;
    std::uint16_t port_number = 0;

    friend
    void
    tag_invoke(
        grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        error_code& ec,
        port_part_rule& t)
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
        port_part_rule& t) noexcept;
};

} // urls
} // boost

#endif
