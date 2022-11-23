//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

// Test that header file is self-contained.
#include <boost/url/experimental/router.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace experimental {

struct router_test
{
    void
    testRoute()
    {
        // static route
        {
            router<std::string> r;
            r.route("user", "view users");
            using match_res = router<std::string>::match_results;
            result<match_res> rm = r.match("user");
            BOOST_TEST(rm.has_value());
            match_res m = *rm;
            BOOST_TEST_EQ(*m, "view users");
        }

        // dynamic {id}
        {
            router<std::string> r;
            r.route("user/{name}", "read user");
            using match_res = router<std::string>::match_results;
            result<match_res> rm = r.match("user/vfalco");
            BOOST_TEST(rm.has_value());
            match_res m = *rm;
            BOOST_TEST_CSTR_EQ(*m, "read user");
        }

        // dynamic :id
        {
            router<std::string> r;
            // AFREITAS: ":" is the most common pattern
            // for dynamic segments, and it's quite clean.
            // However, it's also ambiguous because
            // ":" is in pchars.
            r.route("user/:name", "read user");
            using match_res = router<std::string>::match_results;
            result<match_res> rm = r.match("user/vfalco");
            BOOST_TEST_NOT(rm.has_value());
        }
    }

    void
    run()
    {
        testRoute();
    }
};

TEST_SUITE(router_test, "boost.url.router");

} // experimental
} // urls
} // boost
