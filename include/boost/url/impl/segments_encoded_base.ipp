//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_ENCODED_BASE_IPP
#define BOOST_URL_IMPL_SEGMENTS_ENCODED_BASE_IPP

#include <boost/url/segments_encoded_base.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace urls {

// begin
segments_encoded_base::
iterator::
iterator(
    detail::path_ref const& ref) noexcept
    : it_(ref)
{
}

// end
segments_encoded_base::
iterator::
iterator(
    detail::path_ref const& ref,
    int) noexcept
    : it_(ref, 0)
{
}

//------------------------------------------------

pct_string_view
segments_encoded_base::
buffer() const noexcept
{
    return ref_.string();
}

bool
segments_encoded_base::
is_absolute() const noexcept
{
    return ref_.string().starts_with('/');
}

bool
segments_encoded_base::
empty() const noexcept
{
    return ref_.nseg() == 0;
}

std::size_t
segments_encoded_base::
size() const noexcept
{
    return ref_.nseg();
}

pct_string_view
segments_encoded_base::
front() const noexcept
{
    BOOST_ASSERT(! empty());
    return *begin();
}

pct_string_view
segments_encoded_base::
back() const noexcept
{
    BOOST_ASSERT(! empty());
    return *--end();
}

auto
segments_encoded_base::
begin() const noexcept ->
    iterator
{
    return iterator(ref_);
}

auto
segments_encoded_base::
end() const noexcept ->
    iterator
{
    return iterator(ref_, 0);
}

} // urls
} // boost

#endif
