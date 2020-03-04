//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_URL_HPP
#define BOOST_URL_URL_HPP

#include <boost/url/config.hpp>
#include <boost/url/basic_url.hpp>
#include <memory>

namespace boost {
namespace urls {

using url = basic_url<std::allocator<char>>;

} // urls
} // boost

#endif
