//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_GRAMMAR_IMPL_RANGE_HPP
#define BOOST_URL_GRAMMAR_IMPL_RANGE_HPP

#include <boost/url/detail/except.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/assert.hpp>
#include <iterator>

namespace boost {
namespace urls {
namespace grammar {

template<class T>
void
range_::
parse_impl(
    char const*& it,
    char const* end,
    error_code& ec,
    range_& t)
{
    typename T::value_type t0;
    auto start = it;
    std::size_t n = 0;
    T::begin(it, end, ec, t0);
    if(ec.failed())
    {
        if(ec == error::end)
        {
            ec = {};
            goto finish;
        }
        return;
    }
    for(;;)
    {
        ++n;
        T::increment(
            it, end, ec, t0);
        if(ec.failed())
        {
            if(ec == error::end)
            {
                ec = {};
                break;
            }
            return;
        }
    }
finish:
    t.str = string_view(
        start, it - start);
    t.count = n;
}

template<class T>
range_::
range_(T const*) noexcept
    : fp_(&range_::parse_impl<T>)
{
    // Type requirements not met!
    BOOST_STATIC_ASSERT(
        is_range<T>::value);
}

//------------------------------------------------

template<class R>
class range_base<R>::iterator
{
    friend class range_base<R>;

    char const* next_ = nullptr;
    char const* end_ = nullptr;
    typename R::reference v_;

    explicit
    iterator(string_view s)
        : next_(s.data())
        , end_(s.data() + s.size())
    {
        error_code ec;
        R::begin(next_, end_, ec, v_);
        BOOST_ASSERT(! ec.failed());
    }

    iterator(
        string_view s,
        int) noexcept
        : next_(nullptr)
        , end_(s.data() + s.size())
    {
    }

public:
    using value_type = typename R::value_type;
    using reference = typename R::reference;
    using pointer = void const*;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() = default;
    iterator(
        iterator const&) noexcept = default;
    iterator& operator=(
        iterator const&) noexcept = default;

    reference
    operator*() const noexcept
    {
        return v_;
    }

    bool
    operator==(
        iterator other) const noexcept
    {
        return
            next_ == other.next_ &&
            end_ == other.end_;
    }

    bool
    operator!=(
        iterator other) const noexcept
    {
        return
            next_ != other.next_ ||
            end_ != other.end_;
    }

    iterator&
    operator++() noexcept
    {
        BOOST_ASSERT(next_ != nullptr);
        error_code ec;
        R::increment(next_, end_, ec, v_);
        if(ec == error::end)
            next_ = nullptr;
        else
            BOOST_ASSERT(! ec.failed());
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }
};

//------------------------------------------------

template<class R>
auto
range_base<R>::
begin() const ->
    iterator
{
    return iterator(s_);
}

template<class R>
auto
range_base<R>::
end() const ->
    iterator
{
    return iterator(s_, 1);
}

template<class R>
void
range_base<R>::
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    std::size_t N,
    std::size_t M)
{
    std::size_t n = 0;
    auto const start = it;
    typename range<R>::reference v;

    R::begin(it, end, ec, v);
    if(ec.failed())
    {
        if(ec != error::end)
            return;
        if(n < N)
        {
            // too few
            ec = error::syntax;
            return;
        }

        // good
        ec = {};
        n_ = n;
        s_ = string_view(
            start, it - start);
        return;
    }

    for(;;)
    {
        ++n;
        R::increment(it, end, ec, v);
        if(ec.failed())
        {
            if(ec != error::end)
                return;
            ec = {};
            break;
        }
        if(n > M)
        {
            // too many
            ec = error::syntax;
            return;
        }
    }

    // good
    n_ = n;
    s_ = string_view(start, it - start);
}

} // grammar
} // urls
} // boost

#endif
