//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_ORIGIN_FORM_IPP
#define BOOST_URL_RFC_IMPL_ORIGIN_FORM_IPP

#include <boost/url/rfc/origin_form.hpp>

namespace boost {
namespace urls {
namespace rfc {

char const*
origin_form::
parse(
    char const* const,
    char const* const,
    error_code&)
{
    return nullptr;
}

char const*
absolute_uri::
parse(
    char const* const,
    char const* const,
    error_code&)
{
    return nullptr;
}

} // rfc
} // urls
} // boost

#endif
