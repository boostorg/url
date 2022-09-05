//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_BASE_IPP
#define BOOST_URL_IMPL_SEGMENTS_BASE_IPP

#include <boost/url/segments_base.hpp>

namespace boost {
namespace urls {

string_view
segments_base::
iterator::
dereference() const
{
    if(! valid_)
    {
        // VFALCO This could be better,
        // we should never shrink size() for
        // a recycled std::string, because
        // otherwise when we resize it larger
        // we will again have to value-init (?)
        // the new chars.
        s_.acquire();
        (*it_.dereference()).assign_to(*s_);
        valid_ = true;
    }
    return *s_;
}

// begin
segments_base::
iterator::
iterator(
    detail::path_ref const& ref) noexcept
    : it_(ref)
{
}

// end
segments_base::
iterator::
iterator(
    detail::path_ref const& ref,
    int) noexcept
    : it_(ref, 0)
{
}

//------------------------------------------------

pct_string_view
segments_base::
buffer() const noexcept
{
    return ref_.string();
}

bool
segments_base::
is_absolute() const noexcept
{
    return ref_.string().starts_with('/');
}

bool
segments_base::
empty() const noexcept
{
    return ref_.nseg() == 0;
}

std::size_t
segments_base::
size() const noexcept
{
    return ref_.nseg();
}

std::string
segments_base::
front() const noexcept
{
    BOOST_ASSERT(! empty());
    return *begin();
}

std::string
segments_base::
back() const noexcept
{
    BOOST_ASSERT(! empty());
    return *--end();
}

auto
segments_base::
begin() const noexcept ->
    iterator
{
    return iterator(ref_);
}

auto
segments_base::
end() const noexcept ->
    iterator
{
    return iterator(ref_, 0);
}
} // urls
} // boost

#endif
