//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_LITERAL_HPP
#define BOOST_URL_BNF_LITERAL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>

namespace boost {
namespace urls {
namespace bnf {

/** BNF for literal string
*/
template<char...Cn>
class literal
{
public:
    using value_type = string_view;

    string_view const&
    operator*() const noexcept
    {
        return s_;
    }

    string_view const*
    operator->() const noexcept
    {
        return s_;
    }

    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec) noexcept;

    template<char...Cn>
    friend
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec,
        literal<Cn...> const&) noexcept;

private:
    string_view s_;
};

} // bnf
} // urls
} // boost

#include <boost/url/bnf/impl/literal.hpp>

#endif
