//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/rfc/host.hpp>

#include "test_suite.hpp"
#include "test_bnf.hpp"

namespace boost {
namespace urls {
namespace rfc {

class host_test
{
public:
    void
    run()
    {
        using T = host;
        bad <T>("%");
        good<T>("");
        good<T>("[::]");
        good<T>("1.2.3.4");
        good<T>("boost.org");
        good<T>("999.0.0.1"); // name
    }
};

TEST_SUITE(
    host_test,
    "boost.url.host");

} // rfc
} // urls
} // boost
