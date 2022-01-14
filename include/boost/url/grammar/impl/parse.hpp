//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_IMPL_PARSE_HPP
#define BOOST_URL_GRAMMAR_IMPL_PARSE_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/grammar/error.hpp>
#include <utility>

namespace boost {
namespace urls {
namespace grammar {

inline
void
tag_invoke(
    parse_tag const&,
    char const*& it,
    char const* const end,
    error_code& ec,
    char ch) noexcept
{
    if(it == end)
    {
        // end of input
        ec = error::incomplete;
        return;
    }
    if(*it != ch)
    {
        // expected ch
        ec = error::syntax;
        return;
    }
    ++it;
}

//------------------------------------------------

namespace detail {

inline
void
parse_impl(
    char const*&,
    char const*,
    error_code&) noexcept
{
}

// this is here to avoid
// having to clear `ec`
// for every call
template<
    class R0,
    class... Rn>
void
parse_impl(
    char const*& it,
    char const* const end,
    error_code& ec,
    R0&& r0,
    Rn&&... rn)
{
    using grammar::tag_invoke;
    tag_invoke(
        parse_tag{},
        it, end, ec,
        std::forward<R0>(r0));
    if(ec.failed())
        return;
    parse_impl(
        it, end, ec,
        std::forward<Rn>(rn)...);
}

} // detail

template<class... Rn>
bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    Rn&&... rn)
{
    ec = {};
    detail::parse_impl(
        it, end, ec,
        std::forward<Rn>(rn)...);
    return ! ec.failed();
}

//------------------------------------------------

template<class... Rn>
bool
parse_all(
    char const*& it,
    char const* const end,
    error_code& ec,
    Rn&&... rn)
{
    auto const it0 = it;
    if(! (parse)(it, end, ec,
        std::forward<Rn>(rn)...))
    {
        it = it0;
        return false;
    }
    return true;
}

//------------------------------------------------

template<class... Rn>
bool
parse_string(
    string_view s,
    error_code& ec,
    Rn&&... rn)
{
    auto it = s.data();
    auto const end = it + s.size();
    if(! (parse)(it, end, ec,
        std::forward<Rn>(rn)...))
        return false;
    if(it != end)
    {
        // input not consumed fully
        ec = error::leftover;
        return false;
    }
    return true;
}

//------------------------------------------------

template<class... Rn>
void
parse_string(
    string_view s,
    Rn&&... rn)
{
    error_code ec;
    if(! parse_string(s, ec,
        std::forward<Rn>(rn)...))
    {
        urls::detail::throw_system_error(
            ec, BOOST_CURRENT_LOCATION);
    }
    BOOST_ASSERT(! ec.failed());
}

} // grammar
} // urls
} // boost

#endif
