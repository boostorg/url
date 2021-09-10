//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_PCT_ENCODING_TYPES_HPP
#define BOOST_URL_PCT_ENCODING_TYPES_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <cstddef>

namespace boost {
namespace urls {

/** A valid percent-encoded string
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

//------------------------------------------------

/** Options for removing percent-encoding from strings
*/
struct pct_decode_opts
{
    /** True if null characters are allowed in decoded output
    */
    bool allow_null = true;

    /** True if PLUS ('+') decodes into SP (space, ' ')

        @par Specification
        @li <a href="https://www.w3.org/TR/html401/interact/forms.html#h-17.13.4.1">
            application/x-www-form-urlencoded (w3.org)</a>
    */
    bool plus_to_space = true;

    /** True if decoding a non-normal string is an error
    */
    bool non_normal_is_error = false;
};

//------------------------------------------------

/** Options for applying percent-encoding to strings
*/
struct pct_encode_opts
{
    /** True if SP (space, ' ') encodes into PLUS ('+')

        @par Specification
        @li <a href="https://www.w3.org/TR/html401/interact/forms.html#h-17.13.4.1">
            application/x-www-form-urlencoded (w3.org)</a>
    */
    bool space_to_plus = false;
};

} // urls
} // boost

#endif
