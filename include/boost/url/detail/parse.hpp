 //
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_PARSE_HPP
#define BOOST_URL_DETAIL_PARSE_HPP

#include <boost/url/detail/parts.hpp>
#include <boost/url/bnf/range.hpp>
#include <boost/url/rfc/authority_bnf.hpp>
#include <boost/url/rfc/host_bnf.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <boost/url/rfc/detail/query_params_bnf.hpp>

namespace boost {
namespace urls {
namespace detail {

void
apply_host(
    parts& p,
    host_bnf const& h) noexcept;

void
apply_authority(
    parts& p,
    optional<
        authority_bnf> const& t) noexcept;

void
apply_path(
    parts& p,
    bnf::range<
        pct_encoded_str> const& t) noexcept;

void
apply_query(parts& p,
    optional<bnf::range<
        query_param>> const& t) noexcept;

void
apply_fragment(
    parts& p,
    optional<
        pct_encoded_str> const& t) noexcept;

} // detail
} // urls
} // boost

#endif
