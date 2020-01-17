//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_SEGMENTS_HPP
#define BOOST_URL_SEGMENTS_HPP

#include <boost/url/config.hpp>
#include <boost/url/detail/view_base.hpp>
#include <boost/url/detail/char_type.hpp>

namespace boost {
namespace url {

class segments : protected detail::view_base
{
    friend class view;

    segments(detail::view_base const& v) noexcept
    {
        static_cast<
            detail::view_base&
                >(*this) = v;
    }

public:
    class const_iterator
    {
        friend segments;

        segments const* path_ = nullptr;
        unsigned i_ = 0;

        const_iterator(
            segments const& p,
            unsigned i)
            : path_(&p)
            , i_(i)
        {
        }

    public:
        using value_type = string_view;

        const_iterator() = default;

        value_type
        operator*() const noexcept
        {
            return path_->get(
                detail::id_path + i_);
        }

        bool
        operator==(
            const_iterator other) const noexcept
        {
            return
                path_ == other.path_ &&
                i_ == other.i_;
        }

        bool
        operator!=(
            const_iterator other) const noexcept
        {
            return !(*this == other);
        }

        const_iterator&
        operator++() noexcept
        {
            ++i_;
            return *this;
        }

        const_iterator
        operator++(int) noexcept
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
    };

    using iterator = const_iterator;

    std::size_t
    size() const noexcept
    {
        return nseg_;
    }

    const_iterator
    begin() const noexcept
    {
        return const_iterator(*this, 0);
    }

    const_iterator
    end() const noexcept
    {
        return const_iterator(
            *this, nseg_);
    }
};

} // url
} // boost

#endif
