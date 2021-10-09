//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/authority_view.hpp>

#include <boost/url/static_pool.hpp>
#include <boost/url/bnf/parse.hpp>
#include "test_bnf.hpp"
#include "test_suite.hpp"
#include <sstream>

namespace boost {
namespace urls {

class authority_view_test
{
public:
    void
    run()
    {
    }
};

TEST_SUITE(
    authority_view_test,
    "boost.url.authority_view");

} // urls
} // boost
