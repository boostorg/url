//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_ERROR_HPP
#define BOOST_URL_ERROR_HPP

#include <boost/url/config.hpp>

namespace boost {
namespace url {

/** Error codes returned by URL operations
*/
enum class error
{
    /// An input did not match a structural element (soft error)
    mismatch = 1,

    /// A syntax error occurred
    syntax,

    /// The parser encountered an invalid input
    invalid
};

enum class condition
{
    parse_error = 1
};

} // url
} // boost

#include <boost/url/impl/error.hpp>
#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/error.ipp>
#endif

#endif
