//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PARAMS_VIEW_HPP
#define BOOST_URL_IMPL_PARAMS_VIEW_HPP

#include <cstddef>

namespace boost {
namespace urls {

class params_view::iterator
{
    char const* p_ = nullptr;
    std::size_t nk_ = 0;
    std::size_t nv_ = 0;
    char const* next_ = nullptr;
    string_value::allocator a_;

    friend class params_view;

    iterator(
        string_view s,
        string_value::allocator a)
        : a_(a)
    {
        // VFALCO TODO
        (void)s;
    }

public:
    BOOST_URL_DECL
    ~iterator() noexcept;

    BOOST_URL_DECL
    iterator() noexcept;
};

} // urls
} // boost

#endif
