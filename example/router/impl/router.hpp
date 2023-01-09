//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#include <boost/url/detail/except.hpp>
#include <boost/url/decode_view.hpp>
#include <boost/url/grammar/unsigned_rule.hpp>
#include <boost/mp11/algorithm.hpp>

namespace boost {
namespace urls {

template <class T>
template <class U>
void
router<T>::
insert(string_view pattern, U&& v)
{
    BOOST_STATIC_ASSERT(
        std::is_same<T, U>::value ||
        std::is_convertible<U, T>::value);
    data_.emplace_back(std::forward<U>(v));
    insert_impl( pattern, data_.size() - 1 );
}

template <class T>
template <std::size_t N>
T const*
router<T>::
find(segments_encoded_view path, matches_storage<N>& m) const noexcept
{
    string_view matches[N];
    string_view ids[N];
    string_view* matches_it = matches;
    string_view* ids_it = ids;
    std::size_t idx = find_impl(
        path, matches_it, ids_it );
    if (idx != std::size_t(-1))
    {
        BOOST_ASSERT(matches_it >= matches);
        m = { matches, ids,
              static_cast<std::size_t>(
                  matches_it - matches) };
        return &data_[idx];
    }
    m = {};
    return nullptr;
}

} // urls
} // boost
