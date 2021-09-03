//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_SEQUENCE_HPP
#define BOOST_URL_BNF_IMPL_SEQUENCE_HPP

#include <boost/url/bnf/tuple.hpp>

namespace boost {
namespace urls {
namespace bnf {

template<class... Bn>
char const*
sequence<Bn...>::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    auto it = detail::parse(
        start, end, ec, &v_[0],
        std::integral_constant<
            std::size_t, 0>{}, t_);
    if(ec)
        return start;
    return it;
}

template<
    std::size_t I, class... Bn>
auto
get(sequence<Bn...>& e) ->
    tuple_element<
        I, sequence<Bn...>>&
{
    return get<I>(e.t_);
}

template<
    std::size_t I, class... Bn>
auto
get(sequence<Bn...> const& e) ->
    tuple_element<I,
        sequence<Bn...>> const&
{
    return get<I>(e.t_);
}

} // bnf
} // urls
} // boost

#endif
