//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_DECODED_RANGE_IPP
#define BOOST_URL_IMPL_PCT_DECODED_RANGE_IPP

#include <boost/url/pct_decoded_range.hpp>
#include <memory>
#include <new>

namespace boost {
namespace urls {

pct_decoded_range::const_iterator
pct_decoded_range::begin() const noexcept
{
    return {p_, n_, plus_to_space_};
}

pct_decoded_range::const_iterator
pct_decoded_range::cbegin() const noexcept
{
    return begin();
}

pct_decoded_range::const_iterator
pct_decoded_range::end() const noexcept
{
    return {p_, n_, plus_to_space_, 0};
}

pct_decoded_range::const_iterator
pct_decoded_range::cend() const noexcept
{
    return {p_, n_, plus_to_space_, 0};
}

pct_decoded_range::const_reference
pct_decoded_range::front() const
{
    BOOST_ASSERT( !empty() );
    return *begin();
}

pct_decoded_range::const_reference
pct_decoded_range::back() const
{
    BOOST_ASSERT( !empty() );
    auto last = end();
    --last;
    return *last;
}

pct_decoded_range::size_type
pct_decoded_range::copy(
    char* dest,
    size_type count,
    size_type pos) const
{
    if( pos > size() )
        detail::throw_invalid_argument(
            BOOST_CURRENT_LOCATION);
    std::size_t rlen = (std::min)(count, size() - pos);
    auto first = begin();
    while (pos--)
        ++first;
    auto last = first;
    while (rlen--)
        ++last;
    for (; first != last; ++first, ++dest)
        *dest = *first;
    return rlen;
}

int
pct_decoded_range::compare(string_view other) const
{
    std::size_t rlen = (std::min)(size(), other.size());

    // int cmp = traits_type::compare(encoded_data(), other.data(), rlen);
    auto first1 = begin();
    auto last1 = end();
    auto first2 = other.begin();
    auto last2 = other.end();
    int cmp = 0;
    for ( ;
         (first1 != last1) && (first2 != last2);
         ++first1, ++first2, --rlen)
    {
        if (*first1 < *first2)
        {
            cmp = -1;
            break;
        }
        else if (*first2 < *first1)
        {
            cmp = +1;
            break;
        }
    }
    if( cmp != 0 )
        return cmp;
    if( size() == other.size() )
        return 0;
    return size() < other.size()? -1: +1;
}

int
pct_decoded_range::compare(pct_decoded_range other) const
{
    std::size_t rlen = (std::min)(size(), other.size());

    // int cmp = traits_type::compare(encoded_data(), other.data(), rlen);
    auto first1 = begin();
    auto last1 = end();
    auto first2 = other.begin();
    auto last2 = other.end();
    int cmp = 0;
    for ( ;
         (first1 != last1) && (first2 != last2);
         ++first1, ++first2, --rlen)
    {
        if (*first1 < *first2)
        {
            cmp = -1;
            break;
        }
        else if (*first2 < *first1)
        {
            cmp = +1;
            break;
        }
    }
    if( cmp != 0 )
        return cmp;
    if( size() == other.size() )
        return 0;
    return size() < other.size()? -1: +1;
}

std::ostream&
operator<<(std::ostream& os,
           pct_decoded_range const& str) {
    auto it = str.begin();
    auto end = str.end();
    for (; it != end; ++it)
        os << *it;
    return os;
}

} // urls
} // boost

#endif
