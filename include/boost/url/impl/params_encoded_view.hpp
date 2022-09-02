//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_ENCODED_VIEW_HPP
#define BOOST_URL_IMPL_PARAMS_ENCODED_VIEW_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/detail/params_iter_impl.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

//------------------------------------------------

class params_encoded_view::iterator
{
    detail::params_iter_impl it_;

    friend class params_encoded_view;

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
    using reference = param_view;
    using pointer = param_view;
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
        return ! a.it_.equal(b.it_);
    }
};

//------------------------------------------------
//
// Special Members
//
//------------------------------------------------

inline
params_encoded_view&
params_encoded_view::
operator=(std::initializer_list<
    reference> init)
{
    assign(init);
    return *this;
}

//------------------------------------------------
//
// Observers
//
//------------------------------------------------

inline
bool
params_encoded_view::
empty() const noexcept
{
    return u_->u_.nparam_ == 0;
}

inline
std::size_t
params_encoded_view::
size() const noexcept
{
    return u_->u_.nparam_;
}

inline
auto
params_encoded_view::
begin() const noexcept ->
    iterator
{
    return { *u_ };
}

inline
auto
params_encoded_view::
end() const noexcept ->
    iterator
{
    return { *u_, 0 };
}

inline
bool
params_encoded_view::
contains(
    string_view key,
    ignore_case_param ic) const
{
    return find(begin(),
        key, ic) != end();
}

inline
auto
params_encoded_view::
find(
    string_view key,
    ignore_case_param ic) const ->
        iterator
{
    return find(
        begin(), key, ic);
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

inline
void
params_encoded_view::
clear() noexcept
{
    u_->remove_query();
}

//------------------------------------------------

inline
void
params_encoded_view::
assign(std::initializer_list<
    reference> init)
{
    assign(init.begin(), init.end());
}

template<class FwdIt>
void
params_encoded_view::
assign(FwdIt first, FwdIt last)
{
/*  If you get a compile error here, it
    means that the iterators you passed
    do not meet the requirements stated
    in the documentation.
*/
    static_assert(
        std::is_convertible<
            typename std::iterator_traits<
                FwdIt>::value_type,
            param_view>::value,
        "Type requirements not met");

    assign(first, last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

//------------------------------------------------

inline
auto
params_encoded_view::
append(
    param_view const& v) ->
        iterator
{
    return insert(end(), v);
}

inline
auto
params_encoded_view::
append(
    std::initializer_list<
        param_view> init) ->
    iterator
{
    return insert(end(), init);
}

template<class FwdIt>
auto
params_encoded_view::
append(FwdIt first, FwdIt last) ->
    iterator
{
/*  If you get a compile error here, it
    means that the iterators you passed
    do not meet the requirements stated
    in the documentation.
*/
    static_assert(
        std::is_convertible<
            typename std::iterator_traits<
                FwdIt>::value_type,
            param_view>::value,
        "Type requirements not met");

    return insert(
        end(), first, last);
}

//------------------------------------------------

inline
auto
params_encoded_view::
insert(
    iterator before,
    param_view const& v) ->
        iterator
{
    return insert(
        before, &v, &v + 1);
}

inline
auto
params_encoded_view::
insert(
    iterator before,
    std::initializer_list<
        param_view> init) ->
    iterator
{
    return insert(before,
        init.begin(), init.end());
}

template<class FwdIt>
auto
params_encoded_view::
insert(
    iterator before,
    FwdIt first,
    FwdIt last) ->
        iterator
{
/*  If you get a compile error here, it
    means that the iterators you passed
    do not meet the requirements stated
    in the documentation.
*/
    static_assert(
        std::is_convertible<
            typename std::iterator_traits<
                FwdIt>::value_type,
            param_view>::value,
        "Type requirements not met");

    return insert(
        before,
        first,
        last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

//------------------------------------------------

inline
auto
params_encoded_view::
erase(
    iterator pos) noexcept ->
    iterator
{
    return erase(
        pos, std::next(pos));
}

inline
auto
params_encoded_view::
erase(
    iterator first,
    iterator last) noexcept ->
        iterator
{
    string_view s("", 0);
    return u_->edit_params(
        first.it_,
        last.it_,
        detail::query_iter(s));
}

//------------------------------------------------

inline
auto
params_encoded_view::
replace(
    iterator pos,
    param_view const& value) ->
        iterator
{
    return replace(
        pos, std::next(pos),
            &value, &value + 1);
}

inline
auto
params_encoded_view::
replace(
    iterator from,
    iterator to,
    std::initializer_list<
        param_view> init) ->
    iterator
{
    return replace(from, to,
        init.begin(), init.end());
}

template<class FwdIt>
auto
params_encoded_view::
replace(
    iterator from,
    iterator to,
    FwdIt first,
    FwdIt last) ->
        iterator
{
/*  If you get a compile error here, it
    means that the iterators you passed
    do not meet the requirements stated
    in the documentation.
*/
    static_assert(
        std::is_convertible<
            typename std::iterator_traits<
                FwdIt>::value_type,
            param_view>::value,
        "Type requirements not met");

    return u_->edit_params(
        from.it_, to.it_,
        detail::make_params_iter(
            first, last));
}

//------------------------------------------------
//
// implementation
//
//------------------------------------------------

template<class FwdIt>
void
params_encoded_view::
assign(FwdIt first, FwdIt last,
    std::forward_iterator_tag)
{
    u_->edit_params(
        begin().it_,
        end().it_,
        detail::make_params_encoded_iter(
            first, last));
}

template<class FwdIt>
auto
params_encoded_view::
insert(
    iterator before,
    FwdIt first,
    FwdIt last,
    std::forward_iterator_tag) ->
        iterator
{
    return u_->edit_params(
        before.it_,
        before.it_,
        detail::make_params_encoded_iter(
            first, last));
}

} // urls
} // boost

#endif
