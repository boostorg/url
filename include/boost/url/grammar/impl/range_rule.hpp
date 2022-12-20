//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
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
        return *rv_;
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
        rv_ = r_->r_->parse(
            p_, end, false);
        if( !rv_ )
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
        rv_ = r_->r_->parse(
            p_, end, true);
        if( !rv_ )
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
range<T>::
range(
    string_view s,
    std::size_t n,
    detail::any_iter_rule<
        T> const* r) noexcept
    : s_(s)
    , n_(n)
    , r_(r->acquire())
{
}

//------------------------------------------------

template<class T>
range<T>::
~range()
{
    r_->release();
}

template<class T>
range<T>::
range() noexcept
    : r_([]
        {
            static
            const
            detail::any_iter_rule<T> r(2);

            return &r;
        }())
{
}

template<class T>
range<T>::
range(
    range const& other) noexcept
    : s_(other.s_)
    , n_(other.n_)
    , r_(other.r_->acquire())
{
}

template<class T>
auto
range<T>::
operator=(
    range const& other) noexcept ->
        range&
{
    other.r_->acquire();
    r_->release();

    s_ = other.s_;
    n_ = other.n_;
    r_ = other.r_;
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
    auto rv = r_->parse(it, end, true);
    if( !rv )
    {
        if(rv.error() != error::end_of_range)
        {
            // rewind unless error::end_of_range
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
                n, r_);
    }
    for(;;)
    {
        ++n;
        it1 = it;
        rv = r_->parse(it, end, false);
        if( !rv )
        {
            if(rv.error() != error::end_of_range)
            {
                // rewind unless error::end_of_range
                it = it1;
            }
            break;
        }
        if(n >= M_)
        {
            // too many
            BOOST_URL_RETURN_EC(
                error::mismatch);
        }
    }
    if(n < N_)
    {
        // too few
        BOOST_URL_RETURN_EC(
            error::mismatch);
    }
    // good
    return range<T>(string_view(
        it0, it - it0), n, r_);
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
    auto rv = r_->parse(it, end, true);
    if( !rv )
    {
        if(rv.error() != error::end_of_range)
        {
            // rewind unless error::end_of_range
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
                n, r_);
    }
    for(;;)
    {
        ++n;
        it1 = it;
        rv = r_->parse(it, end, false);
        if( !rv )
        {
            if(rv.error() != error::end_of_range)
            {
                // rewind unless error::end_of_range
                it = it1;
            }
            break;
        }
        if(n >= M_)
        {
            // too many
            BOOST_URL_RETURN_EC(
                error::mismatch);
        }
    }
    if(n < N_)
    {
        // too few
        BOOST_URL_RETURN_EC(
            error::mismatch);
    }
    // good
    return range<T>(string_view(
        it0, it - it0),n, r_);
}

} // grammar
} // urls
} // boost

#endif
