//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_DETAIL_IMPL_ENCODE_HPP
#define BOOST_URL_DETAIL_IMPL_ENCODE_HPP

#include <boost/url/error.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/hexdig_chars.hpp>
#include <boost/url/grammar/type_traits.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {
namespace detail {

template<class CharSet>
std::size_t
encode(
    char* dest,
    char const* const end,
    string_view s,
    encode_opts const& opt,
    CharSet const& allowed)
{
    /* If you get a compile error here, it
    means that the value you passed does
    not meet the requirements stated in
    the documentation. */
    static_assert(
        grammar::is_charset<CharSet>::value,
        "Type requirements not met");

    return detail::encode_impl(
        dest,
        end,
        s.data(),
        s.data() + s.size(),
        opt,
        allowed);
}

} // detail
} // urls
} // boost

#endif
