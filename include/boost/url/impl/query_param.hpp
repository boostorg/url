//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_QUERY_PARAM_HPP
#define BOOST_URL_IMPL_QUERY_PARAM_HPP

namespace boost {
namespace urls {

inline
query_param_view::operator query_param_encoded_view() const noexcept
{
    if(has_value)
        return { key.encoded(), value.encoded(), true };
    return { key.encoded(), string_view(), false };
}

} // urls
} // boost

#endif
