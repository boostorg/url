//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_IMPL_PARAMS_ENCODED_VIEW_HPP
#define BOOST_URL_IMPL_PARAMS_ENCODED_VIEW_HPP

#include <boost/url/detail/except.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

inline
params_encoded_view::
params_encoded_view(
    url_base& u) noexcept
    : params_encoded_base(u.u_)
    , u_(&u)
{
}

//------------------------------------------------
//
// Special Members
//
//------------------------------------------------

inline
params_encoded_view&
params_encoded_view::
operator=(
    params_encoded_view const& other)
{
    assign(other.begin(), other.end());
    return *this;
}

inline
params_encoded_view&
params_encoded_view::
operator=(std::initializer_list<
    param_pct_view> init)
{
    assign(init.begin(), init.end());
    return *this;
}

inline
params_encoded_view::
operator
params_const_encoded_view() const noexcept
{
    return {*impl_};
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

inline
void
params_encoded_view::
assign(std::initializer_list<
    param_pct_view> init)
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
                FwdIt>::reference,
            param_pct_view>::value,
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
    param_pct_view const& v) ->
        iterator
{
    return insert(end(), v);
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
                FwdIt>::reference,
            param_pct_view>::value,
        "Type requirements not met");

    return insert(
        end(), first, last);
}

inline
auto
params_encoded_view::
append_list(
    std::initializer_list<
        param_pct_view> init) ->
    iterator
{
    return insert_list(end(), init);
}

//------------------------------------------------

inline
auto
params_encoded_view::
insert(
    iterator before,
    param_pct_view const& v) ->
        iterator
{
    return insert(
        before, &v, &v + 1);
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
                FwdIt>::reference,
            param_pct_view>::value,
        "Type requirements not met");

    return insert(
        before,
        first,
        last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

inline
auto
params_encoded_view::
insert_list(
    iterator before,
    std::initializer_list<
        param_pct_view> init) ->
    iterator
{
    return insert(before,
        init.begin(), init.end());
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
    param_pct_view const& value) ->
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
        param_pct_view> init) ->
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
                FwdIt>::reference,
            param_pct_view>::value,
        "Type requirements not met");

    return u_->edit_params(
        from.it_, to.it_,
        detail::make_params_encoded_iter(
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
