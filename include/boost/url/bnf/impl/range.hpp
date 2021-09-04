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
class range<T>::iterator
{
    T v_;
    char const* next_ = nullptr;
    char const* end_ = nullptr;
    begin_t begin_ = nullptr;
    increment_t increment_ = nullptr;

    friend class range;

    iterator(
        string_view s,
        begin_t begin,
        increment_t increment)
        : end_(s.data() + s.size())
        , begin_(begin)
        , increment_(increment)
    {
        using namespace urls::detail;
        error_code ec;
        next_ = begin_(
            s.data(), end_, ec, v_);
        if(ec == error::end)
            next_ = nullptr;
        else if(ec.failed())
            throw_system_error(ec,
                BOOST_CURRENT_LOCATION);
    }

    explicit
    iterator(
        char const* end,
        begin_t begin,
        increment_t increment)
        : end_(end)
        , begin_(begin)
        , increment_(increment)
    {
    }

public:
    using value_type = T;
    using pointer = value_type const*;
    using reference = value_type const&;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() noexcept = default;

    bool
    operator==(
        iterator const& other) const
    {
        return
            next_ == other.next_ &&
            end_ == other.end_ &&
            begin_ == other.begin_ &&
            increment_ == other.increment_;
    }

    bool
    operator!=(
        iterator const& other) const
    {
        return !(*this == other);
    }

    value_type const&
    operator*() const noexcept
    {
        return v_;
    }

    value_type const*
    operator->() const noexcept
    {
        return &v_;
    }

    iterator&
    operator++()
    {
        using namespace urls::detail;
        error_code ec;
        next_ = increment_(
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

    iterator
    operator++(int)
    {
        auto temp = *this;
        ++(*this);
        return temp;
    }
};

//------------------------------------------------

template<class T>
auto
range<T>::
begin() const ->
    iterator
{
    return iterator(s_,
        begin_, increment_);
}

template<class T>
auto
range<T>::
end() const ->
    iterator
{
    return iterator(
        s_.data() + s_.size(),
        begin_, increment_);
}

template<class T, class U>
char const*
parse_range(
    char const* start,
    char const* end,
    error_code& ec,
    range<T>& t,
    U const&)
{
    T v;
    std::size_t n = 0;
    auto it = U::begin(
        start, end, ec, v);
    if(ec == error::end)
    {
        t = range<T>(
            string_view(start,
                it - start), n,
            &U::begin,
            &U::increment);
        ec = {};
        return it;
    }
    if(ec.failed())
    {
        t = {};
        return start;
    }
    for(;;)
    {
        ++n;
        it = U::increment(
            it, end, ec, v);
        if(ec == error::end)
            break;
        if(ec.failed())
        {
            t = {};
            return start;
        }
    }
    t = range<T>(
        string_view(start,
            it - start), n,
        &U::begin,
        &U::increment);
    ec = {};
    return it;
}

} // bnf
} // urls
} // boost

#endif
