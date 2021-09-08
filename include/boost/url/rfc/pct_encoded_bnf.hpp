//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PCT_ENCODED_BNF_HPP
#define BOOST_URL_PCT_ENCODED_BNF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/detail/char_set.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

/** Base type for a percent-encoded BNF element
*/
struct pct_encoded_str
{
    /** A string holding the encoded characters
    */
    string_view str;

    /** The number of bytes needed to hold the decoded string
    */
    std::size_t decoded_size;
};

/** BNF for a string of percent-encoded characters from a character set

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-2.1
*/
template<class CharSet>
struct pct_encoded_bnf
{
    pct_encoded_str& v;

    BOOST_STATIC_ASSERT(
        bnf::detail::is_char_set_pred<
            CharSet>::value);

    template<class CharSet_>
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded_bnf<CharSet_> const& t) noexcept;
};

} // urls
} // boost

#include <boost/url/rfc/impl/pct_encoded_bnf.hpp>

#endif
