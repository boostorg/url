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

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <memory>

namespace boost {
namespace url {

class error_test : public beast::unit_test::suite
{
public:
    void check(error e)
    {
        auto const ec = make_error_code(e);
        BEAST_EXPECT(ec.category().name() != nullptr);
        BEAST_EXPECT(! ec.message().empty());
        BEAST_EXPECT(ec.category().default_error_condition(
            static_cast<int>(e)).category() == ec.category());
    }

    void check(condition c, error e)
    {
        {
            auto const ec = make_error_code(e);
            BEAST_EXPECT(ec.category().name() != nullptr);
            BEAST_EXPECT(! ec.message().empty());
            BEAST_EXPECT(ec == c);
        }
        {
            auto ec = make_error_condition(c);
            BEAST_EXPECT(ec.category().name() != nullptr);
            BEAST_EXPECT(! ec.message().empty());
            BEAST_EXPECT(ec == c);
        }
    }

    void
    run() override
    {
        check(condition::parse_error, error::mismatch);
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

BEAST_DEFINE_TESTSUITE(boost,url,error);

} // url
} // boost
