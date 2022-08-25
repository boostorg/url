//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_CONST_VIEW_HPP
#define BOOST_URL_IMPL_PARAMS_CONST_VIEW_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/detail/params_iter_impl.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

class params_const_view::iterator
{
    detail::params_iter_impl it_;

    friend class params_const_view;

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

public:
    using value_type = param;
    using reference = param_decode_view;
    using pointer = param_decode_view;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() = default;

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
        iterator const& a,
        iterator const& b) noexcept
    {
        return a.it_.equal(b.it_);
    }

    friend
    bool
    operator!=(
        iterator const& a,
        iterator const& b) noexcept
    {
        return !a.it_.equal(b.it_);
    }
};

//------------------------------------------------
//
// Capacity
//
//------------------------------------------------

inline
bool
params_const_view::
empty() const noexcept
{
    return u_->u_.nparam_ == 0;
}

inline
std::size_t
params_const_view::
size() const noexcept
{
    return u_->u_.nparam_;
}

//------------------------------------------------
//
// Lookup
//
//------------------------------------------------

inline
auto
params_const_view::
find(string_view key) const noexcept ->
    iterator
{
    return find(begin(), key);
}

inline
bool
params_const_view::
contains(
    string_view key) const noexcept
{
    return find(key) != end();
}

} // urls
} // boost

#endif
