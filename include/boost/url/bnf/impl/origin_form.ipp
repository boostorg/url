//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_ORIGIN_FORM_IPP
#define BOOST_URL_BNF_IMPL_ORIGIN_FORM_IPP

#include <boost/url/bnf/origin_form.hpp>

namespace boost {
namespace urls {
namespace bnf {

char const*
origin_form::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    return start;
}

char const*
absolute_uri::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    return start;
}

} // bnf
} // urls
} // boost

#endif
