//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#include <boost/beast/_experimental/unit_test/suite.hpp>

namespace boost {
namespace url {

class sandbox_test : public beast::unit_test::suite
{
public:
    void
    doSandbox()
    {
        //auto u = parse("https://www.boost.org/index.html");
    }

    void
    run() override
    {
        doSandbox();
        pass();
    }
};

BEAST_DEFINE_TESTSUITE(boost,url,sandbox);

} // url
} // boost
