//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PCT_ENCODED_RULE_HPP
#define BOOST_URL_RFC_PCT_ENCODED_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/pct_encoding.hpp>
#include <boost/url/result.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/detail/charset.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

/** Rule for a string of percent-encoded characters from a character set

    @tparam CharSet The character set indicating
    which characters are unreserved. Any character
    not in this set will be considered reserved.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-2.1">
        2.1. Percent-Encoding (rfc3986)</a>
*/
template<class CharSet>
struct pct_encoded_rule_t
{
    using value_type = pct_encoded_view;

    template<class CharSet_>
    friend
    constexpr
    auto
    pct_encoded_rule(
        CharSet_ const& cs) noexcept ->
            pct_encoded_rule_t<CharSet_>;

    result<value_type>
    parse(
        char const*& it,
        char const* end) const noexcept;

private:
    CharSet cs_;

    constexpr
    pct_encoded_rule_t(
        CharSet const& cs) noexcept
        : cs_(cs)
    {
    }
};

template<class CharSet>
constexpr
auto
pct_encoded_rule(
    CharSet const& cs) noexcept ->
        pct_encoded_rule_t<CharSet>
{
    // If an error occurs here it means that
    // the value of your type does not meet
    // the requirements. Please check the
    // documentation!
    static_assert(
        grammar::is_charset<CharSet>::value,
        "CharSet requirements not met");

    return pct_encoded_rule_t<CharSet>(cs);
}

} // urls
} // boost

#include <boost/url/rfc/impl/pct_encoded_rule.hpp>

#endif
