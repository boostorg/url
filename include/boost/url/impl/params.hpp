//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_HPP
#define BOOST_URL_IMPL_PARAMS_HPP

#include <boost/url/detail/except.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

class params::iterator
{
    url const* u_ = nullptr;
    std::size_t i_ = 0;
    const_string::factory a_;

    friend class params;

    iterator(
        url const& u,
        std::size_t i,
        const_string::factory a) noexcept
        : u_(&u)
        , i_(i)
        , a_(a)
    {
    }

public:
    using value_type = query_param;
    using reference = params::reference;
    using pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::random_access_iterator_tag;

    iterator() = default;

    iterator&
    operator++() noexcept
    {
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

    BOOST_URL_DECL
    reference
    operator*() const;

    friend
    bool
    operator==(
        iterator a,
        iterator b) noexcept
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return a.u_ == b.u_ &&
            a.i_ == b.i_;
    }

    friend
    bool
    operator!=(
        iterator a,
        iterator b) noexcept
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return a.u_ != b.u_ ||
            a.i_ != b.i_;
    }

    // LegacyRandomAccessIterator

    iterator&
    operator+=(
        ptrdiff_t n) noexcept
    {
        i_ += n;
        return *this;
    }

    friend
    iterator
    operator+(
        iterator it,
        ptrdiff_t n) noexcept
    {
        return {
            *it.u_,
            it.i_ + n,
            it.a_ };
    }

    friend
    iterator
    operator+(
        ptrdiff_t n,
        iterator it) noexcept
    {
        return {
            *it.u_,
            it.i_ + n,
            it.a_ };
    }

    iterator&
    operator-=(
        ptrdiff_t n) noexcept
    {
        i_ -= n;
        return *this;
    }

    friend
    iterator
    operator-(
        iterator it,
        ptrdiff_t n) noexcept
    {
        return {
            *it.u_,
            it.i_ - n,
            it.a_ };
    }

    friend
    std::ptrdiff_t
    operator-(
        iterator a,
        iterator b) noexcept
    {
        BOOST_ASSERT(a.u_ == b.u_);
        return static_cast<
            std::ptrdiff_t>(
                a.i_) - b.i_;
    }

    reference
    operator[](ptrdiff_t n) const
    {
        return *(*this + n);
    }

    friend
    bool
    operator<(
        iterator a,
        iterator b)
    {
        BOOST_ASSERT(
            a.u_ == b.u_);
        return a.i_ < b.i_;
    }

    friend
    bool
    operator>(
        iterator a,
        iterator b)
    {
        BOOST_ASSERT(
            a.u_ == b.u_);
        return b < a;
    }

    friend
    bool
    operator>=(
        iterator a,
        iterator b)
    {
        BOOST_ASSERT(
            a.u_ == b.u_);
        return !(a < b);
    }

    friend
    bool
    operator<=(
        iterator a,
        iterator b)
    {
        BOOST_ASSERT(
            a.u_ == b.u_);
        return !(a > b);
    }
};

//------------------------------------------------
//
// Members
//
//------------------------------------------------

template<class Allocator>
params::
params(
    url& u,
    Allocator const& a)
    : u_(&u)
    , a_(a)
{
}

params&
params::
operator=(std::initializer_list<
    value_type> init)
{
    assign(init);
    return *this;
}

void
params::
assign(std::initializer_list<
    value_type> init)
{
    assign(init.begin(), init.end());
}

