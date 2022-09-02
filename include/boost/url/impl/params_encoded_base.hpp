//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_ENCODED_BASE_HPP
#define BOOST_URL_IMPL_PARAMS_ENCODED_BASE_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/detail/params_iter_impl.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

class params_encoded_base::iterator
{
    detail::params_iter_impl it_;

    friend class params_encoded_base;
    friend class params_encoded_view;
    friend class params_const_encoded_view;

    // begin
    iterator(
        url_view_base const& u) noexcept
        : it_(u.u_)
    {
    }

    // end
    iterator(
        url_view_base const& u,
        int) noexcept
        : it_(u.u_, 0)
    {
    }

    iterator(
        detail::params_iter_impl const& it) noexcept
        : it_(it)
    {
    }

public:
    using value_type = param;
    using reference = param_pct_view;
    using pointer = param_pct_view;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::input_iterator_tag;

    iterator() = default;
    iterator(iterator const&) = default;
    iterator& operator=(
        iterator const&) noexcept = default;

    iterator&
    operator++() noexcept
    {
        it_.increment();
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
        it_.decrement();
        return *this;
    }

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    reference
    operator*() const
    {
        return it_.dereference();
    }

    pointer
    operator->() const
    {
        return it_.dereference();
    }

    friend
    bool
    operator==(
        iterator const& it0,
        iterator const& it1) noexcept
    {
        return it0.it_.equal(it1.it_);
    }

    friend
    bool
    operator!=(
        iterator const& it0,
        iterator const& it1) noexcept
    {
        return ! it0.it_.equal(it1.it_);
    }
};

//------------------------------------------------
//
// Observers
//
//------------------------------------------------

inline
bool
params_encoded_base::
empty() const noexcept
{
    return cu_->u_.nparam_ == 0;
}

inline
std::size_t
params_encoded_base::
size() const noexcept
{
    return cu_->u_.nparam_;
}

inline
auto
params_encoded_base::
begin() const noexcept ->
    iterator
{
    return { *cu_ };
}

inline
auto
params_encoded_base::
end() const noexcept ->
    iterator
{
    return { *cu_, 0 };
}

inline
bool
params_encoded_base::
contains(
    pct_string_view key,
    ignore_case_param ic) const noexcept
{
    return find(begin(),
        key, ic) != end();
}

inline
auto
params_encoded_base::
find(
    pct_string_view key,
    ignore_case_param ic) const noexcept ->
        iterator
{
    return find_impl(
        begin().it_, key, ic);
}

inline
auto
params_encoded_base::
find(
    iterator it,
    pct_string_view key,
    ignore_case_param ic) const noexcept ->
        iterator
{
    return find_impl(
        it.it_, key, ic);
}

inline
auto
params_encoded_base::
find_last(
    pct_string_view key,
    ignore_case_param ic) const noexcept ->
        iterator
{
    return find_last_impl(
        end().it_, key, ic);
}

inline
auto
params_encoded_base::
find_last(
    iterator it,
    pct_string_view key,
    ignore_case_param ic) const noexcept ->
        iterator
{
    return find_last_impl(
        it.it_, key, ic);
}

} // urls
} // boost

#endif
