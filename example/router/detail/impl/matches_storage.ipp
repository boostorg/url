//
// Copyright (c) 2023 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#include "../matches_storage.hpp"

namespace boost {
namespace urls {
namespace detail {

auto
matches_base::
at( size_type pos ) const
    -> const_reference
{
    if (pos < size_)
    {
        return matches()[pos];
    }
    boost::throw_exception(
        std::out_of_range(""));
}

auto
matches_base::
operator[]( size_type pos ) const
    -> const_reference
{
    BOOST_ASSERT(pos < size_);
    return matches()[pos];
}

auto
matches_base::
at( string_view id ) const
    -> const_reference
{
    for (std::size_t i = 0; i < size_; ++i)
    {
        if (ids()[i] == id)
            return matches()[i];
    }
    boost::throw_exception(
        std::out_of_range(""));
}

auto
matches_base::
operator[]( string_view id ) const
    -> const_reference
{
    return at(id);
}

auto
matches_base::
find( string_view id ) const
    -> const_iterator
{
    for (std::size_t i = 0; i < size_; ++i)
    {
        if (ids()[i] == id)
            return matches() + i;
    }
    return matches() + size_;
}

auto
matches_base::
begin() const
    -> const_iterator
{
    return &matches()[0];
}

auto
matches_base::
end() const
    -> const_iterator
{
    return &matches()[size_];
}

auto
matches_base::
empty() const noexcept
    -> bool
{
    return size_ == 0;
}

auto
matches_base::
size() const noexcept
    -> size_type
{
    return size_;
}

} // detail
} // urls
} // boost

