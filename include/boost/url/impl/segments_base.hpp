//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_BASE_HPP
#define BOOST_URL_IMPL_SEGMENTS_BASE_HPP

#include <boost/url/grammar/recycled.hpp>
#include <boost/url/detail/segments_iter_impl.hpp>

namespace boost {
namespace urls {

class segments_base::iterator
{
    detail::segments_iter_impl it_;
    mutable grammar::recycled_ptr<
        std::string> s_ = nullptr;
    mutable bool valid_ = false;

    friend class segments_base;
    friend class segments_ref;

    iterator(detail::path_ref const&) noexcept;
    iterator(detail::path_ref const&, int) noexcept;
    iterator(detail::segments_iter_impl const& it) noexcept
        : it_(it)
    {
    }

    BOOST_URL_DECL
    string_view
    dereference() const;

public:
    using value_type = std::string;
    using reference = string_view;
    using pointer = string_view;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    iterator() = default;
    iterator(iterator const&) = default;
    iterator& operator=(
        iterator const&) = default;

    reference
    operator*() const
    {
        return dereference();
    }

    struct arrow_proxy
    {
        string_view s;

        string_view const*
        operator->()
        {
            return &s;
        }
    };

    arrow_proxy
    operator->() const
    {
        return arrow_proxy{
            dereference()};
    }

    iterator&
    operator++() noexcept
    {
        valid_ = false;
        it_.increment();
        return *this;
    }

    iterator&
    operator--() noexcept
    {
        valid_ = false;
        it_.decrement();
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

    bool
    operator==(
        iterator const& other) const noexcept
    {
        return it_.equal(other.it_);
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return ! it_.equal(other.it_);
    }
};

} // urls
} // boost

#endif
