//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_PCT_ENCODED_STR_HPP
#define BOOST_URL_RFC_PCT_ENCODED_STR_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>

namespace boost {
namespace urls {
namespace rfc {

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
template<std::uint8_t CharMask>
struct pct_encoded
{
    pct_encoded_str& v;

    template<std::uint8_t CharMask>
    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        pct_encoded<CharMask> const& t) noexcept;
};
    
} // rfc
} // urls
} // boost

#include <boost/url/rfc/impl/pct_encoded_str.hpp>

#endif
