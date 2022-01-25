//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//                         Damian Jarek (damian dot jarek93 at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/http_proto
//

#ifndef BOOST_URL_GRAMMAR_ELEMENTS_HPP
#define BOOST_URL_GRAMMAR_ELEMENTS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/static_assert.hpp>

#include <boost/mp11/integer_sequence.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/copy_cv.hpp>
#include <cstdlib>
#include <utility>

namespace boost {
namespace urls {
namespace grammar {

namespace detail {

// lightweight tuple by Damian Jarek.
// this one is default-constructible,

template<
    std::size_t I, class R>
struct element_impl
{
    R t;
};

template<class... Rn>
struct elements_impl;

template<
    class... Rn,
    std::size_t... In>
struct elements_impl<
    boost::mp11::index_sequence<
        In...>, Rn...>
  : element_impl<In, Rn>...
{
};

template<class... Rn>
struct tuple : elements_impl<
    boost::mp11::index_sequence_for<
        Rn...>, Rn...>
{
};

} // detail

//------------------------------------------------

template<
    std::size_t I, class R>
R&
get(detail::element_impl<
    I, R>& te)
{
    return te.t;
}

template<
    std::size_t I, class R>
R const&
get(detail::element_impl<
    I, R> const& te)
{
    return te.t;
}

template<
    std::size_t I, class R>
R&&
get(detail::element_impl<
    I, R>&& te)
{
    return std::move(te.t);
}

template<
    std::size_t I, class R>
R&
get(detail::element_impl<
    I, R&>&& te)
{
    return te.t;
}

//------------------------------------------------

/** Rule for a series of elements parsed in sequence.
*/
template<class... Rn>
struct elements
#ifndef BOOST_URL_DOCS
    : detail::elements_impl<
        boost::mp11::index_sequence_for<
            Rn...>, Rn...>
#endif
{
    BOOST_STATIC_ASSERT(
        sizeof...(Rn) > 0);
    friend
    void
    tag_invoke(
        parse_tag const&,
        char const*& it,
        char const* end,
        error_code& ec,
        elements& t) noexcept
    {
        constexpr auto I = sizeof...(Rn);
        parse_impl(
            it, end, ec, t,
            std::integral_constant<
                std::size_t, I-1>{});
    }

private:
    static
    void
    parse_impl(
        char const*& it,
        char const* end,
        error_code& ec,
        elements& t,
        std::integral_constant<
            std::size_t, 0>)
    {
        parse(it, end, ec, get<0>(t));
    }

    template<std::size_t I>
    static
    void
    parse_impl(
        char const*& it,
        char const* end,
        error_code& ec,
        elements& t,
        std::integral_constant<
            std::size_t, I>)
    {
        parse_impl(
            it, end, ec, t,
            std::integral_constant<
                std::size_t, I-1>{});
        if(ec.failed())
            return;
        parse(it, end, ec, get<I>(t));
    }
};

} // grammar
} // urls
} // boost

#endif
