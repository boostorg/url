//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/host_type.hpp>

#include <boost/url/detail/test/test_suite.hpp>

namespace boost {
namespace urls {

class host_type_test
{
public:
    void
    run()
    {
    }
};

TEST_SUITE(
    host_type_test,
    "boost.url.host_type");

} // urls
} // boost
