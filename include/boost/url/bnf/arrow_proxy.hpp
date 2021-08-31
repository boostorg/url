//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/http_proto
//

#ifndef BOOST_URL_BNF_ARROW_PROXY_HPP
#define BOOST_URL_BNF_ARROW_PROXY_HPP

#include <boost/url/detail/config.hpp>
#include <memory>

namespace boost {
namespace urls {
namespace bnf {

template<class T>
struct arrow_proxy
{
    T t;

    T const*
    operator->() const noexcept
    {
        return std::addressof(t);
    }
};

} // bnf
} // urls
} // boost

#endif
