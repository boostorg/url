//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_STRING_HPP
#define BOOST_URL_STRING_HPP

#include <boost/url/detail/config.hpp>
#include <boost/utility/string_view.hpp>
#include <string>

namespace boost {
namespace urls {

/** The type of string_view used by the library
*/
using string_view = boost::string_view;

/// The string alias template return type for allocating member functions.
template<class Allocator>
using string_type =
    std::basic_string<char,
        std::char_traits<char>, Allocator>;

} // urls
} // boost

#endif
