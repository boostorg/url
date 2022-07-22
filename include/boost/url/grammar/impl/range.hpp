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
#include <new>

namespace boost {
namespace urls {
namespace grammar {

//------------------------------------------------

// base class for the type-erased rule
template<class T>
struct range<T>::
    any_rule
{
    virtual ~any_rule() = default;
    virtual void copy(
        void*) const noexcept = 0;
    virtual result<T> begin(
        char const*&, char const*)
            const noexcept = 0;
    virtual result<T> increment(
        char const*&, char const*)
            const noexcept = 0;
};

//------------------------------------------------

template<class T>
class range<T>::
    iterator
{
public:
    using value_type = T;
    using reference = T const&;
    using pointer = void const*;
    using difference_type =
        std::ptrdiff_t;
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
        return rv_.value();
    }

    bool
    operator==(
        iterator other) const noexcept
    {
        // can't compare iterators
        // from different containers!
        BOOST_ASSERT(r_ == other.r_);

        return p_ == other.p_;
    }

    bool
    operator!=(
        iterator other) const noexcept
    {
        return !(*this == other);
    }

    iterator&
    operator++() noexcept
    {
        BOOST_ASSERT(
            p_ != nullptr);
        auto const end =
            r_->s_.data() +
            r_->s_.size();
        rv_ = r_->get().increment(p_, end);
        if(rv_.has_error())
        {
            BOOST_ASSERT(
                rv_.error() ==
                    error::end);
            p_ = nullptr;
        }
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

private:
    friend class range<T>;

    range<T> const* r_ = nullptr;
    char const* p_ = nullptr;
    result<T> rv_;

    iterator(
        range<T> const& r) noexcept
        : r_(&r)
        , p_(r.s_.data())
    {
        auto const end =
            r_->s_.data() +
            r_->s_.size();
        rv_ = r_->get().begin(p_, end);
        if(rv_.has_error())
        {
            BOOST_ASSERT(
                rv_.error() ==
                    error::end);
            p_ = nullptr;
        }
    }

    constexpr
    iterator(
        range<T> const& r,
        int) noexcept
        : p_(nullptr)
        , r_(&r)
    {
    }
};

//------------------------------------------------

template<class T>
auto
range<T>::
get() const noexcept ->
    any_rule const&
{
    return *reinterpret_cast<
        any_rule const*>(buf_);
}

template<class T>
range<T>::
~range()
{
    get().~any_rule();
}

template<class T>
range<T>::
range(
    range const& other) noexcept
    : s_(other.s_)
    , n_(other.n_)
{
    other.get().copy(buf_);
}

template<class T>
auto
range<T>::
begin() const noexcept ->
    iterator
{
    return { *this };
}

template<class T>
auto
range<T>::
end() const noexcept ->
    iterator
{
    return { *this, 0 };
}

template<class T>
template<class R>
range<T>::
range(
    string_view s,
    std::size_t n,
    R const& r,
    range_fn<R, T> begin,
    range_fn<R, T> increment) noexcept
    : s_(s)
    , n_(n)
{
    static_assert(
        is_rule<R>::value,
        "Rule requirements not met");

    static_assert(
        sizeof(R) <= BufferSize,
        "sizeof(R) is unsupported");

    struct impl : any_rule
    {
        R const r_;
        range_fn<R, T> begin_;
        range_fn<R, T> increment_;

        impl(
            R const& r,
            range_fn<R, T> begin,
            range_fn<R, T> increment) noexcept
            : r_(r)
            , begin_(begin)
            , increment_(increment)
        {
        }

        void
        copy(void* dest)
            const noexcept override
        {
            ::new(dest) impl(*this);
        }

        result<T>
        begin(
            char const*& it,
            char const* end)
                const noexcept override
        {
            return (r_.*begin_)(it, end);
        }

        result<T>
        increment(
            char const*& it,
            char const* end)
                const noexcept override
        {
            return (r_.*increment_)(it, end);
        }
    };

    ::new(buf_) impl(
        r, begin, increment);
}

//------------------------------------------------

template<
    class R,
    class T>
auto
parse_range(
    char const*& it,
    char const* end,
    R const& r,
    range_fn<typename std::remove_cv<R>::type, T> begin,
    range_fn<typename std::remove_cv<R>::type, T> increment,
    std::size_t N,
    std::size_t M) ->
        result<range<T>>
{
    std::size_t n = 0;
    auto const it0 = it;
    auto rv = (r.*begin)(it, end);
    if(rv.has_error())
    {
        if(rv.error() != error::end)
            return rv.error();
        if(n < N)
        {
            // too few
            return error::syntax;
        }

        // good
        return range<T>(
            string_view(it0, it - it0),
                n, r, begin, increment);
    }

    for(;;)
    {
        ++n;
        rv = (r.*increment)(it, end);
        if(rv.has_error())
        {
            if(rv.error() != error::end)
                return rv.error();
            break;
        }
        if(n > M)
        {
            // too many
            return error::syntax;
        }
    }

    // good
    return range<T>(
        string_view(it0, it - it0),
            n, r, begin, increment);
}

} // grammar
} // urls
} // boost

#endif
