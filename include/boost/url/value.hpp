//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_VALUE_HPP
#define BOOST_URL_VALUE_HPP

#include <boost/url/config.hpp>
#include <boost/url/basic_value.hpp>
#include <boost/url/detail/storage.hpp>
#include <utility>

namespace boost {
namespace url {

namespace detail {

template<class Allocator>
struct storage_member
{
    alloc_storage<Allocator> st_;

    explicit
    storage_member(
        Allocator const& alloc)
        : st_(alloc)
    {
    }
};

} // detail

template<class Allocator>
class dynamic_value
    : private detail::storage_member<Allocator>
    , public basic_value
{
public:
    dynamic_value() noexcept
        : detail::storage_member<
            Allocator>(Allocator{})
        , basic_value(static_cast<
            detail::storage&>(this->st_))
    {
    }

    explicit
    dynamic_value(
        string_view s,
        Allocator const& a = {})
        : detail::storage_member<
            Allocator>(a)
        , basic_value(this->st_, s)
    {
    }

    explicit
    dynamic_value(
        Allocator const& a) noexcept
        : detail::storage_member<
            Allocator>(a)
        , basic_value(this->st_)
    {
    }

    dynamic_value(
        dynamic_value&&) noexcept
    {
    }

    dynamic_value(
        dynamic_value const&)
    {
    }

    dynamic_value&
    operator=(
        dynamic_value const&)
    {
        return *this;
    }
};

using value = dynamic_value<
    std::allocator<char>>;

} // url
} // boost

#endif
