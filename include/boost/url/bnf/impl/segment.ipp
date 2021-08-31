//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_IMPL_SEGMENT_IPP
#define BOOST_URL_BNF_IMPL_SEGMENT_IPP

#include <boost/url/bnf/segment.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/algorithm.hpp>
#include <boost/url/bnf/char_sets.hpp>
#include <boost/url/bnf/pct_encoded.hpp>

namespace boost {
namespace urls {
namespace bnf {

namespace detail {

class pchar_nc
{
public:
    using value_type = void;

    value_type
    value() const noexcept
    {
        return;
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec)
    {
        auto it = start;
        if(it == end)
        {
            ec = error::need_more;
            return start;
        }
        if(is_unreserved(*it))
        {
            ++it;
            return it;
        }
        if(is_sub_delims(*it))
        {
            ++it;
            return it;
        }
        if(*it == '@')
        {
            ++it;
            return it;
        }
        it = consume<pct_encoded>(
            it, end, ec);
        return it;
    }
};

} // detail

char const*
segment_nz_nc::
parse(
    char const* const start,
    char const* const end,
    error_code& ec)
{
    return consume<one_or_more<
        detail::pchar_nc>>(
            start, end, ec);
}

} // bnf
} // urls
} // boost

#endif
