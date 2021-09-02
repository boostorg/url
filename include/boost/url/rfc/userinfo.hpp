//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_USERINFO_HPP
#define BOOST_URL_RFC_USERINFO_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/char_type.hpp>

namespace boost {
namespace urls {
namespace rfc {

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
            using urls::detail::decode;
            return decode(user_, a);
        }

        template<
            class Allocator =
                std::allocator<char>>
        string_type<Allocator>
        password(
            Allocator const& a = {}) const
        {
            using urls::detail::decode;
            return decode(password_, a);
        }
    };

    value_type const&
    operator*() const noexcept
    {
        return v_;
    }

    value_type const*
    operator->() const noexcept
    {
        return &v_;
    }

    BOOST_URL_DECL
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec);

private:
    class uchar;
    class uchar_nc;

    value_type v_;
};

} // rfc
} // urls
} // boost

#endif
