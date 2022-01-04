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
        BOOST_TEST(ec.category().name() != nullptr);
        BOOST_TEST(! ec.message().empty());
        BOOST_TEST(ec.category().default_error_condition(
            static_cast<int>(e)).category() == ec.category());
    }

    void check(grammar::condition c, error e)
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
        check(error::syntax);

        check(error::bad_alpha);
        check(error::bad_digit);
        check(error::bad_empty_element);
        check(error::bad_hexdig);
        check(error::bad_ipv6);
        check(error::bad_leading_zero);
        check(error::bad_octet);
        check(error::bad_schemeless_path_segment);
        check(error::empty);
        check(error::empty_path_segment);
        check(error::illegal_null);
        check(error::illegal_reserved_char);
        check(error::invalid);
        check(error::missing_char_literal);
        check(error::missing_path_segment);
        check(error::missing_path_separator);
        check(error::missing_words);
        check(error::non_canonical);
        check(error::wrong_char_literal);

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
