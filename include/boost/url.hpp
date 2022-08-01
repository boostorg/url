//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_HPP
#define BOOST_URL_HPP

#include <boost/url/grammar.hpp>

#include <boost/url/authority_view.hpp>
#include <boost/url/error.hpp>
#include <boost/url/error_code.hpp>
#include <boost/url/host_type.hpp>
#include <boost/url/ipv4_address.hpp>
#include <boost/url/ipv6_address.hpp>
#include <boost/url/params.hpp>
#include <boost/url/params_encoded.hpp>
#include <boost/url/params_encoded_view.hpp>
#include <boost/url/params_view.hpp>
#include <boost/url/pct_encoding.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/query_param.hpp>
#include <boost/url/scheme.hpp>
#include <boost/url/segments.hpp>
#include <boost/url/segments_encoded.hpp>
#include <boost/url/segments_encoded_view.hpp>
#include <boost/url/segments_view.hpp>
#include <boost/url/static_pool.hpp>
#include <boost/url/static_url.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/url.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/urls.hpp>

#include <boost/url/rfc/absolute_uri_rule.hpp>
#include <boost/url/rfc/authority_rule.hpp>
#include <boost/url/rfc/gen_delim_chars.hpp>
#include <boost/url/rfc/ipv4_address_rule.hpp>
#include <boost/url/rfc/ipv6_address_rule.hpp>
#include <boost/url/rfc/path_rules.hpp>
#include <boost/url/rfc/pchars.hpp>
#include <boost/url/rfc/pct_encoded_rule.hpp>
#include <boost/url/rfc/query_rule.hpp>
#include <boost/url/rfc/relative_ref_rule.hpp>
#include <boost/url/rfc/reserved_chars.hpp>
#include <boost/url/rfc/sub_delim_chars.hpp>
#include <boost/url/rfc/unreserved_chars.hpp>
#include <boost/url/rfc/uri_rule.hpp>
#include <boost/url/rfc/uri_reference_rule.hpp>

#endif
