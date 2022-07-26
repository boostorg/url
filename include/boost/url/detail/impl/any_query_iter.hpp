//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_IMPL_ANY_QUERY_ITER_HPP
#define BOOST_URL_DETAIL_IMPL_ANY_QUERY_ITER_HPP

#include <boost/url/query_param.hpp>

namespace boost {
namespace urls {
namespace detail {

template<class FwdIt>
bool
plain_params_iter<FwdIt>::
measure(
    std::size_t& n,
    error_code&) noexcept
{
    if(it_ == end_)
        return false;
    query_param_encoded_view v(*it_++);
    if(v.has_value)
        measure_impl(
            v.key, &v.value, n);
    else
        measure_impl(
            v.key, nullptr, n);
    return true;
}

template<class FwdIt>
void
plain_params_iter<FwdIt>::
copy(
    char*& dest,
    char const* end) noexcept
{
    query_param_encoded_view v(*it_++);
    if(v.has_value)
        copy_impl(
            v.key, &v.value,
                dest, end);
    else
        copy_impl(
            v.key, nullptr,
                dest, end);
}

template<class FwdIt>
bool
plain_value_iter<FwdIt>::
measure(
    std::size_t& n,
    error_code&) noexcept
{
    if(it_ == end_)
        return false;
    query_param_encoded_view v(*it_++);
    if(v.has_value)
        measure_impl(
            v.key, &v.value, n);
    else
        measure_impl(
            v.key, nullptr, n);
    return true;
}

template<class FwdIt>
void
plain_value_iter<FwdIt>::
copy(
    char*& dest,
    char const* end) noexcept
{
    query_param_encoded_view v(*it_++);
    if(v.has_value)
        copy_impl(
            v.key, &v.value,
                dest, end);
    else
        copy_impl(
            v.key, nullptr,
                dest, end);
}

} // detail
} // urls
} // boost

#endif
