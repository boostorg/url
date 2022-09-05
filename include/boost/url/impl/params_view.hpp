//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_VIEW_HPP
#define BOOST_URL_IMPL_PARAMS_VIEW_HPP

#include <boost/url/params_const_view.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/grammar/recycled.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

inline
params_view::
params_view(
    url_base& u) noexcept
    : params_base(u.u_)
    , u_(&u)
{
}

inline
params_view::
operator
params_const_view() const noexcept
{
    return {*impl_};
}

//------------------------------------------------
//
// Special Members
//
//------------------------------------------------

inline
auto
params_view::
operator=(params_view const& other) ->
    params_view&
{
    assign(other.begin(), other.end());
    return *this;
}

inline
auto
params_view::
operator=(std::initializer_list<
    param_view> init) ->
        params_view&
{
    assign(init.begin(), init.end());
    return *this;
}

//------------------------------------------------
//
// Modifiers
//
//------------------------------------------------

inline
void
params_view::
clear() noexcept
{
    u_->remove_query();
}

//------------------------------------------------

inline
void
params_view::
assign(std::initializer_list<
    param_view> init)
{
    assign(init.begin(), init.end());
}

template<class FwdIt>
void
params_view::
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
            param_view>::value,
        "Type requirements not met");

    assign(first, last,
        typename std::iterator_traits<
            FwdIt>::iterator_category{});
}

//------------------------------------------------

inline
auto
params_view::
append(
    param_view const& v) ->
        iterator
{
    return insert(end(), v);
}

template<class FwdIt>
auto
params_view::
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
            param_view>::value,
        "Type requirements not met");

    return insert(
        end(), first, last);
}

inline
auto
params_view::
append_list(
    std::initializer_list<
        param_view> init) ->
    iterator
{
    return insert_list(end(), init);
}

//------------------------------------------------

inline
auto
params_view::
insert(
    iterator before,
    param_view const& v) ->
        iterator
{
    return insert(
        before, &v, &v + 1);
}

template<class FwdIt>
auto
params_view::
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
            param_view>::value,
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
params_view::
insert_list(
    iterator before,
    std::initializer_list<
        param_view> init) ->
    iterator
{
    return insert(before,
        init.begin(), init.end());
}

//------------------------------------------------

inline
auto
params_view::
erase(
    iterator pos) noexcept ->
    iterator
{
    return erase(
        pos, std::next(pos));
}

inline
auto
params_view::
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
params_view::
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
params_view::
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
params_view::
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
params_view::
assign(FwdIt first, FwdIt last,
    std::forward_iterator_tag)
{
    u_->edit_params(
        begin().it_,
        end().it_,
        detail::make_params_iter(
            first, last));
}

template<class FwdIt>
auto
params_view::
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
        detail::make_params_iter(
            first, last));
}

} // urls
} // boost

#endif
