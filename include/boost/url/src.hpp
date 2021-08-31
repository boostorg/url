//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SRC_HPP
#define BOOST_URL_SRC_HPP

/*

This file is meant to be included once,
in a translation unit of the program.

*/

#ifndef BOOST_URL_SOURCE
#define BOOST_URL_SOURCE
#endif

// We include this in case someone is
// using src.hpp as their main header file
#include <boost/url.hpp>

#include <boost/url/config.hpp>

#include <boost/url/detail/impl/except.ipp>

#include <boost/url/impl/error.ipp>
#include <boost/url/impl/scheme.ipp>
#include <boost/url/impl/url.ipp>
#include <boost/url/impl/url_view.ipp>

#include <boost/url/bnf/impl/origin_form.ipp>
#include <boost/url/bnf/impl/path_abempty.ipp>
#include <boost/url/bnf/impl/path_absolute.ipp>
#include <boost/url/bnf/impl/path_noscheme.ipp>
#include <boost/url/bnf/impl/path_rootless.ipp>
#include <boost/url/bnf/impl/pchar.ipp>
#include <boost/url/bnf/impl/pct_encoded.ipp>
#include <boost/url/bnf/impl/scheme.ipp>
#include <boost/url/bnf/impl/segment.ipp>
#include <boost/url/bnf/impl/userinfo.ipp>

#endif
