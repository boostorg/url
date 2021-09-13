//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RELATIVE_PART_BNF_HPP
#define BOOST_URL_RELATIVE_PART_BNF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/rfc/authority_bnf.hpp>
#include <boost/url/rfc/paths_bnf.hpp>

namespace boost {
namespace urls {

/** BNF for relative-part

    @par BNF
    @code
    relative-part = "//" authority path-abempty
                  / path-absolute
                  / path-noscheme
                  / path-empty
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-4.2
*/
struct relative_part_bnf
{
    bool has_authority = false;
    authority_bnf authority;
    parsed_path path;

    BOOST_URL_DECL
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        relative_part_bnf& t);
};

} // urls
} // boost

#endif
