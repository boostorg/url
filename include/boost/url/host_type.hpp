//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_HOST_TYPE_HPP
#define BOOST_URL_HOST_TYPE_HPP

#include <boost/url/config.hpp>

namespace boost {
namespace url {

/** Identifies the type of host in a URL.
*/
enum class host_type
{
    none,
    name,
    ipv4,
    ipv6
};

} // url
} // boost

#endif
