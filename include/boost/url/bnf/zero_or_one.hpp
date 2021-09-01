//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_ZERO_OR_ONE_HPP
#define BOOST_URL_BNF_ZERO_OR_ONE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/optional.hpp>
#include <boost/type_traits/remove_cv.hpp>

namespace boost {
namespace urls {
namespace bnf {

/** BNF for an optional element

    This rule defines a sequence containing
    an optional element.
*/
template<
    class Element>
class zero_or_one
{
public:
    using value_type = optional<
        typename remove_cv<decltype(std
            ::declval<Element>(
                ).operator*())>::type>;

    value_type const&
    operator*() const noexcept
    {
        return v_;
    }

    value_type const*
    operator->() const noexcept
    {
        return &v_;
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec)
    {
        Element e;
        auto it = e.parse(
            start, end, ec);
        if(ec)
        {
            ec = {};
            v_ = boost::none;
            return start;
        }
        v_ = *e;
        return it;
    }

private:
    value_type v_;
};

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/repeat.hpp>

#endif
