//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_ALGORITHM_HPP
#define BOOST_URL_BNF_ALGORITHM_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/bnf/type_traits.hpp>
#include <type_traits>

namespace boost {
namespace urls {
namespace bnf {

/** Consume a matching element BNF.
*/
template<class Element>
#ifdef BOOST_URL_DOCS
char const*
#else
typename std::enable_if<
    is_element<Element>::value,
    char const*>::type
#endif
consume(
    char const* start,
    char const* end,
    error_code& ec)
{
    Element e;
    auto it = e.parse(
        start, end, ec);
    return it;
}

//------------------------------------------------

/** Consume a matching list BNF
*/
template<class List>
#ifdef BOOST_URL_DOCS
char const*
#else
typename std::enable_if<
    is_list<List>::value,
    char const*>::type
#endif
consume(
    char const* start,
    char const* end,
    error_code& ec)
{
    List e;
    auto it = e.begin(
        start, end, ec);
    for(;;)
    {
        if(ec == error::end)
        {
            ec = {};
            break;
        }
        if(ec.failed())
        {
            // treat as non-match
            return start;
        }
        it = e.increment(
            it, end, ec);
    }
    return it;
}

//------------------------------------------------

/** Require the specified character
*/
inline
char const*
consume(
    char ch,
    char const* start,
    char const* end,
    error_code& ec)
{
    if(start == end)
    {
        ec = error::need_more;
        return start;
    }
    if(*start != ch)
    {
        ec = error::syntax;
        return start;
    }
    return start + 1;
}

//------------------------------------------------

/** Require CRLF
*/
inline
char const*
consume_crlf(
    char const* const start,
    char const* const end,
    error_code& ec) noexcept
{
    auto it = consume(
        '\r', start, end, ec);
    if(ec.failed())
        return it;
    it = consume(
        '\n', it, end, ec);
    if(ec.failed())
        return it;
    return it;
}

//------------------------------------------------

/** Return true if s matches the BNF exactly
*/
template<class BNF>
bool
is_valid(string_view s)
{
    BOOST_STATIC_ASSERT(
        is_bnf<BNF>::value);
    error_code ec;
    auto const end =
        s.data() + s.size();
    auto it = consume<BNF>(
        s.data(), end, ec);
    if(ec.failed())
        return false;
    return it == end;
}

/** Throw an exception if s does not match the BNF exactly
*/
template<class BNF>
void
validate(string_view s)
{
    BOOST_STATIC_ASSERT(
        is_bnf<BNF>::value);
    if(! is_valid<BNF>(s))
        urls::detail::throw_invalid_argument(
            "bad syntax", BOOST_CURRENT_LOCATION);
}

} // bnf
} // urls
} // boost

#endif
