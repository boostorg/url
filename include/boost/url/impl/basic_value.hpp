//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_BASIC_VALUE_HPP
#define BOOST_URL_IMPL_BASIC_VALUE_HPP

namespace boost {
namespace url {

view::
segments_type::
segments_type(
    basic_value const& v) noexcept
    : s_(v.s_)
    , pt_(&v.pt_)
{
}

view::
params_type::
params_type(
    basic_value const& v) noexcept
    : s_(v.s_)
    , pt_(&v.pt_)
{
}

//----------------------------------------------------------

class basic_value::segments_type::iterator
{
    friend segments_type;

    basic_value* v_;
    std::size_t off_;
    std::size_t n_;

    BOOST_URL_DECL
    iterator(
        basic_value* v,
        bool end) noexcept;

public:
    using value_type =
        segments_type::value_type;

    BOOST_URL_DECL
    iterator() noexcept;

    BOOST_URL_DECL
    value_type
    operator*() const noexcept;

    value_type
    operator->() const noexcept
    {
        return operator*();
    }

    bool
    operator==(
        iterator const& other) const noexcept
    {
        BOOST_ASSERT(
            v_ != other.v_ ||
            off_ != other.off_ || 
            n_ == other.n_);
        return
            v_ == other.v_ &&
            off_ == other.off_;
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return !(*this == other);
    }

    BOOST_URL_DECL
    iterator&
    operator++() noexcept;

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    BOOST_URL_DECL
    iterator&
    operator--() noexcept;

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

private:
    inline
    void
    parse() noexcept;
};

//----------------------------------------------------------

class basic_value::params_type::iterator
{
    friend params_type;

    basic_value* v_;
    std::size_t off_;
    std::size_t nk_;
    std::size_t nv_;

    BOOST_URL_DECL
    iterator(
        basic_value* v,
        bool end) noexcept;

public:
    using value_type =
        params_type::value_type;

    BOOST_URL_DECL
    iterator() noexcept;

    BOOST_URL_DECL
    value_type
    operator*() const noexcept;

    value_type
    operator->() const noexcept
    {
        return operator*();
    }

    bool
    operator==(
        iterator const& other) const noexcept
    {
        BOOST_ASSERT(
            v_ != other.v_ ||
            off_ != other.off_ || (
                nk_ == other.nk_ &&
                nv_ == other.nv_));
        return
            v_ == other.v_ &&
            off_ == other.off_;
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return !(*this == other);
    }

    BOOST_URL_DECL
    iterator&
    operator++() noexcept;

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    BOOST_URL_DECL
    iterator&
    operator--() noexcept;

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

private:
    inline
    void
    parse() noexcept;
};

//----------------------------------------------------------

template<class Allocator>
string_type<Allocator>
basic_value::
params_type::
at( string_view key,
    Allocator const& a) const
{
    auto const it = find(key);
    if(it == end())
        out_of_range::raise();
    return it->value(a);
}

//----------------------------------------------------------

auto
basic_value::
segments() const noexcept ->
    view::segments_type
{
    return view::segments_type(*this);
}

auto
basic_value::
segments() noexcept ->
    segments_type
{
    return segments_type(*this);
}

auto
basic_value::
params() const noexcept ->
    view::params_type
{
    return view::params_type(*this);
}

auto
basic_value::
params() noexcept ->
    params_type
{
    return params_type(*this);
}

} // url
} // boost

#endif
