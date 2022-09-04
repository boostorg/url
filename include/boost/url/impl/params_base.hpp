//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_BASE_HPP
#define BOOST_URL_IMPL_PARAMS_BASE_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/grammar/recycled.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class params_view;
class params_const_view;
#endif

//------------------------------------------------

class params_base::iterator
{
    detail::params_iter_impl it_;
    grammar::recycled_ptr<
        std::string> key_ = nullptr;
    grammar::recycled_ptr<
        std::string> value_ = nullptr;
    bool has_value_ = false;
    bool valid_ = false;

    friend class params_base;
    friend class params_view;
    friend class params_const_view;

    // begin
    iterator(
        detail::url_impl const& impl) noexcept
        : it_(impl)
    {
    }

    // end
    iterator(
        detail::url_impl const& impl,
        int) noexcept
        : it_(impl, 0)
    {
    }

    iterator(
        detail::params_iter_impl const& it) noexcept
        : it_(it)
    {
    }

    BOOST_URL_DECL
    param_view
    dereference();

public:
    using value_type = param;
    using reference = param_view;
    using pointer = param_view;
    using difference_type =
        std::ptrdiff_t;
    using iterator_category =
        std::input_iterator_tag;

    iterator() = default;

    BOOST_URL_DECL
    iterator(
        iterator const&) noexcept;

    BOOST_URL_DECL
    iterator& operator=(
        iterator const&) noexcept;

    iterator&
    operator++() noexcept
    {
        valid_ = false;
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
        valid_ = false;
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
    operator*()
    {
        return dereference();
    }

    pointer const
    operator->()
    {
        return dereference();
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
params_base::
empty() const noexcept
{
    return impl_->nparam_ == 0;
}

inline
std::size_t
params_base::
size() const noexcept
{
    return impl_->nparam_;
}

inline
auto
params_base::
begin() const noexcept ->
    iterator
{
    return { *impl_ };
}

inline
auto
params_base::
end() const noexcept ->
    iterator
{
    return { *impl_, 0 };
}

inline
bool
params_base::
contains(
    string_view key,
    ignore_case_param ic) const noexcept
{
    return find(begin(),
        key, ic) != end();
}

inline
auto
params_base::
find(
    string_view key,
    ignore_case_param ic) const noexcept ->
        iterator
{
    return find_impl(
        begin().it_, key, ic);
}

inline
auto
params_base::
find(
    iterator it,
    string_view key,
    ignore_case_param ic) const noexcept ->
        iterator
{
    return find_impl(
        it.it_, key, ic);
}

inline
auto
params_base::
find_last(
    string_view key,
    ignore_case_param ic) const noexcept ->
        iterator
{
    return find_last_impl(
        end().it_, key, ic);
}

inline
auto
params_base::
find_last(
    iterator it,
    string_view key,
    ignore_case_param ic) const noexcept ->
        iterator
{
    return find_last_impl(
        it.it_, key, ic);
}

} // urls
} // boost

#endif
