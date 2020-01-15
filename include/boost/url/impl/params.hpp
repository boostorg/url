//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_PARAMS_HPP
#define BOOST_URL_IMPL_PARAMS_HPP

#include <boost/url/view.hpp>

namespace boost {
namespace url {

params
view::
params() const noexcept
{
    return url::params(*this);
}

//----------------------------------------------------------

class params::iterator
{
    friend params;

    view const* v_ = nullptr;
    id_type i_ = 0;

    iterator(
        view const* v,
        id_type i)
        : v_(v)
        , i_(i)
    {
    }

public:
    using value_type = params::value_type;

    iterator() = default;

    value_type
    operator*() const noexcept
    {
        return value_type(v_, i_);
    }

    value_type
    operator->() const noexcept
    {
        return operator*();
    }

    bool
    operator==(
        iterator const& other) const noexcept
    {
        return
            v_ == other.v_ &&
            i_ == other.i_;
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return !(*this == other);
    }

    iterator&
    operator++() noexcept
    {
        BOOST_ASSERT(i_ < v_->nparam_);
        ++i_;
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    iterator&
    operator--() noexcept
    {
        BOOST_ASSERT(i_ > 0);
        --i_;
        return *this;
    }

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }
};

//----------------------------------------------------------

bool
params::
empty() const noexcept
{
    if(! v_)
        return true;
    return v_->nparam_ == 0;
}

std::size_t
params::
size() const noexcept
{
    if(! v_)
        return 0;
    return v_->nparam_;
}

auto
params::
begin() const noexcept ->
    iterator
{
    BOOST_ASSERT(v_);
    return iterator(v_, 0);
}

auto
params::
end() const noexcept ->
    iterator
{
    BOOST_ASSERT(v_);
    return iterator(
        v_, v_->nparam_);
}

auto
params::
operator[](std::size_t i) const ->
    value_type
{
    BOOST_ASSERT(v_);
    return { v_, static_cast<
        id_type>(i) };
}

template<class Allocator>
string_type<Allocator>
params::
at( string_view key,
    Allocator const& a) const
{
    BOOST_ASSERT(v_);
    auto const it = find(key);
    if(it == end())
        out_of_range::raise();
    return it->value(a);
}

//----------------------------------------------------------

template<class Allocator>
string_type<Allocator>
params::
value_type::
key(Allocator const& a) const
{
    return detail::decode(
        encoded_key(), a);
}

template<class Allocator>
string_type<Allocator>
params::
value_type::
value(Allocator const& a) const
{
    return detail::decode(
        encoded_value(), a);
}

} // url
} // boost

#endif
