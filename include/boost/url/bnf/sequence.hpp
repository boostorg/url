//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_SEQUENCE_HPP
#define BOOST_URL_BNF_SEQUENCE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/tuple.hpp>
#include <boost/assert.hpp>
#include <array>

namespace boost {
namespace urls {
namespace bnf {

/** A sequence of one or more elements
*/
template<
    class... Bn>
class sequence
{
    tuple<Bn...> t_;
    std::array<
        string_view,
        sizeof...(Bn)> v_;

public:
    using value_type = tuple<Bn...>;

    value_type const&
    operator*() const noexcept
    {
        return t_;
    }

    value_type const*
    operator->() const noexcept
    {
        return &t_;
    }

    /** Return the matching string for the Ith element
    */
    string_view const
    operator[](std::size_t i) const
    {
        BOOST_ASSERT(i <
            sizeof...(Bn));
        return v_[i];
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec);

    /** Return the Ith element of the sequence `e`
    */
    template<
        std::size_t I, class... Bn>
    friend
    auto
    get(sequence<Bn...>& e) ->
        tuple_element<
            I, sequence<Bn...>>&;

    /** Return the Ith element of the sequence `e`
    */
    template<
        std::size_t I, class... Bn>
    friend
    auto
    get(sequence<Bn...> const& e) ->
        tuple_element<
            I, sequence<Bn...>> const&;
};

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/sequence.hpp>

#endif
