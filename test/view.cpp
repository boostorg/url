//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/view.hpp>
#include <boost/url/static_pool.hpp>

#include "test_suite.hpp"

namespace boost {
namespace url {

class view_test
{
public:
    test_suite::log_type log;

    void
    testView()
    {
        BOOST_TEST(view().host_type() == host_type::none);
        BOOST_TEST(view("//").host_type() == host_type::none);
        BOOST_TEST(view("//127.0.0.1").host_type() == host_type::ipv4);
        BOOST_TEST(view("//0.0.0.0").host_type() == host_type::ipv4);
        BOOST_TEST(view("//255.255.255.255").host_type() == host_type::ipv4);
        BOOST_TEST(view("//0.0.0.").host_type() == host_type::name);
        BOOST_TEST(view("//127.00.0.1").host_type() == host_type::name);
        BOOST_TEST(view("//999.0.0.0").host_type() == host_type::name);
        BOOST_TEST(view("//example.com").host_type() == host_type::name);
        BOOST_TEST(view("//127.0.0.1.9").host_type() == host_type::name);

        view const v("http://user:pass@example.com:80/path/to/file.txt?k1=v1&k2=v2");
        BOOST_TEST(v.encoded_url() == "http://user:pass@example.com:80/path/to/file.txt?k1=v1&k2=v2");
        BOOST_TEST(v.encoded_origin() == "http://user:pass@example.com:80");
        BOOST_TEST(v.encoded_authority() == "user:pass@example.com:80");
        BOOST_TEST(v.scheme() == "http");
        BOOST_TEST(v.encoded_username() == "user");
        BOOST_TEST(v.encoded_password() == "pass");
        BOOST_TEST(v.encoded_userinfo() == "user:pass");
        BOOST_TEST(v.encoded_host() == "example.com");
        BOOST_TEST(v.port_part() == ":80");
        BOOST_TEST(v.port() == "80");
        BOOST_TEST(v.encoded_host_and_port() == "example.com:80");
        BOOST_TEST(v.encoded_path() == "/path/to/file.txt");
        BOOST_TEST(v.encoded_query() == "k1=v1&k2=v2");
        BOOST_TEST(v.encoded_fragment() == "");

        BOOST_TEST(v.username() == "user");
        BOOST_TEST(v.password() == "pass");
        BOOST_TEST(v.host() == "example.com");
        BOOST_TEST(v.query() == "k1=v1&k2=v2");
        BOOST_TEST(v.fragment() == "");
    }

    //------------------------------------------------------

    void
    testIPv4()
    {
        BOOST_TEST(view().host_type() == host_type::none);
        BOOST_TEST(view("//0.0.0.0").host_type() == host_type::ipv4);
        BOOST_TEST(view("//255.255.255.255").host_type() == host_type::ipv4);
        BOOST_TEST(view("//255.255.255.255").host_type() == host_type::ipv4);
        BOOST_TEST(view("//256.255.255.255").host_type() == host_type::name);
        BOOST_TEST(view("//256.255.255.").host_type() == host_type::name);
        BOOST_TEST(view("//00.0.0.0").host_type() == host_type::name);
    }

