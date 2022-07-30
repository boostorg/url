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

/** A forward range of parsed elements

    @tparam T The value type of the range
*/
template<class T>
class range
{
    // buffer size for type-erased rule
    static
    constexpr
    std::size_t
    BufferSize = 64;

    struct any_rule;

    string_view s_;
    std::size_t n_ = 0;
    char buf_[BufferSize];
    any_rule const* pr_ = nullptr;

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
    template<class R>
    range(
        string_view s,
        std::size_t n,
        R const& increment);

    template<
        class R0, class R1>
    range(
        string_view s,
        std::size_t n,
        R0 const& begin,
        R1 const& increment);

    template<class R_>
    friend
    result<range<typename
        R_::value_type>>
    parse_range(
        char const*& it,
        char const* end,
        R_ const& increment,
        std::size_t N,
        std::size_t M);

    template<
        class R0_, class R1_>
    friend
    result<range<typename
        R0_::value_type>>
    parse_range(
        char const*& it,
        char const* end,
        R0_ const& begin,
        R1_ const& increment,
        std::size_t N,
        std::size_t M);
};

/** Parse a range
*/
template<
    class Rule>
result<range<typename
    Rule::value_type>>
parse_range(
    char const*& it,
    char const* end,
    Rule const& increment,
    std::size_t N = 0,
    std::size_t M = std::size_t(-1));

/** Parse a range
*/
template<
    class Rule0, class Rule1>
result<range<typename
    Rule0::value_type>>
parse_range(
    char const*& it,
    char const* end,
    Rule0 const& begin,
    Rule1 const& increment,
    std::size_t N = 0,
    std::size_t M = std::size_t(-1));

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/range.hpp>

#endif
