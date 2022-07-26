//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_PCT_ENCODED_VIEW_HPP
#define BOOST_URL_DETAIL_PCT_ENCODED_VIEW_HPP

namespace boost {
namespace urls {

class pct_encoded_view;

namespace detail {

struct access
{
    // construct unchecked pct_encoded_view
    BOOST_URL_DECL
    static
    pct_encoded_view
    construct(
        string_view s,
        std::size_t n,
        pct_decode_opts const& opt = {}) noexcept;
};

} // detail
} // urls
} // boost

#endif
