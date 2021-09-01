//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_ELEMENT_HPP
#define BOOST_URL_BNF_ELEMENT_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>

namespace boost {
namespace urls {
namespace bnf {

/** Adapt a List into an element
*/
template<class List>
class element
{
public:
    string_view const&
    operator*() const noexcept
    {
        return s_;
    }

    string_view const*
    operator->() const noexcept
    {
        return &s_;
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec)
    {
        List e_;
        auto it = e_.begin(
            start, end, ec);
        if(ec == error::end)
        {
            ec = {};
            return start;
        }
        if(ec)
            return start;
        for(;;)
        {
            it = e_.increment(
                it, end, ec);
            if(ec == error::end)
            {
                ec = {};
                break;
            }
            if(ec)
                return start;
        }
        s_ = string_view(
            start, it - start);
        return it;
    }

private:
    string_view s_;
};

} // bnf
} // urls
} // boost

#endif
