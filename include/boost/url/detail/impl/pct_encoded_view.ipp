//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_PCT_ENCODED_VIEW_IPP
#define BOOST_URL_DETAIL_IMPL_PCT_ENCODED_VIEW_IPP

#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/detail/pct_encoded_view.hpp>

namespace boost {
namespace urls {

class pct_encoded_view;

namespace detail {

// construct unchecked pct_encoded_view
pct_encoded_view
access::
construct(
    string_view s,
    std::size_t n,
    pct_decode_opts const& opt) noexcept
{
    // AFREITAS: move that inline
    return pct_encoded_view(s, n, opt);
}

} // detail
} // urls
} // boost

#endif
