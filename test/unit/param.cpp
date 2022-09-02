//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/param.hpp>

#include <boost/core/ignore_unused.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct param_test
{
    void
    testParam()
    {
        auto const check =
            []( param const& qp,
                string_view key,
                string_view value,
                bool has_value)
        {
            BOOST_TEST_EQ(qp.key, key);
            BOOST_TEST_EQ(qp.value, value);
            BOOST_TEST_EQ(qp.has_value, has_value);
        };

        // param()
        {
            param qp;
            check(qp, "", "", false);
        }

        // param(string_view)
        {
            param qp("key");
            check(qp, "key", "", false);
        }

        // param(string_view, string_view)
        {
            {
                param qp("key", "value");
                check(qp, "key", "value", true);
            }
            {
                param qp("key", "");
                check(qp, "key", "", true);
            }
        }

        // param(param_view)
        {
            {
                param qp((param_view()));
                check(qp, "", "", false);
            }
            {
                param qp((param_view("key")));
                check(qp, "key", "", false);
            }
            {
                param qp((param_view("key", "value")));
                check(qp, "key", "value", true);
            }
            {
                param qp((param_view("key", "")));
                check(qp, "key", "", true);
            }
        }
    }

    void
    testParamView()
    {
        auto const check =
            []( param_view const& qp,
                string_view key,
                string_view value,
                bool has_value)
        {
            BOOST_TEST_EQ(qp.key, key);
            BOOST_TEST_EQ(qp.value, value);
            BOOST_TEST_EQ(qp.has_value, has_value);
        };

        // param_view()
        {
            param_view qp;
            check(qp, "", "", false);
        }

        // param_view(string_view)
        {
            param_view qp("key");
            check(qp, "key", "", false);
        }

        // param_view(string_view, string_view)
        {
            {
                param_view qp("key", "value");
                check(qp, "key", "value", true);
            }
            {
                param_view qp("key", "");
                check(qp, "key", "", true);
            }
        }
    }

    void
    testParamDecodeView()
    {
    }

    void
    testNatvis()
    {
        param v0 = {};
        param v1 = { "key" };
        param v2 = { "key", "" };
        param v3 = { "key", "value" };

        param_view pv0 = {};
        param_view pv1 = { "key" };
        param_view pv2 = { "key", "" };
        param_view pv3 = { "key", "value" };

        param_pct_view d0 = {};
#if 0
        param_pct_view d1 = { "key" };
        param_pct_view d2 = { "key", "" };
        param_pct_view d3 = { "key", "value" };
#endif

        boost::ignore_unused(
            v0, v1, v2, v3,
            pv0, pv1, pv2, pv3,
            d0);
    }

    void
    run()
    {
        testParam();
        testParamView();
        testParamDecodeView();
        testNatvis();
    }
};

TEST_SUITE(
    param_test,
    "boost.url.param");

} // urls
} // boost
