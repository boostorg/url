//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/scheme.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>

namespace boost {
namespace url {

class scheme_test : public beast::unit_test::suite
{
public:
    void
    run() override
    {
        BEAST_EXPECT(is_special("ftp"));
        BEAST_EXPECT(is_special("file"));
        BEAST_EXPECT(is_special("http"));
        BEAST_EXPECT(is_special("https"));
        BEAST_EXPECT(is_special("ws"));
        BEAST_EXPECT(is_special("wss"));

        BEAST_EXPECT(! is_special("gopher"));
        BEAST_EXPECT(! is_special("magnet"));
        BEAST_EXPECT(! is_special("mailto"));
    }
};

BEAST_DEFINE_TESTSUITE(boost,url,scheme);

} // url
} // boost
