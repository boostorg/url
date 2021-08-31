//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_RANGE_HPP
#define BOOST_URL_BNF_RANGE_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/bnf/arrow_proxy.hpp>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

namespace boost {
namespace urls {
namespace bnf {

template<class List>
class range
{
    string_view s_;

    char const*
    pend() const noexcept
    {
        return s_.data() + s_.size();
    }

public:
    using list_type = List;
    using value_type =
        typename List::value_type;

    class iterator;

    constexpr range(
        range const&) = default;
    constexpr range& operator=(
        range const&) = default;

    /** Default constructor

        Iteration of default constructed ranges
        is undefined.
    */
    constexpr range() = default;

    /** Constructor
    */
    explicit
    range(
        string_view s)
        : s_(s)
    {
    }

    /** Return the entire string underlying the range
    */
    string_view
    str() const noexcept
    {
        return s_;
    }

    inline iterator begin(
        error_code& ec) const;

    inline iterator begin() const;

    inline iterator end() const;

    inline void validate(
        error_code& ec) const;

    inline void validate() const;

    inline bool is_valid() const;
};

//------------------------------------------------

template<class T>
class range<T>::iterator
{
    char const* next_ = nullptr;
    char const* end_ = nullptr;
    T impl_;

    friend class range;

    explicit
    iterator(string_view s)
        : end_(s.data() + s.size())
    {
        error_code ec;
        next_ = impl_.begin(
            s.data(), end_, ec);
        if(ec == error::end)
            next_ = nullptr;
        else if(ec.failed())
            urls::detail::throw_system_error(ec,
                BOOST_CURRENT_LOCATION);
    }

    iterator(
        string_view s,
        error_code& ec)
        : end_(s.data() + s.size())
    {
        next_ = impl_.begin(
            s.data(), end_, ec);
        if(ec == error::end)
        {
            next_ = nullptr;
            ec = {};
        }
    }

    explicit
    iterator(char const* end)
        : next_(nullptr)
        , end_(end)
    {
    }

public:
    using value_type =
        typename range::value_type;
    using pointer = value_type;
    using reference = value_type;
    using iterator_category =
        std::forward_iterator_tag;

    iterator() noexcept = default;

    bool
    operator==(
        iterator const& other) const
    {
        return
            next_ == other.next_ &&
            end_ == other.end_;
    }

    bool
    operator!=(
        iterator const& other) const
    {
        return !(*this == other);
    }

    value_type
    operator*() const noexcept
    {
        return impl_.value();
    }

    arrow_proxy<value_type>
    operator->() const noexcept
    {
        return arrow_proxy<
            value_type>{ impl_.value() };
    }

    void
    increment(
        error_code& ec)
    {
        next_ = impl_.increment(
            next_, end_, ec);
        if(ec == error::end)
        {
            next_ = nullptr;
            ec = {};
        }
    }

    iterator&
    operator++()
    {
        error_code ec;
        increment(ec);
        if(ec.failed())
            urls::detail::throw_system_error(ec,
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

template<class T>
auto
range<T>::
begin(error_code& ec) const ->
    iterator
{
    iterator it(s_, ec);
    if(! ec)
        return it;
    return iterator(pend());
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
    return iterator(pend());
}

//------------------------------------------------

template<class T>
void
range<T>::
validate(
    error_code& ec) const
{
    auto const end_ = end();
    auto it = begin(ec);
    while(! ec)
    {
        if(it == end_)
            return;
        it.increment(ec);
    }
}

template<class T>
void
range<T>::
validate() const
{
    error_code ec;
    validate(ec);
    if(ec.failed())
        urls::detail::throw_system_error(ec,
            BOOST_CURRENT_LOCATION);
}

template<class T>
bool
range<T>::
is_valid() const
{
    error_code ec;
    validate(ec);
    return ! ec.failed();
}

//------------------------------------------------

/** Return the valid prefix of s for the BNF T
*/
template<class T>
string_view
valid_prefix(
    string_view s)
{
    error_code ec;
    auto const end = &*s.end();
    auto pos = s.data();
    T impl;
    pos = impl.begin(pos, end, ec);
    // nothing valid
    if(ec.failed())
        return { s.data(), 0 };
    // valid empty list
    if(! pos)
        return { s.data(), 0 };
    for(;;)
    {
        auto next = impl.increment(
            pos, end, ec);
        if(ec.failed())
            break;
        if(! next)
        {
            pos = end;
            break;
        }
        pos = next;
    }
    return { s.data(), static_cast<
        std::size_t>(pos - s.data()) };
}

} // bnf
} // http_proto
} // boost

#endif
