//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/grammar/type_traits.hpp>

#include <string>

#include "test_suite.hpp"

namespace boost {
namespace urls {
namespace grammar {

struct type_traits_test
{
    void
    run()
    {
#ifdef __cpp_lib_concepts
        BOOST_TEST(std::input_iterator<detail::input_it_ex>);
#endif
        BOOST_TEST(is_mutable_string<std::string>::value);
        BOOST_TEST_NOT(is_mutable_string<int>::value);
        BOOST_TEST_NOT(is_mutable_string<std::vector<char>>::value);
        BOOST_TEST_NOT(is_mutable_string<std::string const>::value);
    }
};

TEST_SUITE(
    type_traits_test,
    "boost.url.grammar.type_traits");

} // grammar
} // urls
} // boost
