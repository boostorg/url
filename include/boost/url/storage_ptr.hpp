//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_STORAGE_PTR_HPP
#define BOOST_URL_STORAGE_PTR_HPP

#include <boost/url/detail/config.hpp>
#include <boost/json/storage_ptr.hpp>

namespace boost {
namespace urls {

/** The type of storage_ptr used by the library
*/
using storage_ptr = ::boost::json::storage_ptr;

} // urls
} // boost

#endif
