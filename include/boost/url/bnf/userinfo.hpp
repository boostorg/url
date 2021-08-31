//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_BNF_USERINFO_HPP
#define BOOST_URL_BNF_USERINFO_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/char_type.hpp>

namespace boost {
namespace urls {
namespace bnf {

/** BNF for userinfo

    @par BNF
    @code
    userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
class userinfo
{
public:
    class value_type
    {
        friend class userinfo;
        string_view userinfo_;
        string_view user_;
        string_view password_;

    public:
        string_view
        encoded_userinfo() const noexcept
        {
            return userinfo_;
        }

        string_view
        encoded_user() const noexcept
        {
            return user_;
        }

        string_view
        encoded_password() const noexcept
        {
            return password_;
        }

        template<
            class Allocator =
                std::allocator<char>>
        string_type<Allocator>
        user(
            Allocator const& a = {}) const
        {
            return detail::decode(
                user_, a);
        }

        template<
            class Allocator =
                std::allocator<char>>
        string_type<Allocator>
        password(
            Allocator const& a = {}) const
        {
            return detail::decode(
                password_, a);
        }
    };

    value_type
    value() const noexcept
    {
        return v_;
    }

    BOOST_URL_DECL
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec);

private:
    struct uchar;
    struct uchar_nc;

    value_type v_;
};

} // bnf
} // urls
} // boost

#endif
