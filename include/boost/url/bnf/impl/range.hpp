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
#include <boost/assert.hpp>
#include <iterator>

namespace boost {
namespace urls {
namespace bnf {

template<class T>
bool
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
    if(! T::begin(it, end, ec, t0))
    {
        if(ec == urls::error::end)
            goto finish;
        if(ec.failed())
            return false;
    }
    for(;;)
    {
        ++n;
        if(! T::increment(
            it, end, ec, t0))
        {
            if(ec == urls::error::end)
                break;
            if(ec.failed())
                return false;
        }
    }
finish:
    t.str = string_view(
        start, it - start);
    t.count = n;
    ec = {};
    return true;
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

inline
bool
parse(
    char const*& it,
    char const* end,
    error_code& ec,
    range_& t)
{
    return t.fp_(it, end, ec, t);
}

//------------------------------------------------

template<
    class V,
    class R>
class range<V, R>::iterator
{
    friend class range<V,R>;

    char const* next_ = nullptr;
    char const* end_ = nullptr;
    fp_t increment_ = nullptr;
    R v_;

    iterator(
        string_view s,
        fp_t begin,
        fp_t increment)
        : next_(s.data())
        , end_(s.data() + s.size())
        , increment_(increment)
    {
        error_code ec;
        begin(next_, end_, ec, v_);
        BOOST_ASSERT(! ec.failed());
    }

    iterator(
        string_view s,
        fp_t increment) noexcept
        : next_(nullptr)
        , end_(s.data() + s.size())
        , increment_(increment)
    {
    }

public:
    using value_type = V;
    using reference = R;
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
    operator*()
    {
        return v_;
    }

    bool
    operator==(
        iterator other) const noexcept
    {
        BOOST_ASSERT(
            increment_ ==
            other.increment_);

        return
            next_ == other.next_ &&
            end_ == other.end_;
    }

    bool
    operator!=(
        iterator other) const noexcept
    {
        BOOST_ASSERT(
            increment_ ==
            other.increment_);

        return
            next_ != other.next_ ||
            end_ != other.end_;
    }

    iterator&
    operator++() noexcept
    {
        BOOST_ASSERT(next_ != nullptr);
        error_code ec;
        increment_(next_, end_, ec, v_);
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

template<
    class V,
    class R>
auto
range<V, R>::
begin() const ->
    iterator
{
    return iterator(
        s_, begin_, increment_);
}

template<
    class V,
    class R>
auto
range<V, R>::
end() const ->
    iterator
{
    return iterator(
        s_, increment_);
}

} // bnf
} // urls
} // boost

#endif
