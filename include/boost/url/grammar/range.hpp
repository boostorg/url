//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_RANGE_HPP
#define BOOST_URL_GRAMMAR_RANGE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/static_assert.hpp>
#include <boost/config/workaround.hpp>
#include <boost/type_traits/make_void.hpp>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {
namespace grammar {

/** The type of function used to construct parsed ranges
*/
template<
    class R, class T>
using range_fn = result<T>(R::*)(
    char const*&, char const*) const;

/** A forward range of parsed elements

    @tparam T The value type of the range
*/
template<class T>
class range
{
    struct any_rule;

    any_rule const&
    get() const noexcept;

public:
    using value_type = T;
    using reference = T const&;
    using const_reference = T const&;
    using pointer = void const*;
    using size_type = std::size_t;
    using difference_type =
        std::ptrdiff_t;

    class iterator;

    using const_iterator = iterator;

    /** Destructor
    */
    ~range();

#ifndef BOOST_URL_DOCS
#if BOOST_WORKAROUND( BOOST_GCC_VERSION, < 50000 ) || \
    BOOST_WORKAROUND( BOOST_CLANG_VERSION, < 40000 )
    range() noexcept = default;
#endif
#endif

    /** Constructor
    */
    range(range const&) noexcept;

    iterator begin() const noexcept;

    iterator end() const noexcept;

    /** Return the parsed string
    */
    string_view
    string() const noexcept
    {
        return s_;
    }

    /** Return the number of elements in the range
    */
    std::size_t
    size() const noexcept
    {
        return n_;
    }

    /** Return true if the range is empty
    */
    bool
    empty() const noexcept
    {
        return n_ == 0;
    }

private:
    // buffer size for type-erased rule
    static
    constexpr
    std::size_t
    BufferSize = 64;

    string_view s_;
    std::size_t n_ = 0;
    char buf_[BufferSize];

    template<class R>
    range(
        string_view s,
        std::size_t n,
        R const& r,
        range_fn<R, T> begin,
        range_fn<R, T> increment) noexcept;

    template<
        class R_,
        class T_>
    friend
    auto
    parse_range(
        char const*& it,
        char const* end,
        R_ const& r,
        range_fn<typename std::remove_cv<R_>::type, T_> begin,
        range_fn<typename std::remove_cv<R_>::type, T_> increment,
        std::size_t N,
        std::size_t M) ->
            result<range<T_>>;
};

/** Parse a range
*/
template<class R, class T>
auto
parse_range(
    char const*& it,
    char const* end,
    R const& r,
    range_fn<typename std::remove_cv<R>::type, T> begin,
    range_fn<typename std::remove_cv<R>::type, T> increment,
    std::size_t N = 0,
    std::size_t M = std::size_t(-1)) ->
        result<range<T>>;

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/range.hpp>

#endif
