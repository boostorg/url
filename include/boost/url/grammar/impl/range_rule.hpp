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
#include <boost/url/detail/empty_value.hpp>
#include <boost/url/grammar/error.hpp>
#include <boost/assert.hpp>
#include <iterator>
#include <new>

namespace boost {
namespace urls {
namespace grammar {

//------------------------------------------------
//
// any_rule
//
//------------------------------------------------

// base class for the type-erased rule pair
template<class T>
struct range<T>::
    any_rule
{
    virtual
    ~any_rule() = default;

    virtual
    void
    destroy() const noexcept = 0;

    virtual
    any_rule const*
    move(void*) const noexcept = 0;

    virtual
    any_rule const*
    copy(void*) const noexcept = 0;

    virtual
    result<T>
    first(
        char const*&,
        char const*) const noexcept = 0;

    virtual
    result<T> 
    next(
        char const*&,
        char const*) const noexcept = 0;
};

//------------------------------------------------

template<class T>
template<class R>
struct range<T>::impl1
    : any_rule
    , private urls::detail::empty_value<R>
{
    explicit
    impl1(R const& next) noexcept
        : urls::detail::empty_value<R>(
            urls::detail::empty_init,
            next)
    {
    }

private:
    void
    destroy() const noexcept override
    {
        static constexpr auto small =
            sizeof(*this) <= BufferSize;
        if(small)
            this->~impl1();
        else
            delete this;
    }

    any_rule const*
    move(void* dest)
        const noexcept override
    {
        static constexpr auto small =
            sizeof(*this) <= BufferSize;
        if(small)
            return ::new(dest) impl1(*this);
        return this;
    }

    any_rule const*
    copy(void* dest)
        const noexcept override
    {
        static constexpr auto small =
            sizeof(*this) <= BufferSize;
        if(small)
            return ::new(dest) impl1(*this);
        return new impl1(*this);
    }

    result<T>
    first(
        char const*& it,
        char const* end)
            const noexcept override
    {
        return grammar::parse(
            it, end, this->get());
    }

    result<T>
    next(
        char const*& it,
        char const* end)
            const noexcept override
    {
        return grammar::parse(
            it, end, this->get());
    }
};

//------------------------------------------------

template<class T>
template<class R0, class R1>
struct range<T>::impl2
    : any_rule
    , private urls::detail::empty_value<R0, 0>
    , private urls::detail::empty_value<R1, 1>
{
    impl2(
        R0 const& first,
        R1 const& next) noexcept
        : urls::detail::empty_value<R0,0>(
            urls::detail::empty_init, first)
        , urls::detail::empty_value<R1,1>(
            urls::detail::empty_init, next)
    {
    }

private:
    void
    destroy() const noexcept override
    {
        static constexpr auto small =
            sizeof(*this) <= BufferSize;
        if(small)
            this->~impl2();
        else
            delete this;
    }

    any_rule const*
    move(void* dest)
        const noexcept override
    {
        static constexpr auto small =
            sizeof(*this) <= BufferSize;
        if(small)
            return ::new(dest) impl2(*this);
        return this;
    }

    any_rule const*
    copy(void* dest)
        const noexcept override
    {
        static constexpr auto small =
            sizeof(*this) <= BufferSize;
        if(small)
            return ::new(dest) impl2(*this);
        return new impl2(*this);
    }

    result<T>
    first(
        char const*& it,
        char const* end)
            const noexcept override
    {
        return grammar::parse(it, end,
            urls::detail::empty_value<
                R0,0>::get());
    }

    result<T>
    next(
        char const*& it,
        char const* end)
            const noexcept override
    {
        return grammar::parse(it, end,
            urls::detail::empty_value<
                R1,1>::get());
    }
};

//------------------------------------------------
//
// iterator
//
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
        rv_ = r_->pr_->next(p_, end);
        if(rv_.has_error())
            p_ = nullptr;
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
        rv_ = r_->pr_->first(p_, end);
        if(rv_.has_error())
            p_ = nullptr;
    }

