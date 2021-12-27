//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/bnf/error.hpp>

#include "test_suite.hpp"

#include <memory>

namespace boost {
namespace urls {
namespace bnf {

class error_test
{
public:
    void check(error e)
    {
        auto const ec = BOOST_URL_ERR(e);
        BOOST_TEST(ec.category().name() != nullptr);
        BOOST_TEST(! ec.message().empty());
        BOOST_TEST(ec.category().default_error_condition(
            static_cast<int>(e)).category() == ec.category());
    }

    void check(condition c, error e)
    {
        {
            auto const ec = BOOST_URL_ERR(e);
            BOOST_TEST(ec.category().name() != nullptr);
            BOOST_TEST(! ec.message().empty());
            BOOST_TEST(ec == c);
        }
        {
            auto ec = make_error_condition(c);
            BOOST_TEST(ec.category().name() != nullptr);
            BOOST_TEST(! ec.message().empty());
            BOOST_TEST(ec == c);
        }
    }

    void
    run()
    {
        check(error::end);
        check(error::incomplete);
        check(error::leftover);
        check(error::syntax);
    }
};

TEST_SUITE(
    error_test,
    "boost.url.bnf.error");

} // bnf
} // urls
} // boost
