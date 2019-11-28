//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/static_pool.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <iostream>

namespace boost {
namespace url {

class static_pool_test : public beast::unit_test::suite
{
public:
    void
    run() override
    {
        pass();
    }
};

BEAST_DEFINE_TESTSUITE(boost,url,static_pool);

} // url
} // boost
