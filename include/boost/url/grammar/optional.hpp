//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_OPTIONAL_HPP
#define BOOST_URL_GRAMMAR_OPTIONAL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Rule for an optional element.
*/
template<class R>
struct optional
{
    bool has_value = false;
    R v;

    explicit
    operator bool() const noexcept
    {
        return has_value;
    }

    R const&
    operator*() const noexcept
    {
        BOOST_ASSERT(has_value);
        return v;
    }

    friend
    void
    tag_invoke(
        parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        optional& t)
    {
        auto const start = it;
        if(! parse(
            it, end, ec, t.v))
        {
            if(ec == condition::fatal)
                return;
            ec = {};
            it = start;
            t.has_value = false;
            return;
        }
        t.has_value = true;
    }
};

} // bnf
} // urls
} // boost

#endif
