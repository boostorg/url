//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/static_url.hpp>

#include <boost/url/url_view.hpp>
#include "test_suite.hpp"
#include <iostream>

namespace boost {
namespace urls {

class static_url_test
{
public:
    using url_t = static_url<1024>;

    void
    f1(url_view const&)
    {
    }

    void
    testSpecial()
    {
        // default ctor
        {
            url_t u;
            BOOST_TEST_EQ(*u.c_str(), '\0');
            BOOST_TEST(u.string().empty());
        }

        url c1      = parse_uri("http://1").value();
        url_t c2    = parse_uri("http://2").value();
        url_view c3 = parse_uri("http://3").value();
        url_view c4 = parse_uri("ftp://").value();

        // copy ctor
        {
            {
                url_t u(c1);
                BOOST_TEST_EQ(u.string(), c1.string());
                BOOST_TEST_NE(u.c_str(), c1.string().data());
            }
            {
                url_t u(c2);
                BOOST_TEST_EQ(u.string(), c2.string());
                BOOST_TEST_NE(u.c_str(), c2.string().data());
            }
            {
                url_t u(c3);
                BOOST_TEST_EQ(u.string(), c3.string());
                BOOST_TEST_NE(u.c_str(), c3.string().data());
            }
        }

        // move ctor
        {
            {
                url_t u(std::move(c1));
                BOOST_TEST_EQ(u.string(), c1.string());
                BOOST_TEST_NE(u.c_str(), c1.string().data());
            }
            {
                url_t u(std::move(c2));
                BOOST_TEST_EQ(u.string(), c2.string());
                BOOST_TEST_NE(u.c_str(), c2.string().data());
            }
            {
                url_t u(std::move(c3));
                BOOST_TEST_EQ(u.string(), c3.string());
                BOOST_TEST_NE(u.c_str(), c3.string().data());
            }
        }

        // copy assign
        {
            {
                url_t u(c4);
                u = c1;
                BOOST_TEST_EQ(u.string(), c1.string());
                BOOST_TEST_NE(u.c_str(), c1.string().data());
            }
            {
                url_t u(c4);
                u = c2;
                BOOST_TEST_EQ(u.string(), c2.string());
                BOOST_TEST_NE(u.c_str(), c2.string().data());
            }
            {
                url_t u(c4);
                u = c3;
                BOOST_TEST_EQ(u.string(), c3.string());
                BOOST_TEST_NE(u.c_str(), c3.string().data());
            }
        }

        // move assign
        {
            {
                url_t u(c4);
                u = std::move(c1);
                BOOST_TEST_EQ(u.string(), c1.string());
                BOOST_TEST_NE(u.c_str(), c1.string().data());
            }
            {
                url_t u(c4);
                u = std::move(c2);
                BOOST_TEST_EQ(u.string(), c2.string());
                BOOST_TEST_NE(u.c_str(), c2.string().data());
            }
            {
                url_t u(c4);
                u = std::move(c3);
                BOOST_TEST_EQ(u.string(), c3.string());
                BOOST_TEST_NE(u.c_str(), c3.string().data());
            }
        }

        // conversion
        {
            url_t u;
            f1(u);
        }

        // static_url(string_view)
        {
            url_t u("http://example.com/path/to/file.txt?#");
        }
    }

    void
    testParts()
    {
        url_view uv;
        BOOST_TEST_NO_THROW( uv = parse_uri(
            "http://user:pass@www.boost.org:8080/x/y/z?a=b&c=3#frag").value());
        url_t u(uv);
        BOOST_TEST(u.encoded_origin() ==
            "http://user:pass@www.boost.org:8080");
        BOOST_TEST_EQ(u.scheme(), "http");
        BOOST_TEST_EQ(u.user(), "user");
        BOOST_TEST_EQ(u.password(), "pass");
        BOOST_TEST_EQ(u.host(), "www.boost.org");
        BOOST_TEST_EQ(u.port(), "8080");
        BOOST_TEST_EQ(u.encoded_path(), "/x/y/z");
        BOOST_TEST_EQ(u.query(), "a=b&c=3");
        BOOST_TEST_EQ(u.encoded_fragment(), "frag");
    }

    void
    run()
    {
        testSpecial();
        testParts();
    }
};

TEST_SUITE(
    static_url_test,
    "boost.url.static_url");

} // urls
} // boost
