//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODING_HPP
#define BOOST_URL_IMPL_PCT_ENCODING_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/url/bnf/detail/char_set.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

template<class CharSet>
std::size_t
pct_decode_size(
    string_view s,
    error_code& ec,
    CharSet const& cs,
    pct_decode_opts const& opt) noexcept
{
    BOOST_STATIC_ASSERT(
        bnf::detail::is_char_set_pred<
            CharSet>::value);
    ec = {};
    auto it = s.data();
    auto const end = it + s.size();
    auto const n =
        detail::pct_decode_size(
            it, end, ec, cs, opt);
    if(ec.failed())
        return n;
    if(it != end)
    {
        ec = error::leftover_input;
        return n;
    }
    return n;
}

template<
    class CharSet,
    class Allocator>
string_type<Allocator>
pct_decode(
    string_view s,
    CharSet const& cs,
    pct_decode_opts const& opt,
    Allocator const& a)
{
    BOOST_STATIC_ASSERT(
        bnf::detail::is_char_set_pred<
            CharSet>::value);
    string_type<Allocator> r(a);
    error_code ec;
    auto const n =
        pct_decode_size(
            s, ec, cs, opt);
    detail::maybe_throw(ec,
        BOOST_CURRENT_LOCATION);
    r.resize(n);
    detail::pct_decode_unchecked(
        &r[0], &r[0] + r.size(),
            s, opt);
    return r;
}

//------------------------------------------------

template<class CharSet>
std::size_t
pct_encode_size(
    string_view s,
    CharSet const& cs,
    pct_encode_opts const& opt) noexcept
{
    BOOST_STATIC_ASSERT(
        bnf::detail::is_char_set_pred<
            CharSet>::value);
    std::size_t n = 0;
    auto it = s.data();
    auto const end = it + s.size();
    if(! opt.space_to_plus)
    {
        while(it != end)
        {
            if(cs(*it))
                ++n;
            else
                n += 3;
            ++it;
        }
        return n;
    }
    // If you are converting space
    // to plus, then space should
    // be in the list of reserved
    // characters!
    BOOST_ASSERT(! cs(' '));
    while(it != end)
    {
        if(*it == ' ')
            ++n;
        else if(cs(*it))
            ++n;
        else
            n += 3;
        ++it;
    }
    return n;
}

template<
    class CharSet,
    class Allocator>
string_type<Allocator>
pct_encode(
    string_view s,
    CharSet const& cs,
    pct_encode_opts const& opt,
    Allocator const& a)
{
    BOOST_STATIC_ASSERT(
        bnf::detail::is_char_set_pred<
            CharSet>::value);
    string_type<Allocator> r(a);
    r.resize(
        pct_encode_size(s, cs, opt));
    auto const end = &r[0] + r.size();
    auto it = detail::pct_encode(
        &r[0], end, s, opt, cs);
    BOOST_ASSERT(it == end);
    return r;
}

} // urls
} // boost

#endif
