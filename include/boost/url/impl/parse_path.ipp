//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#ifndef BOOST_URL_IMPL_PARSE_PATH_IPP
#define BOOST_URL_IMPL_PARSE_PATH_IPP

#include <boost/url/parse_path.hpp>
#include <boost/url/error.hpp>
#include <boost/url/detail/path.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/detail/path_rules.hpp>

namespace boost {
namespace urls {

result<segments_encoded_view>
parse_path(string_view s) noexcept
{
    if(s.empty())
        return segments_encoded_view(
            detail::path_ref(s, 0, 0));
    if(s[0] == '/')
    {
        auto rv = grammar::parse(
            s, detail::path_abempty_rule);
        if(! rv)
            return rv.error();

        // VFALCO We are needlessly recalculating
        // the decoded size here.
        return segments_encoded_view(
            detail::path_ref(
                rv->string(),
                detail::decode_bytes_unchecked(
                    rv->string()),
                detail::path_segments(
                    rv->string(),
                    rv->size())));
    }
    {
        auto rv = grammar::parse(
            s, detail::path_rootless_rule);
        if(! rv)
            return rv.error();
        return segments_encoded_view(
            detail::path_ref(
                rv->string(),
                detail::decode_bytes_unchecked(
                    rv->string()),
                detail::path_segments(
                    rv->string(),
                    rv->size())));
    }
}

} // urls
} // boost

#endif
