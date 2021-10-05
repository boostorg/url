//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_IMPL_IPV6_ADDRESS_HPP
#define BOOST_URL_IMPL_IPV6_ADDRESS_HPP

#include <memory>

namespace boost {
namespace urls {

template<class Allocator>
string_value
ipv6_address::
to_string(Allocator const& a) const
{
    char buf[max_str_len];
    auto const n = print_impl(buf);
    char* dest;
    string_value s(n, a, dest);
    std::memcpy(dest, buf, n);
    return s;
}

} // urls
} // boost

#endif
