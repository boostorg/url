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
            BOOST_TEST(*u.c_str() == '\0');
            BOOST_TEST(u.str().empty());
        }

        url c1      = parse_uri("http://1");
        url_t c2    = parse_uri("http://2");
        url_view c3 = parse_uri("http://3");
        url_view c4 = parse_uri("ftp://");

        // copy ctor
        {
            {
                url_t u(c1);
                BOOST_TEST(u.str() == c1.str());
                BOOST_TEST(u.c_str() != c1.str().data());
            }
            {
                url_t u(c2);
                BOOST_TEST(u.str() == c2.str());
                BOOST_TEST(u.c_str() != c2.str().data());
            }
            {
                url_t u(c3);
                BOOST_TEST(u.str() == c3.str());
                BOOST_TEST(u.c_str() != c3.str().data());
            }
        }

        // move ctor
        {
            {
                url_t u(std::move(c1));
                BOOST_TEST(u.str() == c1.str());
                BOOST_TEST(u.c_str() != c1.str().data());
            }
            {
                url_t u(std::move(c2));
                BOOST_TEST(u.str() == c2.str());
                BOOST_TEST(u.c_str() != c2.str().data());
            }
            {
                url_t u(std::move(c3));
                BOOST_TEST(u.str() == c3.str());
                BOOST_TEST(u.c_str() != c3.str().data());
            }
        }

        // copy assign
        {
            {
                url_t u(c4);
                u = c1;
                BOOST_TEST(u.str() == c1.str());
                BOOST_TEST(u.c_str() != c1.str().data());
            }
            {
                url_t u(c4);
                u = c2;
                BOOST_TEST(u.str() == c2.str());
                BOOST_TEST(u.c_str() != c2.str().data());
            }
            {
                url_t u(c4);
                u = c3;
                BOOST_TEST(u.str() == c3.str());
                BOOST_TEST(u.c_str() != c3.str().data());
            }
        }

        // move assign
        {
            {
                url_t u(c4);
                u = std::move(c1);
                BOOST_TEST(u.str() == c1.str());
                BOOST_TEST(u.c_str() != c1.str().data());
            }
            {
                url_t u(c4);
                u = std::move(c2);
                BOOST_TEST(u.str() == c2.str());
                BOOST_TEST(u.c_str() != c2.str().data());
            }
            {
                url_t u(c4);
                u = std::move(c3);
                BOOST_TEST(u.str() == c3.str());
                BOOST_TEST(u.c_str() != c3.str().data());
            }
        }

        // conversion
        {
            url_t u;
            f1(u);
        }
    }

    void
    testParts()
    {
        url_view uv;
        BOOST_TEST_NO_THROW( uv = parse_uri(
            "http://username:pass@www.boost.org:8080/x/y/z?a=b&c=3#frag"));
        url_t u(uv);
        BOOST_TEST(u.encoded_origin() ==
            "http://username:pass@www.boost.org:8080");
        BOOST_TEST(u.scheme() == "http");
        BOOST_TEST(u.username() == "username");
        BOOST_TEST(u.password() == "pass");
        BOOST_TEST(u.host() == "www.boost.org");
        BOOST_TEST(u.port() == "8080");
        BOOST_TEST(u.encoded_path() == "/x/y/z");
        BOOST_TEST(u.query() == "a=b&c=3");
        BOOST_TEST(u.encoded_fragment() == "frag");
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
