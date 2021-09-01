//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_SCHEME_HPP
#define BOOST_URL_RFC_SCHEME_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/char_set.hpp>

namespace boost {
namespace urls {
namespace rfc {

BOOST_URL_DECL
bool
is_scheme_char(char c) noexcept;

using scheme_char_set =
    bnf::char_set_function<&is_scheme_char>;

/** BNF for scheme

    @par BNF
    @code
    scheme      = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.1
*/
class scheme
{
public:
    using value_type = string_view;

    value_type const&
    value() const noexcept
    {
        return v_;
    }

    BOOST_URL_DECL
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec);

private:
    value_type v_;
};

} // rfc
} // urls
} // boost

#endif
