//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_RANGE_HPP
#define BOOST_URL_BNF_RANGE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string_view.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/make_void.hpp>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {
namespace bnf {

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
    bool(*fp_)(
        char const*&,
        char const*,
        error_code&,
        range_&);

    template<class T>
    static
    bool
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
    bool
    parse(
        char const*& it,
        char const* end,
        error_code& ec,
        range_& t);
};

//------------------------------------------------

template<
    class ValueType,
    class ReferenceType = ValueType>
class range
{
public:
    /** The type of begin and increment function pointers
    */
    typedef bool (*fp_t)(
        char const*&,
        char const*,
        error_code&,
        ValueType&);

#ifdef BOOST_URL_DOCS
    /** A read-only forward iterator
    */
    using iterator = __see_below__;
#else
    class iterator;
#endif

    /** The type of value returned when dereferencing an iterator.
    */
    using value_type = ValueType;

    /** The type of value returned when dereferencing an iterator.
    */
    using reference = ReferenceType;

    /** The type of value returned when dereferencing an iterator.
    */
    using const_reference = ReferenceType;

    /** An unsigned integer type used to represent size.
    */
    using size_type = std::size_t;

    /** A signed integer type used to represent differences.
    */
    using difference_type = std::ptrdiff_t;

    /** Copy constructor.
    */
    range(range const&) = default;

    /** Copy assignment.
    */
    range& operator=(range const&) = default;

    /** Constructor

        Default constructed ranges are empty.
    */
    range() = default;

    /** Constructor
    */
    range(
        string_view s,
        std::size_t n,
        fp_t begin,
        fp_t increment) noexcept
        : s_(s)
        , n_(n)
        , begin_(begin)
        , increment_(increment)
    {
    }

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

private:
    string_view s_;
    std::size_t n_ = 0;
    fp_t begin_ = nullptr;
    fp_t increment_ = nullptr;
};

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/range.hpp>

#endif
