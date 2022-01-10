//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_CHAR_LITERAL_HPP
#define BOOST_URL_GRAMMAR_CHAR_LITERAL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/parse_tag.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Rule for a literal character
*/
template<char Ch>
struct char_literal
{
    char const* p;

    friend
    void
    tag_invoke(
        parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        char_literal& t) noexcept
    {
        if(it == end)
        {
            ec = error::incomplete;
            return;
        }
        if(*it != Ch)
        {
            ec = error::syntax;
            return;
        }
        t.p = it++;
    }
};

} // grammar
} // urls
} // boost

#endif
