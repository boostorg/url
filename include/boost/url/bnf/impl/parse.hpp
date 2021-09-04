//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_PARSE_HPP
#define BOOST_URL_BNF_IMPL_PARSE_HPP

#include <boost/url/detail/except.hpp>
#include <utility>

namespace boost {
namespace urls {
namespace bnf {

char const*
parse(
    char const* const start,
    char const* const end,
    error_code& ec,
    char ch) noexcept
{
    if(start == end)
    {
        ec = error::need_more;
        return start;
    }
    auto it = start;
    if(*it != ch)
    {
        // expected ch
        ec = error::syntax;
        return start;
    }
    ++it;
    return it;
}
 
template<class T>
char const*
parse(
    char const* start,
    char const* end,
    error_code& ec,
    optional<T>& t)
{
    t.emplace();
    auto it = parse(
        start, end, ec, *t);
    if(! ec.failed())
        return it;
    ec = {};
    t.reset();
    return start;
}

char const*
parse(
    char const* start,
    char const* end,
    error_code& ec,
    optional<char> const& t) = delete;

template<
    class T0,
    class T1,
    class... Tn>
char const*
parse(
    char const* start,
    char const* end,
    error_code& ec,
    T0&& t0,
    T1&& t1,
    Tn&&... tn)
{
    auto it = parse(
        start, end, ec,
        std::forward<T0>(t0));
    if(ec)
        return start;
    it = parse(
        it, end, ec,
        std::forward<T1>(t1),
        std::forward<Tn>(tn)...);
    if(ec)
        return start;
    return it;
}

template<
    class T0,
    class... Tn>
void
parse(
    string_view s,
    error_code& ec,
    T0&& t0,
    Tn&&... tn)
{
    auto const end =
        s.data() + s.size();
    auto it = parse(
        s.data(), end, ec,
        std::forward<T0>(t0),
        std::forward<Tn>(tn)...);
    if(ec)
        return;
    if(it != end)
    {
        // partial match
        ec = error::syntax;
        return;
    }
}

template<class T>
bool
is_valid(
    string_view s)
{
    T t;
    error_code ec;
    auto const end =
        s.data() + s.size();
    auto it = parse(
        s.data(), end, ec, t);
    if(ec)
        return false;
    if(it != end)
    {
        // partial match
        return false;
    }
    return true;
}

bool
is_valid(char) = delete;

template<class T>
void
validate(string_view s)
{
    using namespace urls::detail;
    if(! is_valid<T>(s))
        throw_invalid_argument(
            "syntax error",
            BOOST_CURRENT_LOCATION);
}

} // bnf
} // urls
} // boost

#endif
