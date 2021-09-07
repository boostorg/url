//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_DETAIL_CHAR_SET_HPP
#define BOOST_URL_BNF_DETAIL_CHAR_SET_HPP

#include <boost/type_traits/make_void.hpp>
#include <type_traits>

namespace boost {
namespace urls {
namespace bnf {
namespace detail {

template<class T, class = void>
struct is_char_set_pred : std::false_type {};

template<class T>
struct is_char_set_pred<T, boost::void_t<
    decltype(
    std::declval<bool&>() =
        std::declval<T const&>().operator()(
            std::declval<char>())
            ) > > : std::true_type
{
};

template<class T, class = void>
struct has_find_if : std::false_type {};

template<class T>
struct has_find_if<T, boost::void_t<
    decltype(
    std::declval<char const*&>() =
        std::declval<T const&>().find_if(
            std::declval<char const*>(),
            std::declval<char const*>())
            )>> : std::true_type
{
};

template<class T, class = void>
struct has_find_if_not : std::false_type {};

template<class T>
struct has_find_if_not<T, boost::void_t<
    decltype(
    std::declval<char const*&>() =
        std::declval<T const&>().find_if_not(
            std::declval<char const*>(),
            std::declval<char const*>())
            )>> : std::true_type
{
};

template<class Pred>
char const*
find_if(
    char const* first,
    char const* const last,
    Pred const& pred,
    std::false_type) noexcept
{
    while(first != last)
    {
        if(pred(*first))
            break;
        ++first;
    }
    return first;
}

template<class Pred>
char const*
find_if(
    char const* first,
    char const* const last,
    Pred const& pred,
    std::true_type) noexcept
{
    return pred.find_if(
        first, last);
}

template<class Pred>
char const*
find_if_not(
    char const* first,
    char const* const last,
    Pred const& pred,
    std::false_type) noexcept
{
    while(first != last)
    {
        if(! pred(*first))
            break;
        ++first;
    }
    return first;
}

template<class Pred>
char const*
find_if_not(
    char const* first,
    char const* const last,
    Pred const& pred,
    std::true_type) noexcept
{
    return pred.find_if_not(
        first, last);
}

} // detail
} // bnf
} // urls
} // boost

#endif
