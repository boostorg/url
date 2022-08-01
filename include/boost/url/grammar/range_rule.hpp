//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_RANGE_RULE_HPP
#define BOOST_URL_GRAMMAR_RANGE_RULE_HPP

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

    Objects of this type are forward ranges
    returned when parsing using the
    @ref range_rule.
    Iteration is performed by re-parsing the
    underlying character buffer. Ownership
    of the buffer is not transferred; the
    caller is responsible for ensuring that
    the lifetime of the buffer extends until
    it is no longer referenced by the range.

    @tparam T The value type of the range

    @see
        @ref parse,
        @ref range_rule.
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
    /** The type of each element of the range
    */
    using value_type = T;

    /** The type of each element of the range
    */
    using reference = T const&;

    /** The type of each element of the range
    */
    using const_reference = T const&;

    /** Provided for compatibility, unused
    */
    using pointer = void const*;

    /** The type used to represent unsigned integers
    */
    using size_type = std::size_t;

    /** The type used to represent signed integers
    */
    using difference_type = std::ptrdiff_t;

    /** A constant, forward iterator to elements of the range
    */
    class iterator;

    /** A constant, forward iterator to elements of the range
    */
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

        The copy will reference the original
        underlying buffer; ownership is not
        not transferred. The caller is responsible
        for ensuring that the lifetime of the
        buffer extends until no longer in use.
    */
    range(range const&) noexcept;

    /** Return an iterator to the beginning
    */
    iterator begin() const noexcept;

    /** Return an iterator to the end
    */
    iterator end() const noexcept;

    /** Return the matching part of the string
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
        R const& r);

    template<
        class R0, class R1>
    range(
        string_view s,
        std::size_t n,
        R0 const& first,
        R1 const& next);

    template<
        class R0, class R1>
    friend struct range_rule_t;
};

//------------------------------------------------

#ifndef BOOST_URL_DOCS
template<
    class R0,
    class R1 = void>
struct range_rule_t;
#endif

//------------------------------------------------

/** Match a repeating number of elements

    @par Value Type
    @code
    using value_type = range< typename Rule::value_type >;
    @endcode

    @par BNF
    @code
    range        = <N>*<M>rule
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#section-3.6"
        >3.6.  Variable Repetition (rfc5234)</a>

    @see
        @ref parse,
        @ref range.
*/
#ifdef BOOST_URL_DOCS
template<class Rule>
constexpr
__implementation_defined__
range_rule(
    Rule next,
    std::size_t N = 0,
    std::size_t M =
        std::size_t(-1)) noexcept;
#else
template<class R>
struct range_rule_t<R>
{
    using value_type =
        range<typename R::value_type>;

    result<value_type>
    parse(
        char const*& it,
        char const* end) const;

private:
    constexpr
    range_rule_t(
        R const& next,
        std::size_t N,
        std::size_t M) noexcept
        : next_(next)
        , N_(N)
        , M_(M)
    {
    }

    template<class R_>
    friend
    constexpr
    range_rule_t<R_>
    range_rule(
        R_ const& next,
        std::size_t N,
        std::size_t M) noexcept;

    R const next_;
    std::size_t N_;
    std::size_t M_;
};

template<class Rule>
constexpr
range_rule_t<Rule>
range_rule(
    Rule const& next,
    std::size_t N = 0,
    std::size_t M =
        std::size_t(-1)) noexcept
{
    // If you get a compile error here it
    // means that your rule does not meet
    // the type requirements. Please check
    // the documentation.
    static_assert(
        is_rule<Rule>::value,
        "Rule requirements not met");

    return range_rule_t<Rule>{
        next, N, M};
}
#endif

//------------------------------------------------

/** Match a repeating number of elements

    @par Value Type
    @code
    using value_type = range< typename Rule::value_type >;
    @endcode

    @par BNF
    @code
    range       = <1>*<1>rule1
                / rule1 <N-1>*<M-1>rule2
    @endcode

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#section-3.6"
        >3.6.  Variable Repetition (rfc5234)</a>

    @see
        @ref parse,
        @ref range.
*/
#ifdef BOOST_URL_DOCS
template<
    class Rule1, class Rule2>
constexpr
__implementation_defined__
range_rule(
    Rule1 first,
    Rule2 next,
    std::size_t N = 0,
    std::size_t M =
        std::size_t(-1)) noexcept;
#else
template<class R0, class R1>
struct range_rule_t
{
    using value_type =
        range<typename R0::value_type>;

    result<value_type>
    parse(
        char const*& it,
        char const* end) const;

private:
    constexpr
    range_rule_t(
        R0 const& first,
        R1 const& next,
        std::size_t N,
        std::size_t M) noexcept
        : first_(first)
        , next_(next)
        , N_(N)
        , M_(M)
    {
    }

    template<
        class R0_, class R1_>
    friend
    constexpr
    range_rule_t<R0_, R1_>
    range_rule(
        R0_ const& first,
        R1_ const& next,
        std::size_t N,
        std::size_t M) noexcept;

    R0 const first_;
    R1 const next_;
    std::size_t N_;
    std::size_t M_;
};

template<
    class Rule1, class Rule2>
constexpr
range_rule_t<Rule1, Rule2>
range_rule(
    Rule1 const& first,
    Rule2 const& next,
    std::size_t N = 0,
    std::size_t M =
        std::size_t(-1)) noexcept
{
    // If you get a compile error here it
    // means that your rule does not meet
    // the type requirements. Please check
    // the documentation.
    static_assert(
        is_rule<Rule1>::value,
        "Rule requirements not met");
    static_assert(
        is_rule<Rule2>::value,
        "Rule requirements not met");

    // If you get a compile error here it
    // means that your rules do not have
    // the exact same value_type. Please
    // check the documentation.
    static_assert(
        std::is_same<
            typename Rule1::value_type,
            typename Rule2::value_type>::value,
        "Rule requirements not met");

    return range_rule_t<Rule1, Rule2>{
        first, next, N, M};
}
#endif

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/range_rule.hpp>

#endif
