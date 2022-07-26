//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
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

// MUST COME FIRST
#ifndef BOOST_URL_SOURCE
#define BOOST_URL_SOURCE
#endif

// We include this in case someone is
// using src.hpp as their main header file
#include <boost/url.hpp>

#include <boost/url/detail/impl/any_path_iter.ipp>
#include <boost/url/detail/impl/any_query_iter.ipp>
#include <boost/url/detail/impl/copied_strings.ipp>
#include <boost/url/detail/impl/except.ipp>
#include <boost/url/detail/impl/normalize.ipp>
#include <boost/url/detail/impl/path.ipp>
#include <boost/url/detail/impl/pct_encoding.ipp>
#include <boost/url/detail/impl/remove_dot_segments.ipp>
#include <boost/url/detail/impl/params_encoded_iterator_impl.ipp>
#include <boost/url/detail/impl/params_iterator_impl.ipp>
#include <boost/url/detail/impl/pct_encoded_view.ipp>
#include <boost/url/detail/impl/segments_encoded_iterator_impl.ipp>
#include <boost/url/detail/impl/segments_iterator_impl.ipp>

#include <boost/url/impl/authority_view.ipp>
#include <boost/url/impl/error.ipp>
#include <boost/url/impl/ipv4_address.ipp>
#include <boost/url/impl/ipv6_address.ipp>
#include <boost/url/impl/params.ipp>
#include <boost/url/impl/params_encoded.ipp>
#include <boost/url/impl/params_encoded_view.ipp>
#include <boost/url/impl/params_view.ipp>
#include <boost/url/impl/pct_encoded_view.ipp>
#include <boost/url/impl/pct_encoding.ipp>
#include <boost/url/impl/query_param.ipp>
#include <boost/url/impl/scheme.ipp>
#include <boost/url/impl/segments.ipp>
#include <boost/url/impl/segments_encoded.ipp>
#include <boost/url/impl/segments_encoded_view.ipp>
#include <boost/url/impl/segments_view.ipp>
#include <boost/url/impl/static_pool.ipp>
#include <boost/url/impl/static_url.ipp>
#include <boost/url/impl/url.ipp>
#include <boost/url/impl/url_view.ipp>

#include <boost/url/grammar/impl/error.ipp>

#include <boost/url/rfc/impl/absolute_uri_rule.ipp>
#include <boost/url/rfc/impl/authority_rule.ipp>
#include <boost/url/rfc/impl/fragment_rule.ipp>
#include <boost/url/rfc/impl/hier_part_rule.ipp>
#include <boost/url/rfc/impl/host_rule.ipp>
#include <boost/url/rfc/impl/ip_literal_rule.ipp>
#include <boost/url/rfc/impl/ipv_future_rule.ipp>
#include <boost/url/rfc/impl/paths_rule.ipp>
#include <boost/url/rfc/impl/port_rule.ipp>
#include <boost/url/rfc/impl/query_rule.ipp>
#include <boost/url/rfc/impl/reg_name_rule.ipp>
#include <boost/url/rfc/impl/relative_part_rule.ipp>
#include <boost/url/rfc/impl/relative_ref_rule.ipp>
#include <boost/url/rfc/impl/scheme_rule.ipp>
#include <boost/url/rfc/impl/uri_rule.ipp>
#include <boost/url/rfc/impl/uri_reference_rule.ipp>
#include <boost/url/rfc/impl/userinfo_rule.ipp>

#endif
