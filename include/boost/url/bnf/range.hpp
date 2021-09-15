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
#include <boost/url/string.hpp>
#include <boost/type_traits/make_void.hpp>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

struct pct_encoded_str;

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

template<class T>
class range
{
    using func_ptr = bool(*)(
        char const*&, char const*,
            error_code&, T&);

    string_view s_;
    std::size_t n_ = 0;
    func_ptr begin_ = nullptr;
    func_ptr increment_ = nullptr;

    range(
        string_view s,
        std::size_t n,
        func_ptr begin,
        func_ptr increment)
        : s_(s)
        , n_(n)
        , begin_(begin)
        , increment_(increment)
    {
    }

public:
    using value_type = T;

    class iterator;

    range(range const&) = default;
    range& operator=(
        range const&) = default;

    /** Default constructor

        Iteration of default constructed ranges
        is undefined.
    */
    range() = default;

    /** Return true if the range is empty
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

    /** Return the entire string underlying the range
    */
    string_view
    str() const noexcept
    {
        return s_;
    }

    iterator
    begin() const;

    iterator
    end() const;

    template<
        class T_, class U_>
    friend
    bool
    parse_range(
        char const*& it,
        char const* end,
        error_code& ec,
        range<T_>& t,
        U_ const&);
};

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/range.hpp>

#endif
