//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_VIEW_HPP
#define BOOST_URL_PARAMS_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <cstddef>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view;
#endif

class params_view
    : protected detail::parts_base
{
    friend class url_view;

    url_view const* u_ = nullptr;
    string_value::allocator a_;

    template<class Allocator>
    params_view(
        url_view& u,
        Allocator const& a) noexcept
        : u_(&u)
        , a_(a)
    {
    }

public:
    class iterator;

    BOOST_URL_DECL
    bool
    empty() const noexcept;

    BOOST_URL_DECL
    std::size_t
    size() const noexcept;

    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    BOOST_URL_DECL
    iterator
    end() const noexcept;
};

} // urls
} // boost

#include <boost/url/impl/params_view.hpp>

#endif
