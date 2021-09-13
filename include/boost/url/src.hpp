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

#include <boost/url/detail/impl/except.ipp>
#include <boost/url/detail/impl/parse.ipp>
#include <boost/url/detail/impl/pct_encoding.ipp>

#include <boost/url/impl/error.ipp>
#include <boost/url/impl/ipv4_address.ipp>
#include <boost/url/impl/ipv6_address.ipp>
#include <boost/url/impl/path_view.ipp>
#include <boost/url/impl/query_params_view.ipp>
#include <boost/url/impl/scheme.ipp>
#include <boost/url/impl/static_pool.ipp>
#include <boost/url/impl/static_url.ipp>
#include <boost/url/impl/url.ipp>
#include <boost/url/impl/url_view.ipp>

#include <boost/url/rfc/impl/absolute_uri_bnf.ipp>
#include <boost/url/rfc/impl/authority_bnf.ipp>
#include <boost/url/rfc/impl/char_sets.ipp>
#include <boost/url/rfc/impl/fragment_bnf.ipp>
#include <boost/url/rfc/impl/fragment_part_bnf.ipp>
#include <boost/url/rfc/impl/hier_part_bnf.ipp>
#include <boost/url/rfc/impl/host_bnf.ipp>
#include <boost/url/rfc/impl/ip_literal_bnf.ipp>
#include <boost/url/rfc/impl/ipv_future_bnf.ipp>
#include <boost/url/rfc/impl/ipv4_address_bnf.ipp>
#include <boost/url/rfc/impl/ipv6_address_bnf.ipp>
#include <boost/url/rfc/impl/paths_bnf.ipp>
#include <boost/url/rfc/impl/port_bnf.ipp>
#include <boost/url/rfc/impl/port_part_bnf.ipp>
#include <boost/url/rfc/impl/query_bnf.ipp>
#include <boost/url/rfc/impl/query_part_bnf.ipp>
#include <boost/url/rfc/impl/relative_part_bnf.ipp>
#include <boost/url/rfc/impl/relative_ref_bnf.ipp>
#include <boost/url/rfc/impl/scheme_bnf.ipp>
#include <boost/url/rfc/impl/scheme_part_bnf.ipp>
#include <boost/url/rfc/impl/uri_bnf.ipp>
#include <boost/url/rfc/impl/userinfo_bnf.ipp>

#endif
