//
// Copyright (c) 2021 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_CHARSET_HPP
#define BOOST_URL_GRAMMAR_CHARSET_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/detail/charset.hpp>
#include <boost/type_traits/make_void.hpp>
#include <boost/static_assert.hpp>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace boost {
namespace urls {
namespace grammar {

/** Alias for `std::true_type` if T satisfies <em>CharSet</em>.

    This metafunction determines if the
    type `T` meets these requirements of
    <em>CharSet</em>:

    @li An instance of `T` is invocable
    with this equivalent function signature:
    @code
    bool T::operator()( char ) const;
    @endcode

    @par Example
    Use with `enable_if` on the return value:
    @code
    template< class CharSet >
    typename std::enable_if< is_charset<T>::value >::type
    func( CharSet const& cs );
    @endcode

    @tparam T the type to check.
*/
#ifdef BOOST_URL_DOCS
template<class T>
using is_charset = __see_below__;
#else
template<class T, class = void>
struct is_charset : std::false_type {};

template<class T>
struct is_charset<T, boost::void_t<
    decltype(
    std::declval<bool&>() =
        std::declval<T const&>().operator()(
            std::declval<char>())
            ) > > : std::true_type
{
};
#endif

//------------------------------------------------

/** Find the first character in the string that is in the set.

    @par Exception Safety
    Throws nothing.

    @return A pointer to the found character,
    otherwise the value `last`.

    @param first A pointer to the first character
    in the string to search.

    @param last A pointer to one past the last
    character in the string to search.

    @param cs The character set to use.

    @see
        @ref find_if_not.
*/
template<class CharSet>
char const*
find_if(
    char const* const first,
    char const* const last,
    CharSet const& cs) noexcept
{
    // If you get a compile error here
    // it means your type does not meet
    // the requirements. Please check the
    // documentation.
    static_assert(
        is_charset<CharSet>::value,
        "CharSet requirements not met");

    return detail::find_if(first, last, cs,
        detail::has_find_if<CharSet>{});
}

/** Find the first character in the string that is not in CharSet

    @par Exception Safety
    Throws nothing.

    @return A pointer to the found character,
    otherwise the value `last`.

    @param first A pointer to the first character
    in the string to search.

    @param last A pointer to one past the last
    character in the string to search.

    @param cs The character set to use.

    @see
        @ref find_if_not.
*/
template<class CharSet>
char const*
find_if_not(
    char const* const first,
    char const* const last,
    CharSet const& cs) noexcept
{
    // If you get a compile error here
    // it means your type does not meet
    // the requirements. Please check the
    // documentation.
    static_assert(
        is_charset<CharSet>::value,
        "CharSet requirements not met");

    return detail::find_if_not(first, last, cs,
        detail::has_find_if_not<CharSet>{});
}

} // grammar
} // urls
} // boost

#endif
