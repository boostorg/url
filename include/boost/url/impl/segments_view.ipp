//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_SEGMENTS_VIEW_IPP
#define BOOST_URL_IMPL_SEGMENTS_VIEW_IPP

#include <boost/url/segments_view.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/paths_rule.hpp>
#include <boost/assert.hpp>
#include <ostream>

namespace boost {
namespace urls {

//------------------------------------------------
//
// Iterators
//
//------------------------------------------------

auto
segments_view::
begin() const noexcept ->
    iterator
{
    return iterator(s_, n_, a_);
}

auto
segments_view::
end() const noexcept ->
    iterator
{
    return iterator(s_, n_, a_, 0);
}

//------------------------------------------------
//
// Friends
//
//------------------------------------------------

std::ostream&
operator<<(
    std::ostream& os,
    segments_view const& vw)
{
    auto it = vw.begin();
    auto const end = vw.end();
    if( it != end )
    {
        if( vw.is_absolute() )
            os << "/";
        os << *it;
        while( ++it != end )
            os << '/' << *it;
    }
    return os;
}

} // urls
} // boost

#endif
