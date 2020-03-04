//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/error.hpp>

#include "test_suite.hpp"

#include <memory>

namespace boost {
namespace urls {

class error_test
{
public:
    void check(error e)
    {
        auto const ec = make_error_code(e);
        BOOST_TEST(ec.category().name() != nullptr);
        BOOST_TEST(! ec.message().empty());
        BOOST_TEST(ec.category().default_error_condition(
            static_cast<int>(e)).category() == ec.category());
    }

    void check(condition c, error e)
    {
        {
            auto const ec = make_error_code(e);
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
        check(condition::parse_error, error::no_match);
        check(condition::parse_error, error::syntax);
        check(condition::parse_error, error::invalid);

        check(condition::parse_error, error::missing_scheme);
        check(condition::parse_error, error::bad_scheme_start_char);
        check(condition::parse_error, error::bad_scheme_char);
        check(condition::parse_error, error::bad_username_char);
        check(condition::parse_error, error::bad_userinfo_char);
        check(condition::parse_error, error::bad_port_char);
        check(condition::parse_error, error::port_overflow);
        check(condition::parse_error, error::missing_hostname);
        check(condition::parse_error, error::missing_port);

        check(condition::parse_error, error::bad_pct_encoding_digit);
        check(condition::parse_error, error::incomplete_pct_encoding);
        check(condition::parse_error, error::illegal_reserved_char);
    }
};

TEST_SUITE(error_test, "boost.url.error");

} // urls
} // boost