    constexpr
    iterator(
        range<T> const& r,
        int) noexcept
        : r_(&r)
        , p_(nullptr)
    {
    }
};

//------------------------------------------------

template<class T>
template<class R>
range<T>::
range(
    string_view s,
    std::size_t n,
    R const& next)
    : s_(s)
    , n_(n)
{
    using U = impl1<R>;

    static constexpr auto small =
        sizeof(U) <= BufferSize;
    if(small)
    {
        pr_ = ::new(reinterpret_cast<
            void*>(buf_)) U(next);
    }
    else
    {
        pr_ = new U(next);
    }
}

//------------------------------------------------

template<class T>
template<
    class R0, class R1>
range<T>::
range(
    string_view s,
    std::size_t n,
    R0 const& first,
    R1 const& next)
    : s_(s)
    , n_(n)
{
    using U = impl2<R0, R1>;

    static constexpr auto small =
        sizeof(U) <= BufferSize;
    if(small)
    {
        pr_ = ::new(reinterpret_cast<
            void*>(buf_)) U(first, next);
    }
    else
    {
        pr_ = new U(first, next);
    }
}

//------------------------------------------------

template<class T>
range<T>::
~range()
{
    if(pr_)
        pr_->destroy();
}

template<class T>
range<T>::
range(
    range&& other) noexcept
    : s_(other.s_)
    , n_(other.n_)
{
    if(other.pr_)
    {
        pr_ = other.pr_->move(buf_);
        other.pr_ = nullptr;
    }
}

template<class T>
range<T>::
range(
    range const& other)
    : s_(other.s_)
    , n_(other.n_)
{
    if(other.pr_)
        pr_ = other.pr_->copy(buf_);
}

template<class T>
auto
range<T>::
operator=(
    range&& other) noexcept ->
        range&
{
    if(pr_)
        pr_->destroy();
    if(other.pr_)
    {
        s_ = other.s_;
        n_ = other.n_;
        pr_ = other.pr_->move(buf_);
        other.s_ = {};
        other.n_ = 0;
        other.pr_ = nullptr;
    }
    else
    {
        s_ = {};
        n_ = 0;
        pr_ = nullptr;
    }
    
    return *this;
}

template<class T>
auto
range<T>::
operator=(
    range const& other) ->
        range&
{
    range tmp(other);
    if(pr_)
        pr_->destroy();
    s_ = tmp.s_;
    n_ = tmp.n_;
    pr_ = tmp.pr_;
    tmp.pr_ = nullptr;
    return *this;
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

//------------------------------------------------

template<class R>
auto
range_rule_t<R>::
parse(
    char const*& it,
    char const* end) const ->
        result<value_type>
{
    using T = typename R::value_type;

    std::size_t n = 0;
    auto const it0 = it;
    auto it1 = it;
    auto rv = (grammar::parse)(
        it, end, next_);
    if(rv.has_error())
    {
        if(rv.error() != error::end)
        {
            // rewind unless error::end
            it = it1;
        }
        if(n < N_)
        {
            // too few
            return error::syntax;
        }

        // good
        return range<T>(
            string_view(it0, it - it0),
                n, next_);
    }

    for(;;)
    {
        ++n;
        it1 = it;
        rv = (grammar::parse)(
            it, end, next_);
        if(rv.has_error())
        {
            if(rv.error() != error::end)
            {
                // rewind unless error::end
                it = it1;
            }
            break;
        }
        if(n > M_)
        {
            // too many
            return error::syntax;
        }
    }

    // good
    return range<T>(
        string_view(it0, it - it0),
            n, next_);
}

//------------------------------------------------

template<class R0, class R1>
auto
range_rule_t<R0, R1>::
parse(
    char const*& it,
    char const* end) const ->
        result<range<typename
            R0::value_type>>
{
    using T = typename R0::value_type;

    std::size_t n = 0;
    auto const it0 = it;
    auto it1 = it;
    auto rv = (grammar::parse)(
        it, end, first_);
    if(rv.has_error())
    {
        if(rv.error() != error::end)
        {
            // rewind unless error::end
            it = it1;
        }
        if(n < N_)
        {
            // too few
            return error::syntax;
        }

        // good
        return range<T>(
            string_view(it0, it - it0),
                n, first_, next_);
    }

    for(;;)
    {
        ++n;
        it1 = it;
        rv = (grammar::parse)(
            it, end, next_);
        if(rv.has_error())
        {
            if(rv.error() != error::end)
            {
                // rewind unless error::end
                it = it1;
            }
            break;
        }
        if(n > M_)
        {
            // too many
            return error::syntax;
        }
    }

    // good
    return range<T>(
        string_view(it0, it - it0),
            n, first_, next_);
}

} // grammar
} // urls
} // boost

#endif
