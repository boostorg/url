//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_STRING_HPP
#define BOOST_URL_STRING_HPP

#include <boost/url/detail/config.hpp>
#include <boost/type_traits/make_void.hpp>
#include <boost/utility/string_view.hpp>
#include <string>
#include <type_traits>

#ifndef BOOST_NO_CXX17_HDR_STRING_VIEW
#include <string_view>
#define BOOST_URL_HAS_STRING_VIEW
#endif

namespace boost {
namespace urls {

/** The type of string_view used by the library
*/
using string_view = boost::string_view;

/// The string alias template return type for allocating member functions.
template<class Allocator>
using string_type =
    std::basic_string<char,
        std::char_traits<char>, Allocator>;

/** Alias for true_type if T is like a string_view
*/
#ifdef BOOST_URL_DOCS
template<class T>
using is_stringish = __see_below__;
#else
template<class T, class = void>
struct is_stringish :
    std::is_convertible<T, string_view>
{
};

template<class T>
struct is_stringish<T, boost::void_t<
    decltype(
    std::declval<char const*&>() =
        std::declval<T const>().data(),
    std::declval<std::size_t&>() =
        std::declval<T const>().size()
            ) > > : std::true_type
{
};
#endif

#ifdef BOOST_JSON_DOCS
template<class T>
string_view
to_string_view(T const& t) noexcept;
#else
template<class T>
typename std::enable_if<
    std::is_convertible<
        T, string_view>::value,
    string_view>::type
to_string_view(
    T const& t) noexcept
{
    return t;
}

template<class T>
typename std::enable_if<
    is_stringish<T>::value &&
    ! std::is_convertible<
        T, string_view>::value,
    string_view>::type
to_string_view(
    T const& t) noexcept
{
    return { t.data(), t.size() };
}
#endif

} // urls
} // boost

#endif
