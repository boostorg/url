//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/static_url.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

class static_url_test
{
public:
    //------------------------------------------------------

    void
    run()
    {
    }
};

TEST_SUITE(static_url_test, "boost.url.static_url");

} // urls
} // boost
