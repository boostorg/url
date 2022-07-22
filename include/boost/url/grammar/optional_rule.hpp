//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_OPTIONAL_RULE_HPP
#define BOOST_URL_GRAMMAR_OPTIONAL_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/optional.hpp>
#include <boost/url/result.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace grammar {

/** Rule for an optional grammar element
*/
template<class R>
class optional_rule_t
{
public:
    using value_type = optional<
        typename R::value_type>;

    auto
    parse(
        char const*& it,
        char const* end) const ->
            result<value_type>
    {
        auto const it0 = it;
        auto rv = r_.parse(it, end);
        if(! rv.has_error())
            return value_type(*rv);
        it = it0;
        return value_type();
    }

    template<class R_>
    friend
    constexpr
    auto
    optional_rule(
        R_ const& r) ->
            optional_rule_t<R_>;

private:
    constexpr
    optional_rule_t(
        R const& r) noexcept
        : r_(r)
    {
    }

    R const r_;
};

//------------------------------------------------

template<class R>
auto
constexpr
optional_rule(
    R const& r) ->
        optional_rule_t<R>
{
    return { r };
}

} // grammar
} // urls
} // boost

#endif
