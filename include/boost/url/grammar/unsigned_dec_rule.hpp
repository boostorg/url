//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_UNSIGNED_DEC_RULE_HPP
#define BOOST_URL_GRAMMAR_UNSIGNED_DEC_RULE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/result.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/string_view.hpp>
#include <boost/static_assert.hpp>
#include <limits>
#include <type_traits>

namespace boost {
namespace urls {
namespace grammar {

/** Rule for an unsigned decimal

    Extra leading zeroes are not allowed.

    @par BNF
    @code
    unsigned      = "0" / ( ["1"..."9"] *DIGIT )
    @endcode
*/
template<class Unsigned>
struct unsigned_dec_rule
{
    BOOST_STATIC_ASSERT(
        std::numeric_limits<
            Unsigned>::is_integer &&
        ! std::numeric_limits<
            Unsigned>::is_signed);

    struct value_type
    {
        Unsigned u;
        string_view s;
    };

    auto
    parse(
        char const*& it,
        char const* end
            ) const noexcept ->
        result<value_type>;
};

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/unsigned_dec_rule.hpp>

#endif
