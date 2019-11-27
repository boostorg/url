//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_URL_HPP
#define BOOST_URL_URL_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/core/string.hpp>

namespace boost {
namespace beast {
namespace url {

#if 0
namespace detail {
template<class Buffer>
class parser;
} // detail

template<class Buffer>
class basic_uri
{
    template<class Buffer>
    friend class detail::parser;

    detail::parts p_;
    Buffer b_;

    explicit
    basic_uri(detail::parts const& p)
        : p_(p)
    {
    }

public:
    basic_uri() = default;
    basic_uri(basic_uri&&) = default;
    basic_uri(basic_uri const&) = default;
    basic_uri& operator=(basic_uri&&) = default;
    basic_uri& operator=(basic_uri const&) = default;

    template<class... Args>
    explicit
    basic_uri(Args&&... args)
        : b_(std::forward<Args>(args)...)
    {
    }

    //
    // Observers
    //

    known_scheme
    scheme_value() const
    {
        return p_.scheme_value;
    }

    string_view
    scheme() const
    {
        return p_.scheme(b_.data());
    }

    //
    // Modifiers
    //

    void
    clear()
    {
        p_ = parse{};
        b_.clear();
    }
};

//------------------------------------------------------------------------------

template<unsigned short N>
class static_buffer
{
    char buf_[N];

public:
    char*
    data()
    {
        return buf_;
    }

    char const*
    data() const
    {
        return buf_;
    }

    std::size_t
    size() const
    {
        return N;
    }
};
#endif

} // url
} // beast
} // boost

#endif
