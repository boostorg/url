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
    func_ptr begin_ = nullptr;
    func_ptr increment_ = nullptr;

    friend class range;

    iterator(
        string_view s,
        func_ptr begin,
        func_ptr increment)
        : next_(s.data())
        , end_(s.data() + s.size())
        , begin_(begin)
        , increment_(increment)
    {
        using namespace urls::detail;
        error_code ec;
        next_ = s.data();
        if(! begin_(
            next_, end_, ec, v_))
        {
            if(ec == error::end)
                next_ = nullptr;
            else if(ec.failed())
                throw_system_error(ec,
                    BOOST_CURRENT_LOCATION);
        }
    }

    iterator(
        char const* end,
        func_ptr begin,
        func_ptr increment) noexcept
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
        if(increment_(
            next_, end_, ec, v_))
            return *this;
        if(ec == error::end)
        {
            next_ = nullptr;
            return *this;
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
bool
parse_range(
    char const*& it,
    char const* const end,
    error_code& ec,
    range<T>& t,
    U const&)
{
    T v;
    auto start = it;
    std::size_t n = 0;
    if(! U::begin(it, end, ec, v))
    {
        if(ec == error::end)
        {
            t = range<T>(
                string_view(start,
                    it - start), n,
                &U::begin,
                &U::increment);
            ec = {};
            return true;
        }
        if(ec.failed())
        {
            t = {};
            return false;
        }
    }
    for(;;)
    {
        ++n;
        if(! U::increment(
            it, end, ec, v))
        {
            if(ec == error::end)
                break;
            if(ec.failed())
            {
                t = {};
                return false;
            }
        }
    }
    t = range<T>(
        string_view(start,
            it - start), n,
        &U::begin,
        &U::increment);
    ec = {};
    return true;
}

} // bnf
} // urls
} // boost

#endif
