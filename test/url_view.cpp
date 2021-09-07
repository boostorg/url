//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/url_view.hpp>

#include <boost/url/static_pool.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

class url_view_test
{
public:
#if 0
    void
    testSegments()
    {
        BOOST_TEST(url_view().segments().size() == 0);
        BOOST_TEST(url_view("x:a").segments().size() == 1);
        BOOST_TEST(url_view("x:/a").segments().size() == 1);
        BOOST_TEST(url_view("x://y/a").segments().size() == 1);

        BOOST_TEST(url_view("x").segments().size() == 1);
        BOOST_TEST(url_view("x/").segments().size() == 2);
        BOOST_TEST(url_view("x//").segments().size() == 3);

        BOOST_TEST(url_view("/").segments().size() == 1);

        {
            url_view::segments_type const ps{};
            BOOST_TEST(ps.empty());
            BOOST_TEST(ps.size() == 0);
            BOOST_TEST(ps.begin() == ps.end());
            BOOST_TEST(
                url_view::segments_type::iterator() ==
                url_view::segments_type::iterator());
        }
        {
            url_view const v("/path/to/file.txt");
            auto const ps = v.segments();
            BOOST_TEST(! ps.empty());
            BOOST_TEST(ps.size() == 3);
            BOOST_TEST(ps.begin() != ps.end());
            BOOST_TEST(ps.end() == ps.end());

            static_pool<4000> sp;
            {
                auto it = ps.begin();
                BOOST_TEST(it->string(sp.allocator()) == "path"); ++it;
                BOOST_TEST(it->string(sp.allocator()) == "to"); ++it;
                BOOST_TEST(it->string(sp.allocator()) == "file.txt");
            }

            auto it = ps.begin();
            BOOST_TEST(it->encoded_string() == "path");
            it++;
            BOOST_TEST(it->encoded_string() == "to");
            ++it;
            BOOST_TEST(it->encoded_string() == "file.txt");
            --it;
            BOOST_TEST(it->encoded_string() == "to");
            it--;
            BOOST_TEST(it->encoded_string() == "path");
        }
    }
#endif

    //--------------------------------------------

    void
    testParse()
    {
        error_code ec;
        auto const u = urls::parse_uri(
            "http://username:pass@www.boost.org:8080/x/y/z?a=b&c=3#frag",
            ec);
        if(! BOOST_TEST(! ec))
            return;
        BOOST_TEST(u.has_value());
        BOOST_TEST(u->scheme() == "http");
        BOOST_TEST(u->username() == "username");
        BOOST_TEST(u->password() == "pass");
        BOOST_TEST(u->host() == "www.boost.org");
        BOOST_TEST(u->port() == "8080");
        BOOST_TEST(u->encoded_path() == "/x/y/z");
        BOOST_TEST(u->query() == "a=b&c=3");
        BOOST_TEST(u->encoded_fragment() == "frag");
    }

    void
    testScheme()
    {
        {
            auto u = parse_uri(
                "http://");
            BOOST_TEST(u.has_scheme());
            BOOST_TEST(u.scheme() == "http");
        }
        {
            auto u = parse_relative_ref(
                "/x");
            BOOST_TEST(! u.has_scheme());
            BOOST_TEST(u.scheme() == "");
        }
    }

    void
    testAuthority()
    {
        auto const no =
            [](string_view s)
        {
            BOOST_TEST_NO_THROW(
            [s]{
                auto u = parse_uri(s);
                BOOST_TEST(! u.has_authority());
            }());
        };
        auto const yes =
            [](string_view s, string_view m)
        {
            BOOST_TEST_NO_THROW(
            [&]{
                auto u = parse_uri(s);
                BOOST_TEST(u.has_authority());
                BOOST_TEST(
                    u.encoded_authority() == m);
            }());
        };

        no("http:xyz/");
        no("http:/x");
        no("http:/x");
        no("http:%2f%2f");
        no("http:/%40");

        yes("http://", "");
        yes("http://a", "a");
        yes("http://a@", "a@");
        yes("http://:@", ":@");
        yes("http://@", "@");
        yes("http://@x", "@x");
    }

