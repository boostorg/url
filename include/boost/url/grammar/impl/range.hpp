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

// base class for the type-erased rule pair
template<class T>
struct range<T>::
    any_rule
{
    virtual ~any_rule() = default;
    virtual void destroy(
        ) const noexcept = 0;
    virtual any_rule const* copy(
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
        rv_ = r_->pr_->increment(p_, end);
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
        rv_ = r_->pr_->begin(p_, end);
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
range<T>::
~range()
{
    if(pr_)
    {
        if(pr_ == reinterpret_cast<
                any_rule*>(buf_))
            pr_->~any_rule();
        else
            pr_->destroy();
    }
}

template<class T>
range<T>::
range(
    range const& other) noexcept
    : s_(other.s_)
    , n_(other.n_)
{
    if(other.pr_)
        pr_ = other.pr_->copy(buf_);
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
    R const& increment)
    : s_(s)
    , n_(n)
{
    struct impl : any_rule
    {
        R const increment_;

        explicit
        impl(
            R const& increment) noexcept
            : increment_(increment)
        {
        }

        void
        destroy() const noexcept override
        {
            delete this;
        }

        any_rule const*
        copy(void* dest)
            const noexcept override
        {
            static constexpr auto small =
                sizeof(*this) <= BufferSize;
            if(small)
                return ::new(dest) impl(*this);
            return new impl(*this);
        }

        result<T>
        begin(
            char const*& it,
            char const* end)
                const noexcept override
        {
            return (parse)(
                it, end, increment_);
        }

        result<T>
        increment(
            char const*& it,
            char const* end)
                const noexcept override
        {
            return (parse)(
                it, end, increment_);
        }
    };

    pr_ = ::new(reinterpret_cast<
        void*>(buf_)) impl(increment);
}

template<class T>
template<
    class R0, class R1>
range<T>::
range(
    string_view s,
    std::size_t n,
    R0 const& begin,
    R1 const& increment)
    : s_(s)
    , n_(n)
{
    struct impl : any_rule
    {
        R0 const begin_;
        R1 const increment_;

        impl(
            R0 const& begin,
            R1 const& increment) noexcept
            : begin_(begin)
            , increment_(increment)
        {
        }

        void
        destroy() const noexcept override
        {
            delete this;
        }

        any_rule const*
        copy(void* dest)
            const noexcept override
        {
            static constexpr auto small =
                sizeof(*this) <= BufferSize;
            if(small)
                return ::new(dest) impl(*this);
            return new impl(*this);
        }

        result<T>
        begin(
            char const*& it,
            char const* end)
                const noexcept override
        {
            return (parse)(
                it, end, begin_);
        }

        result<T>
        increment(
            char const*& it,
            char const* end)
                const noexcept override
        {
            return (parse)(
                it, end, increment_);
        }
    };

    pr_ = ::new(reinterpret_cast<
        void*>(buf_)) impl(begin, increment);
}

//------------------------------------------------

template<class R>
auto
parse_range(
    char const*& it,
    char const* end,
    R const& increment,
    std::size_t N,
    std::size_t M) ->
        result<range<typename
            R::value_type>>
{
    // If you get a compile error here it
    // means that your rule does not meet
    // the type requirements. Please check
    // the documentation.
    static_assert(
        is_rule<R>::value,
        "Rule requirements not met");

    using T = typename R::value_type;

    std::size_t n = 0;
    auto const it0 = it;
    auto rv = (parse)(it, end, increment);
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
                n, increment);
    }

    for(;;)
    {
        ++n;
        rv = (parse)(it, end, increment);
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
            n, increment);
}

template<class R0, class R1>
auto
parse_range(
    char const*& it,
    char const* end,
    R0 const& begin,
    R1 const& increment,
    std::size_t N,
    std::size_t M) ->
        result<range<typename
            R0::value_type>>
{
    // If you get a compile error here it
    // means that your rule does not meet
    // the type requirements. Please check
    // the documentation.
    static_assert(
        is_rule<R0>::value,
        "Rule requirements not met");
    static_assert(
        is_rule<R1>::value,
        "Rule requirements not met");

    // If you get a compile error here it
    // means that your rules do not have
    // the exact same value_type. Please
    // check the documentation.
    static_assert(
        std::is_same<
            typename R0::value_type,
            typename R1::value_type>::value,
        "Rule requirements not met");

    using T = typename R0::value_type;

    std::size_t n = 0;
    auto const it0 = it;
    auto rv = (parse)(it, end, begin);
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
                n, begin, increment);
    }

    for(;;)
    {
        ++n;
        rv = (parse)(it, end, increment);
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
            n, begin, increment);
}

} // grammar
} // urls
} // boost

#endif
