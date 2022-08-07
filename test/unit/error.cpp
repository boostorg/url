//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/error.hpp>
#include <boost/url/grammar/error.hpp>

#include "test_suite.hpp"

#include <memory>

namespace boost {
namespace urls {

class error_test
{
public:
    void check(error e)
    {
        auto const ec = BOOST_URL_ERR(e);
        BOOST_TEST_NE(ec.category().name(), nullptr);
        BOOST_TEST(! ec.message().empty());
        BOOST_TEST(ec.category().default_error_condition(
            static_cast<int>(e)).category() == ec.category());
    }

    void check(grammar::condition c, error e)
    {
        {
            auto const ec = BOOST_URL_ERR(e);
            BOOST_TEST_NE(ec.category().name(), nullptr);
            BOOST_TEST(! ec.message().empty());
            BOOST_TEST_EQ(ec, c);
        }
        {
            auto ec = make_error_condition(c);
            BOOST_TEST_NE(ec.category().name(), nullptr);
            BOOST_TEST(! ec.message().empty());
            BOOST_TEST_EQ(ec, c);
        }
    }

    void
    run()
    {
        check(error::illegal_null);
        check(error::illegal_reserved_char);
        check(error::non_canonical);

        check(grammar::condition::fatal, error::bad_pct_hexdig);
        check(grammar::condition::fatal, error::incomplete_pct_encoding);
        check(grammar::condition::fatal, error::missing_pct_hexdig);

        check(error::no_space);
        check(error::not_a_base);
    }
};

TEST_SUITE(error_test, "boost.url.error");

} // urls
} // boost
