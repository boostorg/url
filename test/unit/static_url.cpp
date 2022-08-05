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

#include <boost/url/url.hpp>
#include <boost/url/url_view.hpp>
#include "test_suite.hpp"
#include <iostream>

namespace boost {
namespace urls {

class static_url_test
{
public:
    using StaticUrl = static_url<1024>;

    void
    f1(url_view const&)
    {
    }

    void
    testSpecial()
    {
        // default ctor
        {
            StaticUrl u;
            BOOST_TEST_EQ(*u.c_str(), '\0');
            BOOST_TEST(u.string().empty());
        }

        url c1       = parse_uri("http://1").value();
        StaticUrl c2 = parse_uri("http://2").value();
        url_view c3  = parse_uri("http://3").value();
        url_view c4  = parse_uri("ftp://").value();

        // copy ctor
        {
            {
                StaticUrl u(c1);
                BOOST_TEST_EQ(u.string(), c1.string());
                BOOST_TEST_NE(u.c_str(), c1.string().data());
            }
            {
                StaticUrl u(c2);
                BOOST_TEST_EQ(u.string(), c2.string());
                BOOST_TEST_NE(u.c_str(), c2.string().data());
            }
            {
                StaticUrl u(c3);
                BOOST_TEST_EQ(u.string(), c3.string());
                BOOST_TEST_NE(u.c_str(), c3.string().data());
            }
            {
                // different sizes
                static_url<64> u1;
                static_url<128> u2(u1);
            }
        }

        // move ctor
        {
            {
                StaticUrl u(std::move(c1));
                BOOST_TEST_EQ(u.string(), c1.string());
                BOOST_TEST_NE(u.c_str(), c1.string().data());
            }
            {
                StaticUrl u(std::move(c2));
                BOOST_TEST_EQ(u.string(), c2.string());
                BOOST_TEST_NE(u.c_str(), c2.string().data());
            }
            {
                StaticUrl u(std::move(c3));
                BOOST_TEST_EQ(u.string(), c3.string());
                BOOST_TEST_NE(u.c_str(), c3.string().data());
            }
        }

        // copy assign
        {
            {
                StaticUrl u(c4);
                u = c1;
                BOOST_TEST_EQ(u.string(), c1.string());
                BOOST_TEST_NE(u.c_str(), c1.string().data());
            }
            {
                StaticUrl u(c4);
                u = c2;
                BOOST_TEST_EQ(u.string(), c2.string());
                BOOST_TEST_NE(u.c_str(), c2.string().data());
            }
            {
                StaticUrl u(c4);
                u = c3;
                BOOST_TEST_EQ(u.string(), c3.string());
                BOOST_TEST_NE(u.c_str(), c3.string().data());
            }
            {
                // different sizes
                static_url<64> u1;
                static_url<128> u2;
                u2 = u1;
            }
        }

        // move assign
        {
            {
                StaticUrl u(c4);
                u = std::move(c1);
                BOOST_TEST_EQ(u.string(), c1.string());
                BOOST_TEST_NE(u.c_str(), c1.string().data());
            }
            {
                StaticUrl u(c4);
                u = std::move(c2);
                BOOST_TEST_EQ(u.string(), c2.string());
                BOOST_TEST_NE(u.c_str(), c2.string().data());
            }
            {
                StaticUrl u(c4);
                u = std::move(c3);
                BOOST_TEST_EQ(u.string(), c3.string());
                BOOST_TEST_NE(u.c_str(), c3.string().data());
            }
        }

        // conversion
        {
            StaticUrl u;
            f1(u);
        }

        // static_url(string_view)
        {
            StaticUrl u("http://example.com/path/to/file.txt?#");
        }
    }

    void
    testParts()
    {
        url_view uv;
        BOOST_TEST_NO_THROW( uv = parse_uri(
            "http://user:pass@www.boost.org:8080/x/y/z?a=b&c=3#frag").value());
        StaticUrl u(uv);
        BOOST_TEST(u.encoded_origin() ==
            "http://user:pass@www.boost.org:8080");
        BOOST_TEST_EQ(u.scheme(), "http");
        BOOST_TEST_EQ(u.userinfo(), "user:pass");
        BOOST_TEST_EQ(u.user(), "user");
        BOOST_TEST_EQ(u.password(), "pass");
        BOOST_TEST_EQ(u.host(), "www.boost.org");
        BOOST_TEST_EQ(u.port(), "8080");
        BOOST_TEST_EQ(u.encoded_path(), "/x/y/z");
        BOOST_TEST_EQ(u.query(), "a=b&c=3");
        BOOST_TEST_EQ(u.encoded_fragment(), "frag");
    }

    void
    testOstream()
    {
        {
            static_url<64> u = parse_uri(
                "http://example.com/index.htm?q#f").value();
            std::stringstream ss;
            ss << u;
            BOOST_TEST(ss.str() ==
                "http://example.com/index.htm?q#f");
        }
    }

    void
    run()
    {
        testSpecial();
        testParts();
        testOstream();
    }
};

TEST_SUITE(
    static_url_test,
    "boost.url.static_url");

} // urls
} // boost
