//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_DETAIL_ANY_ITER_RULE_HPP
#define BOOST_URL_DETAIL_ANY_ITER_RULE_HPP

#include <boost/static_assert.hpp>
#include <atomic>
#include <cstdlib>
#include <type_traits>

namespace boost {
namespace urls {
namespace grammar {
namespace detail {

template<class T>
struct BOOST_SYMBOL_VISIBLE
    any_iter_rule
{
    virtual
    ~any_iter_rule() noexcept = default;

    explicit
    any_iter_rule(int) noexcept
        : refs_(2) // never delete
    {
    }

    virtual
    result<T>
    parse(
        char const*&,
        char const*,
        bool) const noexcept
    {
        return error_code{};
    }

    any_iter_rule const*
    acquire() const noexcept
    {
        ++refs_;
        return this;
    }

    void
    release() const noexcept
    {
        if(--refs_)
            return;
        delete this;
    }

protected:
    any_iter_rule() noexcept
        : refs_(1)
    {
    }

private:
    std::atomic<
        std::size_t> mutable refs_;
};

//------------------------------------------------

template<class R0, class R1 = void>
struct iter_rule0_v0
    : any_iter_rule<
        typename R0::value_type>
{
    R0 r0_;
    R1 r1_;

    iter_rule0_v0(
        R0 const& r0,
        R1 const& r1) noexcept
        : r0_(r0)
        , r1_(r1)
    {
    }

    iter_rule0_v0(
        R0 const& r0,
        R1 const& r1,
        int) noexcept
        : any_iter_rule<
            typename R0::value_type>(2)
        , r0_(r0)
        , r1_(r1)
    {
    }

    result<typename R0::value_type>
    parse(
        char const*& it,
        char const* end,
        bool first) const noexcept override
    {
        if(! first)
            return grammar::parse(it, end, r1_);
        return grammar::parse(it, end, r0_);
    }
};

//------------------------------------------------

template<class R>
struct iter_rule0_v0<R, void>
    : any_iter_rule<
        typename R::value_type>
{
    R r_;

    explicit
    iter_rule0_v0(
        R const& r) noexcept
        : r_(r)
    {
    }

    iter_rule0_v0(
        R const& r,
        int) noexcept
        : any_iter_rule<
            typename R::value_type>(2)
        , r_(r)
    {
    }

    result<typename R::value_type>
    parse(
        char const*& it,
        char const* end,
        bool) const noexcept override
    {
        return grammar::parse(it, end, r_);
    }
};

//------------------------------------------------

template<
    class R0,
    class R1
    , class = void
    , class = typename std::enable_if<
        ! std::is_empty<R0>::value ||
        ! std::is_empty<R1>::value>::type
>
constexpr
auto
make_any_iter_rule2(
    R0 const& r0,
    R1 const& r1) ->
        any_iter_rule<typename
            R0::value_type> const*
{
    BOOST_STATIC_ASSERT(std::is_same<
        typename R0::value_type,
        typename R1::value_type>::value);

    return new iter_rule0_v0<
        R0, R1>(r0, r1);
}

template<
    class R0,
    class R1
    , class = typename std::enable_if<
        std::is_empty<R0>::value &&
        std::is_empty<R1>::value>::type
>
constexpr
auto
make_any_iter_rule2(
    R0 const& r0,
    R1 const& r1) ->
        any_iter_rule<typename
            R0::value_type> const*
{
    BOOST_STATIC_ASSERT(std::is_same<
        typename R0::value_type,
        typename R1::value_type>::value);

    static
    iter_rule0_v0<
        R0, R1> const r(2);

    return &r;
}

//------------------------------------------------

template<
    class R
    , class = void
    , class = typename std::enable_if<
        ! std::is_empty<R>::value>::type
>
constexpr
auto
make_any_iter_rule1(
    R const& r) ->
        any_iter_rule<typename
            R::value_type> const*
{
    return new iter_rule0_v0<R>(r);
}

template<
    class R
    , class = typename std::enable_if<
        std::is_empty<R>::value>::type
>
constexpr
auto
make_any_iter_rule1(
    R const& r) ->
        any_iter_rule<typename
            R::value_type> const*
{
    static
    iter_rule0_v0<
        R> const r(2);

    return &r;
}

} // detail
} // grammar
} // urls
} // boost

#endif