template<class FwdIt>
void
params::
assign(FwdIt first, FwdIt last)
{
    assign(first, last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

template<class FwdIt>
void
params::
assign(FwdIt first, FwdIt last,
    std::forward_iterator_tag)
{
    using detail::
        make_plain_params_iter;
    u_->edit_params(
        0,
        size(),
        make_plain_params_iter(
            first, last),
        make_plain_params_iter(
            first, last));
}

//------------------------------------------------
//
// Element Access
//
//------------------------------------------------

auto
params::
at(std::size_t pos) const ->
    reference
{
    if(pos >= size())
        detail::throw_out_of_range(
            BOOST_CURRENT_LOCATION);
    return (*this)[pos];
}

auto
params::
front() const ->
    reference
{
    BOOST_ASSERT(! empty());
    return (*this)[0];
}

auto
params::
back() const ->
    reference
{
    BOOST_ASSERT(! empty());
    return (*this)[size() - 1];
}

//--------------------------------------------
//
// Iterators
//
//--------------------------------------------

auto
params::
begin() const noexcept ->
    iterator
{
    return { *u_, 0, a_ };
}

auto
params::
end() const noexcept ->
    iterator
{
    return { *u_, size(), a_ };
}

//------------------------------------------------
//
// Capacity
//
//------------------------------------------------

std::size_t
params::
size() const noexcept
{
    return u_->nparam_;
}

bool
params::
empty() const noexcept
{
    return size() == 0;
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

void
params::
clear() noexcept
{
    erase(begin(), end());
}

//------------------------------------------------

auto
params::
insert(
    iterator before,
    value_type const& v) ->
        iterator
{
    return insert(
        before, &v, &v + 1);
}

auto
params::
insert(
    iterator before,
    std::initializer_list<
        value_type> init) ->
            iterator
{
    return insert(before,
        init.begin(), init.end());
}

template<class FwdIt>
auto
params::
insert(
    iterator before,
    FwdIt first,
    FwdIt last) ->
        iterator
{
    return insert(before, first, last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

template<class FwdIt>
auto
params::
insert(
    iterator before,
    FwdIt first,
    FwdIt last,
    std::forward_iterator_tag) ->
        iterator
{
    using detail::
        make_plain_params_iter;
    BOOST_ASSERT(before.u_ == u_);
    u_->edit_params(
        before.i_,
        before.i_,
        make_plain_params_iter(
            first, last),
        make_plain_params_iter(
            first, last));
    return before;
}

//------------------------------------------------

auto
params::
replace(
    iterator pos,
    value_type const& value) ->
        iterator
{
    return replace(
        pos,
        pos + 1,
        &value,
        &value + 1);
}

template<class FwdIt>
auto
params::
replace(
    iterator from,
    iterator to,
    FwdIt first,
    FwdIt last) ->
        iterator
{
    using detail::
        make_plain_params_iter;
    BOOST_ASSERT(from.u_ == u_);
    BOOST_ASSERT(to.u_ == u_);
    u_->edit_params(
        from.i_,
        to.i_,
        make_plain_params_iter(
            first, last),
        make_plain_params_iter(
            first, last));
    return from;
}

auto
params::
replace(
    iterator from,
    iterator to,
    std::initializer_list<
        value_type> init) ->
    iterator
{
    return replace(
        from,
        to,
        init.begin(),
        init.end());
}

//------------------------------------------------

auto
params::
emplace_at(
    iterator pos,
    string_view key,
    string_view value) ->
        iterator
{
    using detail::
        make_plain_params_iter;
    value_type v{
        key, value, true };
    BOOST_ASSERT(pos.u_ == u_);
    u_->edit_params(
        pos.i_,
        pos.i_ + 1,
        make_plain_params_iter(
            &v, &v + 1),
        make_plain_params_iter(
            &v, &v + 1));
    return pos;
}

auto
params::
emplace_at(
    iterator pos,
    string_view key) ->
        iterator
{
    BOOST_ASSERT(pos.u_ == u_);
    value_type v{key,
        string_view{}, false};
    u_->edit_params(
        pos.i_,
        pos.i_ + 1,
        detail::make_enc_params_iter(
            &v, &v + 1),
        detail::make_enc_params_iter(
            &v, &v + 1));
    return pos;
}

auto
params::
emplace_before(
    iterator before,
    string_view key,
    string_view value) ->
        iterator
{
    return insert(
        before,
        value_type{
            key,
            value,
            true});
}

auto
params::
emplace_before(
    iterator before,
    string_view key) ->
        iterator
{
    return insert(
        before,
        value_type{
            key,
            string_view{},
            false});
}

//------------------------------------------------

auto
params::
erase(iterator pos) ->
    iterator
{
    return erase(pos, pos + 1);
}

//------------------------------------------------

auto
params::
emplace_back(
    string_view key) ->
        iterator
{
    return insert(
        end(), value_type{
            key, {}, false});
}

auto
params::
emplace_back(
    string_view key,
    string_view value) ->
        iterator
{
    return insert(
        end(), value_type{
            key, value, true});
}

void
params::
push_back(
    value_type const& v)
{
    insert(end(), v);
}


void
params::
pop_back() noexcept
{
    erase(end() - 1);
}

//------------------------------------------------
//
// Lookup
//
//------------------------------------------------

auto
params::
find(string_view key) const noexcept ->
    iterator
{
    return find(begin(), key);
}

bool
params::
contains(string_view key) const noexcept
{
    return find(key) != end();
}

} // urls
} // boost

#endif
