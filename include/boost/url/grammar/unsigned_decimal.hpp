//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_UNSIGNED_DECIMAL_HPP
#define BOOST_URL_GRAMMAR_UNSIGNED_DECIMAL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/parse_tag.hpp>
#include <boost/url/string_view.hpp>
#include <boost/static_assert.hpp>
#include <limits>
#include <type_traits>

namespace boost {
namespace urls {
namespace grammar {

/** Rule for an unsigned decimal

    @par BNF
    @code
    unsigned      = "0" / ( ["1"..."9"] *DIGIT )
    @endcode
*/
template<class Unsigned>
struct unsigned_decimal
{
    Unsigned u;
    string_view s;

    friend
    void
    tag_invoke(
        parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        unsigned_decimal& t) noexcept
    {
        parse(it, end, ec, t);
    }

private:
    BOOST_STATIC_ASSERT(
        std::numeric_limits<
            Unsigned>::is_integer &&
        ! std::numeric_limits<
            Unsigned>::is_signed);

    static
    void
    parse(
        char const*& it,
        char const* end,
        error_code& ec,
        unsigned_decimal& t) noexcept;
};

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/unsigned_decimal.hpp>

#endif
