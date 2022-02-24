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
#include <boost/type_traits/make_void.hpp>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {
namespace grammar {

/** Alias for `std::true_type` if T satisfies __Range__
*/
#ifdef BOOST_URL_DOCS
template<class T>
using is_range = __see_below__;
#else
template<class T, class = void>
struct is_range : std::false_type {};

template<class T>
struct is_range<T, boost::void_t<
    decltype(typename T::value_type()), // default ctor
    decltype(
    std::declval<bool&>() =
        T::begin(
            std::declval<char const*&>(),
            std::declval<char const*>(),
            std::declval<error_code&>(),
            std::declval<typename
                std::add_lvalue_reference<
                    typename T::value_type>::type>()),
    std::declval<bool&>() =
        T::increment(
            std::declval<char const*&>(),
            std::declval<char const*>(),
            std::declval<error_code&>(),
            std::declval<typename
                std::add_lvalue_reference<
                    typename T::value_type>::type>())
        ) > >
    : std::true_type
{
};
#endif

//------------------------------------------------

class range_
{
    void(*fp_)(
        char const*&,
        char const*,
        error_code&,
        range_&);

    template<class T>
    static
    void
    parse_impl(
        char const*& it,
        char const* end,
        error_code& ec,
        range_& t);

protected:
    template<class T>
    explicit
    range_(T const*) noexcept;

public:
    string_view str;
    std::size_t count;

    template<class T>
    class iterator;

    range_() noexcept
        : fp_(nullptr)
    {
    }

    friend
    void
    tag_invoke(
        parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        range_& t)
    {
        t.fp_(it, end, ec, t);
    }
};

//------------------------------------------------

/** Rule for a repeating number of rules.
*/
template<class R>
class range_base
{
    string_view s_;
    std::size_t n_ = 0;

protected:
    /** Parse a range
    */
    void
    parse(
        char const*& it,
        char const* end,
        error_code& ec,
        std::size_t N,
        std::size_t M);

public:
#ifdef BOOST_URL_DOCS
    /** A read-only forward iterator
    */
    using iterator = __see_below__;
#else
    class iterator;
#endif

    /** The type of value returned when dereferencing an iterator.
    */
    using value_type = typename R::value_type;

    /** The type of value returned when dereferencing an iterator.
    */
    using reference = typename R::reference;

    /** The type of value returned when dereferencing an iterator.
    */
    using const_reference = reference;

    /** An unsigned integer type used to represent size.
    */
    using size_type = std::size_t;

    /** A signed integer type used to represent differences.
    */
    using difference_type = std::ptrdiff_t;

    /** Copy constructor.
    */
    range_base(range_base const&) = default;

    /** Copy assignment.
    */
    range_base& operator=(range_base const&) = default;

    /** Constructor

        Default constructed ranges are empty.
    */
    range_base() = default;

    /** Return the underlying string.
    */
    string_view
    str() const noexcept
    {
        return s_;
    }

    /** Return an iterator to the beginning.
    */
    iterator
    begin() const;

    /** Return an iterator to the end.
    */
    iterator
    end() const;

    /** Return true if the range contains no elements
    */
    bool
    empty() const noexcept
    {
        return n_ == 0;
    }

    /** Return the number of elements in the range
    */
    std::size_t
    size() const noexcept
    {
        return n_;
    }
};

//------------------------------------------------

/** Rule for a repeating number of rules.

    This rule defines a sequence containing
    at least N and at most M of R.

    @par BNF
    @code
    range           =  <n>*<m>element

    *<m>element     => <0>*<m>element
    <n>*element     => <n>*<inf.>element
    *element        => <0>*<inf.>element
    <n>element      => <n>*<n>element
    [ element ]     => *1( element )
    @endcode

    @tparam R The rule to repeat
    @tparam N The minimum number of repetitions, which may be zero
    @tparam M The maximum number of repetitions.

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc5234#section-3.6"
        >3.6. Variable Repetition: *Rule</a>
*/
template<
    class R,
    std::size_t N = 0,
    std::size_t M = std::size_t(-1)>
class range : public range_base<R>
{
    BOOST_STATIC_ASSERT(
        M >= N ||
        (N == M && N > 0));

public:
    /** Parse a range
    */
    friend
    void
    tag_invoke(
        parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        range& t)
    {
        t.parse(it, end, ec, N, M);
    }
};

} // grammar
} // urls
} // boost

#include <boost/url/grammar/impl/range.hpp>

#endif
