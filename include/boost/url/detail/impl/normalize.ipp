//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_NORMALIZE_IPP
#define BOOST_URL_DETAIL_IMPL_NORMALIZE_IPP

#include <boost/url/detail/normalize.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/assert.hpp>
#include <cstring>

namespace boost {
namespace urls {
namespace detail {

int
compare_encoded(
    string_view lhs,
    string_view rhs) noexcept
{
    auto consume_one = [](
        string_view::iterator& it,
        char& c,
        std::size_t& n)
    {
        if(*it != '%')
        {
            c = *it;
            ++it;
        }
        else
        {
            pct_decode_unchecked(
                &c,
                &c + 1,
                string_view(it, 3));
            it += 3;
        }
        ++n;
    };

    std::size_t n0 = 0;
    std::size_t n1 = 0;
    auto it0 = lhs.begin();
    auto it1 = rhs.begin();
    auto end0 = lhs.end();
    auto end1 = rhs.end();
    char c0 = 0;
    char c1 = 0;
    while(
        it0 < end0 &&
        it1 < end1)
    {
        consume_one(it0, c0, n0);
        consume_one(it1, c1, n1);
        if (c0 < c1)
            return -1;
        if (c1 < c0)
            return 1;
    }
    n0 += pct_decode_bytes_unchecked(
        string_view(it0, end0 - it0));
    n1 += pct_decode_bytes_unchecked(
        string_view(it1, end1 - it1));
    if (n0 == n1)
        return 0;
    if (n0 < n1)
        return -1;
    return 1;
}

std::size_t
starts_with_encoded(
    string_view lhs,
    string_view rhs) noexcept
{
    auto consume_one = [](
        string_view::iterator& it,
        char &c)
    {
        if(*it != '%')
        {
            c = *it;
            ++it;
            return;
        }
        pct_decode_unchecked(
            &c,
            &c + 1,
            string_view(it, 3));
        it += 3;
    };

    auto it0 = lhs.begin();
    auto it1 = rhs.begin();
    auto end0 = lhs.end();
    auto end1 = rhs.end();
    char c0 = 0;
    char c1 = 0;
    while (it0 < end0 &&
           it1 < end1)
    {
        consume_one(it0, c0);
        consume_one(it1, c1);
        if (c0 != c1)
            return 0;
    }
    if (it1 == end1)
        return it0 - lhs.begin();
    return 0;
}

std::size_t
ends_with_encoded(
    string_view lhs,
    string_view rhs) noexcept
{
    auto consume_last = [](
        string_view::iterator& it,
        string_view::iterator& end,
        char& c)
    {
        if((end - it) < 3 ||
           *(std::prev(end, 3)) != '%')
        {
            c = *std::prev(end);
            --end;
            return;
        }
        pct_decode_unchecked(
            &c,
            &c + 1,
            string_view(std::prev(
                end, 3), 3));
        end -= 3;
    };

    auto it0 = lhs.begin();
    auto it1 = rhs.begin();
    auto end0 = lhs.end();
    auto end1 = rhs.end();
    char c0 = 0;
    char c1 = 0;
    while(
        it0 < end0 &&
        it1 < end1)
    {
        consume_last(it0, end0, c0);
        consume_last(it1, end1, c1);
        if (c0 != c1)
            return 0;
    }
    if (it1 == end1)
        return lhs.end() - end0;
    return 0;
}

int
ci_compare_encoded(
    string_view lhs,
    string_view rhs) noexcept
{
    auto consume_one =
        []( string_view::iterator& it,
            char &c,
            std::size_t& n)
    {
        if(*it != '%')
        {
            c = grammar::ascii_tolower(*it);
            ++it;
        }
        else
        {
            pct_decode_unchecked(
                &c,
                &c + 1,
                string_view(it, 3));
            c = grammar::ascii_tolower(c);
            it += 3;
        }
        ++n;
    };

    std::size_t n0 = 0;
    std::size_t n1 = 0;
    auto it0 = lhs.begin();
    auto it1 = rhs.begin();
    auto end0 = lhs.end();
    auto end1 = rhs.end();
    char c0 = 0;
    char c1 = 0;
    while (
        it0 < end0 &&
        it1 < end1)
    {
        consume_one(it0, c0, n0);
        consume_one(it1, c1, n1);
        if (c0 < c1)
            return -1;
        if (c1 < c0)
            return 1;
    }
    n0 += pct_decode_bytes_unchecked(
        string_view(it0, end0 - it0));
    n1 += pct_decode_bytes_unchecked(
        string_view(it1, end1 - it1));
    if (n0 == n1)
        return 0;
    if (n0 < n1)
        return -1;
    return 1;
}

int
ci_compare(
    string_view lhs,
    string_view rhs) noexcept
{
    auto rlen = (std::min)(lhs.size(), rhs.size());
    for (std::size_t i = 0; i < rlen; ++i)
    {
        char c0 = grammar::ascii_tolower(lhs[i]);
        char c1 = grammar::ascii_tolower(rhs[i]);
        if (c0 < c1)
            return -1;
        if (c1 < c0)
            return 1;
    }
    if ( lhs.size() == rhs.size() )
        return 0;
    if ( lhs.size() < rhs.size() )
        return -1;
    return 1;
}

} // detail
} // urls
} // boost

#endif