    void
    testIPv6()
    {
        BOOST_TEST(view("//[::]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[0000:0000:0000:0000:0000:0000:0000:0000]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[1234:5678:9ABC:DEF0:0000:0000:0000:0000]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[3FFE:1900:4545:3:200:F8FF:FE21:67CF]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[FE80:0:0:0:200:F8FF:FE21:67CF]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[2001:0DB8:0A0B:12F0:0000:0000:0000:0001]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[2001:DB8:3333:4444:5555:6666:7777:8888]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[2001:DB8:3333:4444:CCCC:DDDD:EEEE:FFFF]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[::]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[2001:DB8::]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[::1234:5678]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[2001:DB8::1234:5678]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[2001:0DB8:0001:0000:0000:0AB9:C0A8:0102]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[2001:DB8:1::AB9:C0A8:102]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[684D:1111:222:3333:4444:5555:6:77]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[0:0:0:0:0:0:0:0]").host_type() == host_type::ipv6);
            
        BOOST_TEST(view("//[::1:2:3:4:5]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[0:0:0:1:2:3:4:5]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[1:2::3:4:5]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[1:2:0:0:0:3:4:5]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[1:2:3:4:5::]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[1:2:3:4:5:0:0:0]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[0:0:0:0:0:FFFF:102:405]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[::]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[::0]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[::1]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[0:0:0::1]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[FFFF::1]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[FFFF:0:0:0:0:0:0:1]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[2001:0DB8:0A0B:12F0:0:0:0:1]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[2001:DB8:A0B:12F0::1]").host_type() == host_type::ipv6);

        BOOST_TEST(view("//[::FFFF:1.2.3.4]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[0:0:0:0:0:0:1.2.3.4]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[::1.2.3.4]").host_type() == host_type::ipv6);

        BOOST_TEST_THROWS(view("http://[0]"), invalid_part);
        BOOST_TEST_THROWS(view("//[0:1.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(view("//[0:0:0:0:0:0:0::1.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(view("http://[0:0:0:0:0:0:0:1.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(view("http://[::FFFF:999.2.3.4]"), invalid_part);

        // coverage
        BOOST_TEST_THROWS(view("//["), invalid_part);
        BOOST_TEST_THROWS(view("//[::"), invalid_part);
        BOOST_TEST_THROWS(view("//[0"), invalid_part);
        BOOST_TEST_THROWS(view("//[:"), invalid_part);
        BOOST_TEST_THROWS(view("//[::0::]"), invalid_part);
        BOOST_TEST_THROWS(view("//[:0::]"), invalid_part);
        BOOST_TEST_THROWS(view("//[0::0:x]"), invalid_part);
        BOOST_TEST_THROWS(view("//[x::]"), invalid_part);
        BOOST_TEST_THROWS(view("//[0:12"), invalid_part);
        BOOST_TEST_THROWS(view("//[0:123"), invalid_part);
        BOOST_TEST_THROWS(view("//[::1.]"), invalid_part);
        BOOST_TEST_THROWS(view("//[::1.2]"), invalid_part);
        BOOST_TEST_THROWS(view("//[::1.2"), invalid_part);
        BOOST_TEST_THROWS(view("//[::1.2x]"), invalid_part);
        BOOST_TEST_THROWS(view("//[::1.2.]"), invalid_part);
        BOOST_TEST_THROWS(view("//[::1.2.3"), invalid_part);
        BOOST_TEST_THROWS(view("//[::1.2.3]"), invalid_part);
        BOOST_TEST_THROWS(view("//[::1.2.3x]"), invalid_part);
        BOOST_TEST_THROWS(view("//[::1.2.3.]"), invalid_part);
        BOOST_TEST_THROWS(view("//[::1.2.3.4x]"), invalid_part);

        BOOST_TEST(view("//[1:2:3:4:5:6::7]").host_type() == host_type::ipv6);
        BOOST_TEST(view("//[1:2:3:4:5:6:7::]").host_type() == host_type::ipv6);
    }

    void
    testHost()
    {
        BOOST_TEST(view().host() == "");
        BOOST_TEST(view("//?#").host() == "");
        BOOST_TEST(view("//x?#").host() == "x");
        BOOST_TEST(view("//%2F").host() == "/");
        BOOST_TEST(view("//%2F?#").host() == "/");

        BOOST_TEST(view().encoded_host() == "");
        BOOST_TEST(view("//?#").encoded_host() == "");
        BOOST_TEST(view("//x?#").encoded_host() == "x");
        BOOST_TEST(view("//%2F").encoded_host() == "%2F");
        BOOST_TEST(view("//%2F?#").encoded_host() == "%2F");

        testIPv4();
        testIPv6();
    }

    void
    testPort()
    {
        BOOST_TEST(view().port() == "");
        BOOST_TEST(view().port_part() == "");
        BOOST_TEST(view("//x:/").port() == "");
        BOOST_TEST(view("//x:/").port_part() == ":");
        BOOST_TEST(view("//x:80/").port() == "80");
        BOOST_TEST(view("//x:80/").port_part() == ":80");
    }

    void
    testHostAndPort()
    {
        BOOST_TEST(view().encoded_host_and_port() == "");
        BOOST_TEST(view("//").encoded_host_and_port() == "");
        BOOST_TEST(view("//x").encoded_host_and_port() == "x");
        BOOST_TEST(view("//x:").encoded_host_and_port() == "x:");
        BOOST_TEST(view("//x:0").encoded_host_and_port() == "x:0");
        BOOST_TEST(view("//x:0/").encoded_host_and_port() == "x:0");
    }

    //------------------------------------------------------

    void
    testPath()
    {
        BOOST_TEST(view().encoded_path() == "");
        BOOST_TEST(view("x:a").encoded_path() == "a");
        BOOST_TEST(view("x:/a").encoded_path() == "/a");
        BOOST_TEST(view("x://y/a").encoded_path() == "/a");

        BOOST_TEST(view("x").encoded_path() == "x");
        BOOST_TEST(view("x/").encoded_path() == "x/");
        BOOST_TEST(view("x//").encoded_path() == "x//");

        BOOST_TEST(view("/").encoded_path() == "/");

        testSegments();
    }

    void
    testSegments()
    {
        {
            view::segments_type const ps;
            BOOST_TEST(ps.empty());
            BOOST_TEST(ps.size() == 0);
            BOOST_TEST(ps.begin() == ps.end());
            BOOST_TEST(
                view::segments_type::iterator() ==
                view::segments_type::iterator());
        }
        {
            view const v("/path/to/file.txt");
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

    //------------------------------------------------------

    void
    testQuery()
    {
        BOOST_TEST(view("").query() == "");
        BOOST_TEST(view("?").query() == "");
        BOOST_TEST(view("?x").query() == "x");

        BOOST_TEST(view("").encoded_query() == "");
        BOOST_TEST(view("?").encoded_query() == "");
        BOOST_TEST(view("?x").encoded_query() == "x");

        BOOST_TEST(view("").query_part() == "");
        BOOST_TEST(view("?").query_part() == "?");
        BOOST_TEST(view("?x").query_part() == "?x");

        testParams();
    }

    void
    testParams()
    {
        {
            view::params_type const qp;
            BOOST_TEST(qp.empty());
            BOOST_TEST(qp.size() == 0);
            BOOST_TEST(qp.begin() == qp.end());
            BOOST_TEST(! qp.contains("x"));
            BOOST_TEST(qp.count("x") == 0);
            BOOST_TEST(qp.find("x") == qp.end());
            BOOST_TEST_THROWS(qp.at("x"), out_of_range);

            BOOST_TEST(
                view::params_type::iterator() ==
                view::params_type::iterator());
        }
        {
            view const v("?x=1&y=2&y=3&z");
            auto qp = v.params();
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
            --it;
            BOOST_TEST(it->encoded_key() == "y");
            it--;
            it--;
            BOOST_TEST(it->encoded_key() == "x");
        }
    }

    //------------------------------------------------------

    void
    testFragment()
    {
        BOOST_TEST(view("").fragment() == "");
        BOOST_TEST(view("#").fragment() == "");
        BOOST_TEST(view("#x").fragment() == "x");

        BOOST_TEST(view("").encoded_fragment() == "");
        BOOST_TEST(view("#").encoded_fragment() == "");
        BOOST_TEST(view("#x").encoded_fragment() == "x");

        BOOST_TEST(view("").fragment_part() == "");
        BOOST_TEST(view("#").fragment_part() == "#");
        BOOST_TEST(view("#x").fragment_part() == "#x");
    }

    void
    run()
    {
        testView();

        testHost();
        testPort();
        testHostAndPort();
        testPath();
        testQuery();
        testFragment();
    }
};

TEST_SUITE(view_test, "boost.url.view");

} // url
} // boost
