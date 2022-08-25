//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAM_HPP
#define BOOST_URL_IMPL_PARAM_HPP

namespace boost {
namespace urls {

inline
param_decode_view::
operator param_view() const noexcept
{
    if(has_value)
        return { key.encoded(), value.encoded() };
    return { key.encoded() };
}

} // urls
} // boost

#endif
