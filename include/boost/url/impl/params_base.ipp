//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_IMPL_PARAMS_BASE_IPP
#define BOOST_URL_IMPL_PARAMS_BASE_IPP

#include <boost/url/params_base.hpp>
#include <ostream>

namespace boost {
namespace urls {

param_view
params_base::
iterator::
dereference() const
{
    if(! valid_)
    {
        // VFALCO This could be done with
        // one string instead of two.
        key_.acquire();
        value_.acquire();
        decode_opts opt;
        opt.plus_to_space = true;
        param_pct_view qp = it_.dereference();
        qp.key.decode(opt,
            string_token::preserve_size(*key_));
        has_value_ = qp.has_value;
        if(has_value_)
            qp.value.decode(opt,
                string_token::preserve_size(*value_));
        valid_ = true;
    };
    return {
        string_view(key_->data(), it_.dk),
        string_view(value_->data(), it_.dv),
        has_value_};
}

params_base::
iterator::
iterator(
    detail::query_ref const& ref) noexcept
    : it_(ref)
{
}

params_base::
iterator::
iterator(
    detail::query_ref const& ref, int) noexcept
    : it_(ref, 0)
{
}

params_base::
iterator::
iterator(
    iterator const& other) noexcept
    : it_(other.it_)
{
    // don't copy recycled_ptr
}

auto
params_base::
iterator::
operator=(
    iterator const& other) noexcept ->
        iterator&
{
    // don't copy recycled_ptr
    it_ = other.it_;
    valid_ = false;
    return *this;
}

//------------------------------------------------
//
// params_base
//
//------------------------------------------------

params_base::
params_base(
    detail::query_ref const& ref) noexcept
    : ref_(ref)
{
}

pct_string_view
params_base::
buffer() const noexcept
{
    return ref_.buffer();
}

bool
params_base::
empty() const noexcept
{
    return ref_.nparam() == 0;
}

std::size_t
params_base::
size() const noexcept
{
    return ref_.nparam();
}

auto
params_base::
begin() const noexcept ->
    iterator
{
    return { ref_ };
}

auto
params_base::
end() const noexcept ->
    iterator
{
    return { ref_, 0 };
}

//------------------------------------------------

std::size_t
params_base::
count(
    string_view key,
    ignore_case_param ic) const noexcept
{
    std::size_t n = 0;
    auto it = find(key, ic);
    auto const end_ = end();
    while(it != end_)
    {
        ++n;
        ++it;
        it = find(it, key, ic);
    }
    return n;
}

//------------------------------------------------
//
// (implementation)
//
//------------------------------------------------

detail::params_iter_impl
params_base::
find_impl(
    detail::params_iter_impl it,
    string_view key,
    ignore_case_param ic) const noexcept
{
    detail::params_iter_impl end_(ref_, 0);
    if(! ic)
    {
        for(;;)
        {
            if(it.equal(end_))
                return it;
            if(*it.key() == key)
                return it;
            it.increment();
        }
    }
    for(;;)
    {
        if(it.equal(end_))
            return it;
        if( grammar::ci_is_equal(
                *it.key(), key))
            return it;
        it.increment();
    }
}

detail::params_iter_impl
params_base::
find_last_impl(
    detail::params_iter_impl it,
    string_view key,
    ignore_case_param ic) const noexcept
{
    detail::params_iter_impl begin_(ref_);
    if(! ic)
    {
        for(;;)
        {
            if(it.equal(begin_))
                return { ref_, 0 };
            it.decrement();
            if(*it.key() == key)
                return it;
        }
    }
    for(;;)
    {
        if(it.equal(begin_))
            return { ref_, 0 };
        it.decrement();
        if(grammar::ci_is_equal(
                *it.key(), key))
            return it;
    }
}

//------------------------------------------------

std::ostream&
operator<<(
    std::ostream& os,
    params_base const& qp)
{
    os << qp.buffer();
    return os;
}

} // urls
} // boost

#endif
