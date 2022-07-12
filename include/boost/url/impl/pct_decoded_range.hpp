//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_DECODED_RANGE_HPP
#define BOOST_URL_IMPL_PCT_DECODED_RANGE_HPP

namespace boost {
namespace urls {

class pct_decoded_range::const_iterator
{
    char const* begin_{nullptr};
    char const* pos_{nullptr};
    char const* end_{nullptr};
    bool plus_to_space_{true};

    friend pct_decoded_range;

    const_iterator(
        char const* str,
        size_type n,
        bool plus_to_space) noexcept
        : begin_(str)
        , pos_(str)
        , end_(str + n)
        , plus_to_space_(plus_to_space)
    {}

    // end ctor
    const_iterator(
        char const* str,
        size_type n,
        bool plus_to_space,
        int) noexcept
        : begin_(str)
        , pos_(str + n)
        , end_(str + n)
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

    const_iterator() noexcept = default;

    const_iterator(const_iterator const&) noexcept = default;

    const_iterator&
    operator=(const_iterator const&) noexcept = default;

    reference
    operator*() const noexcept
    {
        char c{'\0'};
        pct_decode_opts opt;
        opt.plus_to_space = plus_to_space_;
        pct_decode_unchecked(
            &c, &c+1, string_view(pos_, end_), opt);
        return c;
    }

    const_iterator&
    operator++() noexcept
    {
        BOOST_ASSERT(pos_ != nullptr);
        pos_ += *pos_ == '%' ? 3 : 1;
        return *this;
    }

    const_iterator&
    operator--() noexcept
    {
        BOOST_ASSERT(pos_ != begin_);
        if (pos_ - begin_ >= 3 &&
            *(pos_ - 3) == '%')
            pos_ -= 3;
        else
            --pos_;
        return *this;
    }

    const_iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    const_iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    bool
    operator==(
        const_iterator const& other) const noexcept
    {
        return pos_ == other.pos_;
    }

    bool
    operator!=(
        const_iterator const& other) const noexcept
    {
        return !(*this == other);
    }
};

template <class A>
std::basic_string<char, std::char_traits<char>, A>&
pct_decoded_range::append_to(std::basic_string<char, std::char_traits<char>, A> & str) const
{
    str.append(begin(), end());
    return str;
}

} // urls
} // boost

#endif