    void
    testUserinfo()
    {
        auto const no =
            [](string_view s)
        {
            BOOST_TEST_NO_THROW(
            [s]{
                auto u = parse_uri(s);
                BOOST_TEST(! u.has_userinfo());
            }());
        };
        auto const yes =
            []( string_view s,
                string_view m1,
                string_view m2)
        {
            BOOST_TEST_NO_THROW(
            [&]{
                auto u = parse_uri(s);
                BOOST_TEST(u.has_userinfo());
                BOOST_TEST(
                    u.encoded_userinfo() == m1);
                BOOST_TEST(
                    u.userinfo() == m2);
            }());
        };

        no("http:");
        no("http:xyz/");
        no("http:/x");
        no("http:/x");
        no("http:%2f%2f");
        no("http:/%40");
        no("http://");
        no("http://a");

        yes("http://a@", "a", "a");
        yes("http://:@", ":", ":");
        yes("http://@", "", "");
        yes("http://@x", "", "");
        yes("http://%61@x", "%61", "a");
        yes("http://:%61@x", ":%61", ":a");
        yes("http://%61%3a%62@x", "%61%3a%62", "a:b");

        {
            auto u = parse_uri("x://@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "");
            BOOST_TEST(u.userinfo() == "");
            BOOST_TEST(u.has_username() == false);
            BOOST_TEST(u.encoded_username() == "");
            BOOST_TEST(u.username() == "");
            BOOST_TEST(u.has_password() == false);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        }
        {
            auto u = parse_uri("x://:@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == ":");
            BOOST_TEST(u.userinfo() == ":");
            BOOST_TEST(u.has_username() == false);
            BOOST_TEST(u.encoded_username() == "");
            BOOST_TEST(u.username() == "");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        }
        {
            auto u = parse_uri("x://a%41:@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "a%41:");
            BOOST_TEST(u.has_username() == true);
            BOOST_TEST(u.encoded_username() == "a%41");
            BOOST_TEST(u.username() == "aA");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        }
        {
            auto u = parse_uri("x://:b%42@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == ":b%42");
            BOOST_TEST(u.has_username() == false);
            BOOST_TEST(u.encoded_username() == "");
            BOOST_TEST(u.username() == "");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "b%42");
            BOOST_TEST(u.password() == "bB");
        }
        {
            auto u = parse_uri("x://a:b@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "a:b");
            BOOST_TEST(u.has_username() == true);
            BOOST_TEST(u.encoded_username() == "a");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "b");
        }
        {
            auto u = parse_uri("x://%3a:%3a@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "%3a:%3a");
            BOOST_TEST(u.userinfo() == ":::");
            BOOST_TEST(u.has_username() == true);
            BOOST_TEST(u.encoded_username() == "%3a");
            BOOST_TEST(u.username() == ":");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "%3a");
            BOOST_TEST(u.password() == ":");
        }
        {
            auto u = parse_uri("x://%2525@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "%2525");
            BOOST_TEST(u.userinfo() == "%25");
            BOOST_TEST(u.has_username() == true);
            BOOST_TEST(u.encoded_username() == "%2525");
            BOOST_TEST(u.username() == "%25");
            BOOST_TEST(u.has_password() == false);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        }
    }

    void
    testHost()
    {
        {
            auto u = parse_uri(
                "res:foo/");
            BOOST_TEST(u.host_type() ==
                host_type::none);
            BOOST_TEST(u.encoded_host() ==
                "");
        }
        {
            auto u = parse_uri(
                "http://");
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST(u.encoded_host() ==
                "");
        }
        {
            auto u = parse_uri(
                "http:///");
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST(u.encoded_host() ==
                "");
        }
        {
            auto u = parse_uri(
                "http://www.example.com/");
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST(u.encoded_host() ==
                "www.example.com");
            BOOST_TEST(u.host() ==
                "www.example.com");
        }
        {
            auto u = parse_uri(
                "http://192.168.0.1/");
            BOOST_TEST(u.host_type() ==
                host_type::ipv4);
            BOOST_TEST(u.encoded_host() ==
                "192.168.0.1");
            BOOST_TEST(u.host() ==
                "192.168.0.1");
            BOOST_TEST(
                u.ipv4_address().to_uint() ==
                    0xc0a80001);
        }
        {
            auto u = parse_uri(
                "http://[1::6:192.168.0.1]:8080/");
            BOOST_TEST(u.host_type() ==
                host_type::ipv6);
            BOOST_TEST(u.encoded_host() ==
                "[1::6:192.168.0.1]");
            BOOST_TEST(u.host() ==
                "[1::6:192.168.0.1]");
            BOOST_TEST(u.ipv6_address() ==
                make_ipv6_address("1::6:c0a8:1"));
        }
        {
            auto u = parse_uri(
                "http://[v1.x]:8080/");
            BOOST_TEST(u.host_type() ==
                host_type::ipvfuture);
            BOOST_TEST(u.encoded_host() ==
                "[v1.x]");
            BOOST_TEST(u.host() ==
                "[v1.x]");
            BOOST_TEST(u.ipv_future() ==
                "[v1.x]");
        }
    }

    void
    testPort()
    {
        {
            auto u = parse_uri(
                "http://");
            BOOST_TEST(! u.has_port());
            BOOST_TEST(u.port() == "");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            auto u = parse_uri(
                "http://www");
            BOOST_TEST(! u.has_port());
            BOOST_TEST(u.port() == "");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            auto u = parse_uri(
                "http://:");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            auto u = parse_uri(
                "http://:0");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "0");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            auto u = parse_uri(
                "http://:42");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "42");
            BOOST_TEST(u.port_number() == 42);
        }
        {
            auto u = parse_uri(
                "http://:65535");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "65535");
            BOOST_TEST(u.port_number() == 65535);
        }
        {
            auto u = parse_uri(
                "http://:65536");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "65536");
            BOOST_TEST(u.port_number() == 0);
        }
    }

