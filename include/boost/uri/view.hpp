//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/uri
//

#ifndef BOOST_URI_VIEW_HPP
#define BOOST_URI_VIEW_HPP

#include <boost/beast/uri/parts.hpp>
#include <iosfwd>

namespace boost {
namespace beast {
namespace uri {

struct view
{
    view() = default;
    view(view const&) = default;
    view& operator=(view const&) = default;

    view(parts const& p, char const* data) noexcept
        : p_(p)
    {
        p_.data = data;
    }

    string_view
    str() const noexcept
    {
        return {p_.data, p_.size};
    }

    char const*
    c_str() const noexcept
    {
        return p_.data;
    }

    known_scheme
    scheme_value() const noexcept
    {
        return p_.scheme_value;
    }

    string_view
    scheme() const noexcept
    {
        return p_.scheme(p_.data);
    }

    string_view
    authority() const noexcept
    {
        return p_.authority(p_.data);
    }

    string_view
    userinfo() const noexcept
    {
        return p_.userinfo(p_.data);
    }

    string_view
    username() const noexcept
    {
        return p_.username(p_.data);
    }

    string_view
    password() const noexcept
    {
        return p_.password(p_.data);
    }

    string_view
    host() const noexcept
    {
        return p_.host(p_.data);
    }

    string_view
    port() const noexcept
    {
        return p_.port(p_.data);
    }

    string_view
    path() const noexcept
    {
        return p_.path(p_.data);
    }

    string_view
    query() const noexcept
    {
        return p_.query(p_.data);
    }

    string_view
    fragment() const noexcept
    {
        return p_.fragment(p_.data);
    }

    parts const&
    cparts() const noexcept
    {
        return p_;
    }

protected:
    parts p_;
};

inline
std::ostream&
operator<<(std::ostream& os, view const& v)
{
    os << v.str();
    return os;
}

} // uri
} // beast
} // boost

#endif
