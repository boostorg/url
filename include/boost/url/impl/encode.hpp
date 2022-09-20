//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_IMPL_ENCODE_HPP
#define BOOST_URL_IMPL_ENCODE_HPP

#include <boost/url/detail/encode.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/encode_opts.hpp>
#include <boost/url/grammar/charset.hpp>
#include <boost/url/grammar/hexdig_chars.hpp>
#include <boost/url/grammar/type_traits.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

template<class CharSet>
std::size_t
encoded_size(
    string_view s,
    encode_opts const& opt,
    CharSet const& allowed) noexcept
{
/*  If you get a compile error here, it
    means that the value you passed does
    not meet the requirements stated in
    the documentation.
*/
    static_assert(
        grammar::is_charset<CharSet>::value,
        "Type requirements not met");

    return detail::encoded_size_impl(
        s.data(),
        s.data() + s.size(),
        opt,
        allowed);
}

//------------------------------------------------


//------------------------------------------------

template<
    class StringToken,
    class CharSet>
BOOST_URL_STRTOK_RETURN
encode(
    string_view s,
    StringToken&& token,
    encode_opts const& opt,
    CharSet const& allowed) noexcept
{
    // CharSet must satisfy is_charset
    BOOST_STATIC_ASSERT(
        grammar::is_charset<CharSet>::value);
    auto const n =
        encoded_size(s, opt, allowed);
    auto p = token.prepare(n);
    if(n > 0)
        detail::encode_unchecked(
            p, p + n, s, opt, allowed);
    return token.result();
}

} // urls
} // boost

#endif
