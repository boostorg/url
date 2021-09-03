//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_RANGE_HPP
#define BOOST_URL_BNF_IMPL_RANGE_HPP

#include <boost/url/detail/except.hpp>

namespace boost {
namespace urls {
namespace bnf {

template<class T>
range<T>::
iterator::
iterator(string_view s)
    : end_(s.data() + s.size())
{
    error_code ec;
    next_ = T::begin(
        s.data(), end_, ec, v_);
    if(ec == error::end)
        next_ = nullptr;
    else if(ec.failed())
        urls::detail::throw_system_error(ec,
            BOOST_CURRENT_LOCATION);
}

template<class T>
auto
range<T>::
iterator::
operator++() ->
    iterator&
{
    using namespace urls::detail;
    error_code ec;
    next_ = T::increment(
        next_, end_, ec, v_);
    if(ec == error::end)
    {
        next_ = nullptr;
        ec = {};
    }
    if(ec.failed())
        throw_system_error(ec,
            BOOST_CURRENT_LOCATION);
    return *this;
}

template<class T>
auto
range<T>::
begin(error_code& ec) const ->
    iterator
{
    iterator it(s_, ec);
    if(! ec)
        return it;
    return iterator(
        s_.data() + s_.size());
    // VFALCO is this better than above?
    //return iterator();
}

template<class T>
auto
range<T>::
begin() const ->
    iterator
{
    return iterator(s_);
}

template<class T>
auto
range<T>::
end() const ->
    iterator
{
    return iterator(
        s_.data() + s_.size());
}

} // bnf
} // urls
} // boost

#endif
