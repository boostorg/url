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
#include <boost/url/grammar/recycled.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <exception>
#include <iterator>
#include <new>

#include <stddef.h> // ::max_align_t

namespace boost {
namespace urls {
namespace grammar {

// VFALCO This could be reused for
// other things that need to type-erase

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
    move(void* dest) noexcept
    {
        ::new(dest) any_rule(
            std::move(*this));
    }

    virtual
    result<T>
    first(
        char const*&,
        char const*) const noexcept
    {
        // pure virtual called
        return error_code{};
    }

    virtual
    result<T> 
    next(
        char const*&,
        char const*) const noexcept
    {
        // pure virtual called
        return error_code{};
    }
};

//------------------------------------------------

// small
template<class T>
template<class R, bool Small>
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
    move(void* dest) noexcept override
    {
        ::new(dest) impl1(
            std::move(*this));
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

// big
template<class T>
template<class R>
struct range<T>::impl1<R, false>
    : any_rule
{
    struct impl
    {
        R r;
    };

    recycled_ptr<
        aligned_storage<impl>> p_;

    impl const&
    get() const noexcept
    {
        return *reinterpret_cast<
            impl const*>(p_->addr());
    }

    explicit
    impl1(R const& next) noexcept
    {
        ::new(p_->addr()) impl{next};
    }

private:
    ~impl1()
    {
        get().~impl();
    }

    impl1(impl1&&) = default;

    void
    move(void* dest) noexcept override
    {
        ::new(dest) impl1(
            std::move(*this));
    }

    result<T>
    first(
        char const*& it,
        char const* end)
            const noexcept override
    {
        return grammar::parse(
            it, end, this->get().r);
    }

    result<T>
    next(
        char const*& it,
        char const* end)
            const noexcept override
    {
        return grammar::parse(
            it, end, this->get().r);
    }
};

//------------------------------------------------

// small
template<class T>
template<
    class R0, class R1, bool Small>
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
    move(void* dest) noexcept override
    {
        ::new(dest) impl2(
            std::move(*this));
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

// big
template<class T>
template<
    class R0, class R1>
struct range<T>::impl2<R0, R1, false>
    : any_rule
{
    struct impl
    {
        R0 first;
        R1 next;
    };

    recycled_ptr<
        aligned_storage<impl>> p_;

    impl const&
    get() const noexcept
    {
        return *reinterpret_cast<
            impl const*>(p_->addr());
    }

    impl2(
        R0 const& first,
        R1 const& next) noexcept
    {
        ::new(p_->addr()) impl{
            first, next};
    }

private:
    ~impl2()
    {
        get().~impl();
    }

    impl2(impl2&&) = default;

    void
    move(void* dest) noexcept override
    {
        ::new(dest) impl2(
            std::move(*this));
    }

    result<T>
    first(
        char const*& it,
        char const* end)
            const noexcept override
    {
        return grammar::parse(
            it, end, get().first);
    }

    result<T>
    next(
        char const*& it,
        char const* end)
            const noexcept override
    {
        return grammar::parse(
            it, end, get().next);
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
        rv_ = r_->get().next(p_, end);
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
        rv_ = r_->get().first(p_, end);
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
    BOOST_STATIC_ASSERT(
        sizeof(impl1<R, false>) <=
            BufferSize);

    ::new(&get()) impl1<R,
        sizeof(impl1<R, true>) <=
            BufferSize>(next);
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
    BOOST_STATIC_ASSERT(
        sizeof(impl2<R0, R1, false>) <=
            BufferSize);

    ::new(&get()) impl2<R0, R1,
        sizeof(impl2<R0, R1, true>
            ) <= BufferSize>(
                first, next);
}

//------------------------------------------------

template<class T>
range<T>::
~range()
{
    get().~any_rule();
}

template<class T>
range<T>::
range() noexcept
{
    ::new(&get()) any_rule{};
}

template<class T>
range<T>::
range(
    range&& other) noexcept
    : s_(other.s_)
    , n_(other.n_)
{
    other.get().move(&get());
}

template<class T>
auto
range<T>::
operator=(
    range&& other) noexcept ->
        range&
{
    s_ = other.s_;
    n_ = other.n_;
    other.s_ = {};
    other.n_ = 0;    
    get().~any_rule();
    other.get().move(&get());
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
        if(rv.error() != error::range_end)
        {
            // rewind unless error::range_end
            it = it1;
        }
        if(n < N_)
        {
            // too few
            BOOST_URL_RETURN_EC(
                error::mismatch);
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
            if(rv.error() != error::range_end)
            {
                // rewind unless error::range_end
                it = it1;
            }
            break;
        }
        if(n > M_)
        {
            // too many
            BOOST_URL_RETURN_EC(
                error::mismatch);
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
        if(rv.error() != error::range_end)
        {
            // rewind unless error::range_end
            it = it1;
        }
        if(n < N_)
        {
            // too few
            BOOST_URL_RETURN_EC(
                error::mismatch);
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
            if(rv.error() != error::range_end)
            {
                // rewind unless error::range_end
                it = it1;
            }
            break;
        }
        if(n > M_)
        {
            // too many
            BOOST_URL_RETURN_EC(
                error::mismatch);
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