    void
    testQuery()
    {
        {
            auto u = parse_uri(
                "http://");
            BOOST_TEST(! u.has_query());
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.query() == "");
        }
        {
            auto u = parse_uri(
                "http://?");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.query() == "");
        }
        {
            auto u = parse_uri(
                "http://?k");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "k");
            BOOST_TEST(u.query() == "k");
        }
        {
            auto u = parse_uri(
                "http://?k=");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "k=");
            BOOST_TEST(u.query() == "k=");
        }
        {
            auto u = parse_uri(
                "http://?#");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.query() == "");
        }
        {
            auto u = parse_uri(
                "http://?%3f");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "%3f");
            BOOST_TEST(u.query() == "?");
        }
        {
            auto u = parse_uri(
                "http://?%25");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "%25");
            BOOST_TEST(u.query() == "%");
        }
        {
            auto u = parse_uri(
                "http://?&");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "&");
            BOOST_TEST(u.query() == "&");
        }
        {
            auto u = parse_uri(
                "http://?%26");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "%26");
            BOOST_TEST(u.query() == "&");
        }
        {
            auto u = parse_uri(
                "http://?a%3db%26");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "a%3db%26");
            BOOST_TEST(u.query() == "a=b&");
        }

        {
            query_params_view const qp{};
            BOOST_TEST(qp.empty());
            BOOST_TEST(qp.size() == 0);
            BOOST_TEST(qp.begin() == qp.end());
            BOOST_TEST(! qp.contains("x"));
            BOOST_TEST(qp.count("x") == 0);
            BOOST_TEST(qp.find("x") == qp.end());
            BOOST_TEST_THROWS(qp.at("x"), std::out_of_range);

            BOOST_TEST(
                query_params_view::iterator() ==
                query_params_view::iterator());
        }
        {
            url_view const v("?x=1&y=2&y=3&z");
            auto qp = v.query_params();
            BOOST_TEST(! qp.empty());
            BOOST_TEST(qp.size() == 4);
            BOOST_TEST(qp.begin() != qp.end());
            BOOST_TEST(qp.end() == qp.end());
            BOOST_TEST(qp.contains("x"));
            BOOST_TEST(qp.contains("y"));
            BOOST_TEST(! qp.contains("a"));
            BOOST_TEST(qp.count("x") == 1);
            BOOST_TEST(qp.count("y") == 2);
            BOOST_TEST(qp.count("a") == 0);
            BOOST_TEST(qp.find("x")->encoded_value() == "1");
            BOOST_TEST(qp.find("y")->encoded_value() == "2");
            BOOST_TEST(qp.find("a") == qp.end());
            BOOST_TEST(qp["x"] == "1");
            BOOST_TEST(qp["y"] == "2");
            BOOST_TEST(qp["a"] == "");
            BOOST_TEST(qp.at("x") == "1");
            BOOST_TEST(qp.at("y") == "2");

            BOOST_TEST_THROWS(
                qp.at("a"),
                std::out_of_range);

            static_pool<4000> sp;
            {
                auto it = qp.begin();
                BOOST_TEST(it->key(sp.allocator()) == "x"); ++it;
                BOOST_TEST(it->key(sp.allocator()) == "y"); ++it;
                BOOST_TEST(it->key(sp.allocator()) == "y"); ++it;
                BOOST_TEST(it->key(sp.allocator()) == "z");
                it = qp.begin();
                BOOST_TEST(it->value(sp.allocator()) == "1"); ++it;
                BOOST_TEST(it->value(sp.allocator()) == "2"); ++it;
                BOOST_TEST(it->value(sp.allocator()) == "3"); ++it;
                BOOST_TEST(it->value(sp.allocator()) == "");
            }

            auto it = qp.begin();
            BOOST_TEST(it->encoded_key() == "x");
            it++;
            it++;
            BOOST_TEST(it->encoded_key() == "y");
            ++it;
            BOOST_TEST(it->encoded_key() == "z");
        }
    }

    void
    testFragment()
    {
        {
            auto u = parse_uri(
                "http://");
            BOOST_TEST(! u.has_fragment());
            BOOST_TEST(u.encoded_fragment() == "");
            BOOST_TEST(u.fragment() == "");
        }
        {
            auto u = parse_uri(
                "http://#");
            BOOST_TEST(u.has_fragment());
            BOOST_TEST(u.encoded_fragment() == "");
            BOOST_TEST(u.fragment() == "");
        }
        {
            auto u = parse_uri(
                "http://#x");
            BOOST_TEST(u.has_fragment());
            BOOST_TEST(u.encoded_fragment() == "x");
            BOOST_TEST(u.fragment() == "x");
        }
        {
            auto u = parse_uri(
                "http://#x%23");
            BOOST_TEST(u.has_fragment());
            BOOST_TEST(u.encoded_fragment() == "x%23");
            BOOST_TEST(u.fragment() == "x#");
        }
        {
            auto u = parse_uri(
                "http://#x%25");
            BOOST_TEST(u.has_fragment());
            BOOST_TEST(u.encoded_fragment() == "x%25");
            BOOST_TEST(u.fragment() == "x%");
        }
    }

    void
    testShared()
    {
        string_view s =
            "http://username:pass@www.boost.org:8080/x/y/z?a=b&c=3#frag";
        std::shared_ptr<url_view> sp;
        {
            auto const u = urls::parse_uri(s);
            sp = u.make_shared();
            BOOST_TEST(
                u.encoded_url().data() !=
                sp->encoded_url().data());
        }
        BOOST_TEST(sp->encoded_url() == s);
    }

    //--------------------------------------------

    void
    run()
    {
        testParse();
        testScheme();
        testAuthority();
        testUserinfo();
        testHost();
        testPort();
        testQuery();
        testFragment();
        testShared();
    }
};

TEST_SUITE(url_view_test, "boost.url.url_view");

} // urls
} // boost
