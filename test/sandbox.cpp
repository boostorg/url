//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#include "test_suite.hpp"

#if 0
namespace boost {
namespace urls {

class url_view;

class url_base;

template<class Allocator>
class basic_url;

template<std::size_t N>
class static_url;

using url = basic_url<std::allocator<char>>;

} // urls
} // boost
#endif

namespace boost {
namespace urls {

class sandbox_test
{
public:
    void
    run()
    {
    }
};

TEST_SUITE(sandbox_test, "boost.url.sandbox");

} // urls
} // boost
