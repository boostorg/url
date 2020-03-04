//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_SRC_HPP
#define BOOST_URL_SRC_HPP

/*

This file is meant to be included once,
in a translation unit of the program.

*/

#include <boost/url/config.hpp>

#if defined(BOOST_URL_HEADER_ONLY)
# error Do not compile library source with the header only macro defined
#endif

#include <boost/url/impl/url_base.ipp>
#include <boost/url/impl/error.ipp>
#include <boost/url/impl/scheme.ipp>
#include <boost/url/impl/url_view.ipp>

#endif
