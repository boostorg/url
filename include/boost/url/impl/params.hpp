//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_HPP
#define BOOST_URL_IMPL_PARAMS_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/detail/params_iterator_impl.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

class params::iterator
{
    detail::params_iterator_impl impl_;

    friend class params;

    iterator(
        string_view s,
        std::size_t nparam) noexcept
        : impl_(s, nparam)
    {
    }

    // end
    iterator(
        string_view s,
        std::size_t nparam,
        int) noexcept
        : impl_(s, nparam, 0)
    {
    }

    string_view
    encoded_key() const noexcept
    {
        return impl_.encoded_key();
    }

public:
    using value_type = query_param;
    using reference = query_param_view;
    using pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() = default;

    iterator&
    operator++() noexcept
    {
        impl_.increment();
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
        params_view::reference r0 =
            impl_.dereference();
        return {
            std::move(r0.key),
            std::move(r0.value),
            r0.has_value};
    }

    friend
    bool
    operator==(
        iterator const& a,
        iterator const& b) noexcept
    {
        return a.impl_.equal(b.impl_);
    }

    friend
    bool
    operator!=(
        iterator const& a,
        iterator const& b) noexcept
    {
        return !a.impl_.equal(b.impl_);
    }
};

//------------------------------------------------
//
// Members
//
//------------------------------------------------


inline
params&
params::
operator=(std::initializer_list<
    query_param_view> init)
{
    assign(init);
    return *this;
}

inline
void
params::
assign(std::initializer_list<
    query_param_view> init)
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

//--------------------------------------------
//
// Iterators
//
//--------------------------------------------

inline
auto
params::
begin() const noexcept ->
    iterator
{
    if (u_->u_.nparam_ > 0)
        return { u_->encoded_query(),
            u_->u_.nparam_ };
    return end();
}

inline
auto
params::
end() const noexcept ->
    iterator
{
    return { u_->encoded_query(),
        u_->u_.nparam_, 0 };
}

//------------------------------------------------
//
// Capacity
//
//------------------------------------------------

inline
std::size_t
params::
size() const noexcept
{
    return u_->u_.nparam_;
}

inline
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

inline
void
params::
clear() noexcept
{
    erase(begin(), end());
}

//------------------------------------------------

inline
auto
params::
insert(
    iterator before,
    query_param_view const& v) ->
        iterator
{
    return insert(
        before, &v, &v + 1);
}

inline
auto
params::
insert(
    iterator before,
    std::initializer_list<
        query_param_view> init) ->
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
        typename std::enable_if<
            !std::is_convertible<
                FwdIt, string_view
                >::value,
            iterator>::type
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
    BOOST_ASSERT(before.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(before.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());
    u_->edit_params(
        before.impl_.i_,
        before.impl_.i_,
        make_plain_params_iter(
            first, last),
        make_plain_params_iter(
            first, last));
    return std::next(begin(), before.impl_.i_);
}

//------------------------------------------------

inline
auto
params::
replace(
    iterator pos,
    query_param_view const& value) ->
        iterator
{
    return replace(
        pos,
        std::next(pos),
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
    BOOST_ASSERT(from.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(from.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());
    BOOST_ASSERT(to.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(to.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());
    u_->edit_params(
        from.impl_.i_,
        to.impl_.i_,
        make_plain_params_iter(
            first, last),
        make_plain_params_iter(
            first, last));
    return std::next(begin(), from.impl_.i_);
}

inline
auto
params::
replace(
    iterator from,
    iterator to,
    std::initializer_list<
        query_param_view> init) ->
    iterator
{
    return replace(
        from,
        to,
        init.begin(),
        init.end());
}

//------------------------------------------------

inline
auto
params::
replace(
    iterator pos,
    string_view key,
    string_view value) ->
        iterator
{
    using detail::
        make_plain_params_iter;
    query_param_view v{
        pct_encoded_view(key),
        pct_encoded_view(value),
        true };
    BOOST_ASSERT(pos.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(pos.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());
    u_->edit_params(
        pos.impl_.i_,
        pos.impl_.i_ + 1,
        make_plain_params_iter(
            &v, &v + 1),
        make_plain_params_iter(
            &v, &v + 1));
    return std::next(begin(), pos.impl_.i_);
}

inline
auto
params::
replace(
    iterator pos,
    string_view key) ->
        iterator
{
    BOOST_ASSERT(pos.impl_.begin_ ==
        u_->encoded_query().data());
    BOOST_ASSERT(pos.impl_.end_ ==
        u_->encoded_query().data() +
        u_->encoded_query().size());
    query_param_view v{
        pct_encoded_view(key), {}, false};
    u_->edit_params(
        pos.impl_.i_,
        pos.impl_.i_ + 1,
        detail::make_enc_params_iter(
            &v, &v + 1),
        detail::make_enc_params_iter(
            &v, &v + 1));
    return std::next(begin(), pos.impl_.i_);
}

inline
auto
params::
insert(
    iterator before,
    string_view key,
    string_view value) ->
        iterator
{
    return insert(
        before,
        query_param_view{
            pct_encoded_view(key),
            pct_encoded_view(value),
            true });
}

inline
auto
params::
insert(
    iterator before,
    string_view key) ->
        iterator
{
    return insert(
        before,
        query_param_view{
            pct_encoded_view(key), {}, false});
}

//------------------------------------------------

inline
auto
params::
erase(iterator pos) ->
    iterator
{
    return erase(pos, std::next(pos));
}

//------------------------------------------------

inline
auto
params::
append(
    string_view key) ->
        iterator
{
    return insert(
        end(), query_param_view{
            pct_encoded_view(key), {}, false});
}

inline
auto
params::
append(
    string_view key,
    string_view value) ->
        iterator
{
    return insert(
        end(), query_param_view{
            pct_encoded_view(key),
            pct_encoded_view(value),
            true});
}

inline
void
params::
push_back(
    query_param_view const& v)
{
    insert(end(), v);
}

//------------------------------------------------
//
// Lookup
//
//------------------------------------------------

inline
auto
params::
find(string_view key) const noexcept ->
    iterator
{
    return find(begin(), key);
}

inline
bool
params::
contains(string_view key) const noexcept
{
    return find(key) != end();
}

} // urls
} // boost

#endif
