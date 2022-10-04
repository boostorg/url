//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_IMPL_DECODE_HPP
#define BOOST_URL_IMPL_DECODE_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/grammar/hexdig_chars.hpp>
#include <boost/url/grammar/type_traits.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

template< class StringToken >
auto
decode(
    BOOST_URL_PCT_STRING_VIEW s,
    decode_opts const& opt,
    StringToken&& token) noexcept ->
        BOOST_URL_STRTOK_RETURN
{
    return s.decode(opt, std::move(token));
}

} // urls
} // boost

#endif
