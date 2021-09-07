//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAllinace/url
//

#ifndef BOOST_URL_IMPL_IPV4_ADDRESS_HPP
#define BOOST_URL_IMPL_IPV4_ADDRESS_HPP

#include <memory>

namespace boost {
namespace urls {

template<class Allocator>
string_type<Allocator>
ipv4_address::
to_string(Allocator const& a) const
{
    string_type<Allocator> s(a);
    char buf[max_str_len];
    s.resize(print_impl(buf));
    std::memcpy(&s[0], buf, s.size());
    return s;
}

} // urls
} // boost

#endif
