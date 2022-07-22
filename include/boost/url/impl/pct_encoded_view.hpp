//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODED_VIEW_HPP
#define BOOST_URL_IMPL_PCT_ENCODED_VIEW_HPP

#include <boost/url/grammar/type_traits.hpp>

namespace boost {
namespace urls {

class pct_encoded_view::iterator
{
    char const* begin_{nullptr};
    char const* pos_{nullptr};
    bool plus_to_space_{true};

    friend pct_encoded_view;

    iterator(
        char const* str,
        bool plus_to_space) noexcept
        : begin_(str)
        , pos_(str)
        , plus_to_space_(plus_to_space)
    {}

    // end ctor
    iterator(
        char const* str,
        size_type n,
        bool plus_to_space) noexcept
        : begin_(str)
        , pos_(str + n)
        , plus_to_space_(plus_to_space)
    {}

public:
    using value_type = char;
    using reference = char;
    using pointer = void const*;
    using const_reference = char;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    iterator() = default;

    iterator(iterator const&) = default;

    iterator&
    operator=(iterator const&) = default;

    BOOST_URL_DECL
    reference
    operator*() const noexcept;

    iterator&
    operator++() noexcept
    {
        BOOST_ASSERT(pos_ != nullptr);
        if (*pos_ != '%')
            ++pos_;
        else
            pos_ += 3;
        return *this;
    }

    iterator&
    operator--() noexcept
    {
        BOOST_ASSERT(pos_ != begin_);
        if (pos_ - begin_ < 3 ||
                pos_[-3] != '%')
            --pos_;
        else
            pos_ -= 3;
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    char const*
    base()
    {
        return pos_;
    }

    bool
    operator==(
        iterator const& other) const noexcept
    {
        return pos_ == other.pos_;
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return !(*this == other);
    }
};

inline
auto
pct_encoded_view::
begin() const noexcept ->
    const_iterator
{
    return {p_, plus_to_space_};
}

inline
auto
pct_encoded_view::
end() const noexcept ->
    const_iterator
{
    return {p_, n_, plus_to_space_};
}

inline
auto
pct_encoded_view::
front() const noexcept ->
    const_reference
{
    BOOST_ASSERT( !empty() );
    return *begin();
}

inline
auto
pct_encoded_view::
back() const noexcept ->
    const_reference
{
    BOOST_ASSERT( !empty() );
    return *--end();
}

template <class MutableString>
MutableString&
pct_encoded_view::
assign_to(
    MutableString& s) const
{
    // If you get a compiler error here, it means that
    // your String type is missing the necessary append
    // or assign member functions, or that your
    // value_type is not char!
    //
    // This function should only be used
    // if the container `MutableString` has the
    // member function `assign(iterator, iterator)`.
    BOOST_STATIC_ASSERT(
        grammar::is_mutable_string<
            MutableString, iterator>::value);
    s.assign(begin(), end());
    return s;
}

template <class MutableString>
MutableString&
pct_encoded_view::
append_to(
    MutableString& s) const
{
    // If you get a compiler error here, it means that
    // your String type is missing the necessary append
    // or assign member functions, or that your
    // value_type is not char!
    //
    // This function should only be used
    // if the container `MutableString` has the
    // member function `append(iterator, iterator)`.
    BOOST_STATIC_ASSERT(
        grammar::is_mutable_string<
            MutableString, iterator>::value);
    s.append(begin(), end());
    return s;
}

} // urls
} // boost

#endif
