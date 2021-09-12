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

#include <boost/url/rfc/fragment_part_bnf.hpp>
#include <boost/url/rfc/query_part_bnf.hpp>
#include <boost/url/rfc/scheme_part_bnf.hpp>

namespace boost {
namespace urls {
namespace detail {

void
apply(
    parts& p,
    scheme_part_bnf const& t) noexcept;

void
apply(
    parts& p,
    host_bnf const& h) noexcept;

void
apply(
    parts& p,
    authority_bnf const& t) noexcept;

void
apply_path(
    parts& p,
    string_view path,
    std::size_t path_count) noexcept;

void
apply(
    parts& p,
    query_part_bnf const& t) noexcept;

void
apply(
    parts& p,
    fragment_part_bnf const& t) noexcept;

} // detail
} // urls
} // boost

#endif
