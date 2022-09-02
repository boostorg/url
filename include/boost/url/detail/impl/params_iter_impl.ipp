//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_PARAMS_ITER_IMPL_IPP
#define BOOST_URL_DETAIL_IMPL_PARAMS_ITER_IMPL_IPP

#include <boost/url/decode_view.hpp>
#include <boost/url/detail/params_iter_impl.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {
namespace detail {

/*  pos offset from beginning of query,
            includes the leading '?'
    nk  size of key including '?' or '&'
    nv  size of value including '='
    dk  decoded key size, no '?' or '&'
    dv  decoded value size, no '='
    i   zero-based index of param
*/

params_iter_impl::
params_iter_impl(
    url_impl const& u_) noexcept
    : u(&u_)
{
    update();
}

params_iter_impl::
params_iter_impl(
    url_impl const& u_,
    int) noexcept
    : u(&u_)
    , pos(u->len(id_query))
    , i(u->nparam_)
{
}

params_iter_impl::
params_iter_impl(
    url_impl const& u_,
    std::size_t pos_,
    std::size_t i_) noexcept
    : u(&u_)
    , pos(pos_)
    , i(i_)
{
    update();
}

param_pct_view
params_iter_impl::
dereference() const noexcept
{
    BOOST_ASSERT(nk > 0);
    auto const p = u->cs_ +
        u->offset(id_query) + pos;
    BOOST_ASSERT(
        (pos == 0 && *p == '?') ||
        (pos > 0  && *p == '&'));
    if(nv)
        return {
            detail::make_pct_string_view(
                string_view(
                    p + 1, nk - 1),
                dk),
            detail::make_pct_string_view(
                string_view(
                    p + nk + 1, nv - 1),
                dv)};
    return {
        detail::make_pct_string_view(
            string_view(
                p + 1, nk - 1),
            dk)};
}

void
params_iter_impl::
increment() noexcept
{
    BOOST_ASSERT(
        i != u->nparam_);
    ++i;
    pos += nk + nv;
    update();
}

void
params_iter_impl::
decrement() noexcept
{
    BOOST_ASSERT(i != 0);
    BOOST_ASSERT(
        u->len(id_query) > 0);
    --i;
    dk = 0;
    dv = 0;
    auto p = u->cs_ +
        u->offset(id_query) + pos;
    auto end = p;
    for(;;)
    {
        --p;
        if( *p == '&' ||
            *p == '?')
        {
            // key only
            nk = static_cast<
                std::size_t>(
                    end - p);
            dk = nk - dk - 1;
            nv = 0;
            pos -= nk;
            return;
        }
        if(*p == '=')
            break;
        if(*p == '%')
            dk += 2;
    }
    // maybe value
    nv = static_cast<
        std::size_t>(
            end - p);
    for(;;)
    {
        --p;
        if( *p == '&' ||
            *p == '?')
            break;
        if(*p == '%')
        {
            dv += 2;
            continue;
        }
        if(*p == '=')
        {
            // maybe value
            nv = static_cast<
                std::size_t>(
                    end - p);
        }
    }
    // key
    nk = static_cast<
        std::size_t>(
            end - p - nv);
    dk = nk - dk - 1;
    dv = nv - dv - 1;
    pos -= nv + nk;
}

void
params_iter_impl::
update() noexcept
{
    dk = 0;
    dv = 0;
    auto s = u->get(id_query);
    s.remove_prefix(pos);
    if(s.empty())
    {
        // end
        BOOST_ASSERT(pos ==
            u->len(id_query));
        nk = 0;
        nv = 0;
        return;
    }
    BOOST_ASSERT(
        s.front() == '?' ||
        s.front() == '&');
    // key
    auto p0 = s.begin();
    auto const end = s.end();
    auto p = p0;
    for(;;)
    {
        ++p;
        if( p == end ||
            *p == '&')
        {
            // no value
            nk = static_cast<
                std::size_t>(
                    p - s.begin());
            dk = nk - dk - 1;
            nv = 0;
            return;
        }
        if(*p == '=')
            break;
        if(*p == '%')
        {
            BOOST_ASSERT(
                end - p >= 3);
            dk += 2;
            p += 2;
        }
    }
    nk = static_cast<
        std::size_t>(
            p - s.begin());
    dk = nk - dk - 1;
    p0 = p;
    // value
    for(;;)
    {
        ++p;
        if( p == end ||
            *p == '&')
            break;
        if(*p == '%')
        {
            BOOST_ASSERT(
                end - p >= 3);
            dv += 2;
            p += 2;
        }
    }
    nv = static_cast<
        std::size_t>(
            p - p0);
    dv = nv - dv - 1;
}

} // detail
} // url
} // boost

#endif
