//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_DECODE_VIEW_HPP
#define BOOST_URL_DETAIL_DECODE_VIEW_HPP

#include <boost/url/decode_opts.hpp>
#include <utility>

namespace boost {
namespace urls {

class decode_view;

namespace detail {

struct access
{
    // construct unchecked decode_view
    BOOST_URL_DECL
    static
    decode_view
    construct(
        string_view s,
        std::size_t n,
        decode_opts const& opt = {}) noexcept;
};

//------------------------------------------------

struct make_decode_view_t
{
    template<class... Args>
    decode_view
    operator()(Args&&... args) const;
};

constexpr
make_decode_view_t
make_decode_view{};

//------------------------------------------------

} // detail
} // urls
} // boost

#endif
