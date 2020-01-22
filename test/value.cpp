//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/value.hpp>

#include <boost/url/static_pool.hpp>

#include "test_suite.hpp"

/*

    https://nodejs.org/api/url.html
    https://medialize.github.io/URI.js/
    https://developer.mozilla.org/en-US/docs/Web/API/URL
    https://docs.microsoft.com/en-us/dotnet/api/system.uri?view=netframework-4.8
*/

namespace boost {
namespace url {

class value_test
{
public:
    test_suite::log_type log;

    void
    testConstValue()
    {
        BOOST_TEST(value().host() == host_type::none);
        BOOST_TEST(value("//").host() == host_type::none);
        BOOST_TEST(value("//127.0.0.1").host() == host_type::ipv4);
        BOOST_TEST(value("//0.0.0.0").host() == host_type::ipv4);
        BOOST_TEST(value("//255.255.255.255").host() == host_type::ipv4);
        BOOST_TEST(value("//0.0.0.").host() == host_type::name);
        BOOST_TEST(value("//127.00.0.1").host() == host_type::name);
        BOOST_TEST(value("//999.0.0.0").host() == host_type::name);
        BOOST_TEST(value("//example.com").host() == host_type::name);
        BOOST_TEST(value("//127.0.0.1.9").host() == host_type::name);

        {
            value const v("http://user:pass@example.com:80/path/to/file.txt?k1=v1&k2=v2");
            BOOST_TEST(v.encoded_url() == "http://user:pass@example.com:80/path/to/file.txt?k1=v1&k2=v2");
            BOOST_TEST(v.encoded_origin() == "http://user:pass@example.com:80");
            BOOST_TEST(v.encoded_authority() == "user:pass@example.com:80");
            BOOST_TEST(v.scheme() == "http");
            BOOST_TEST(v.encoded_username() == "user");
            BOOST_TEST(v.encoded_password() == "pass");
            BOOST_TEST(v.encoded_userinfo() == "user:pass");
            BOOST_TEST(v.encoded_hostname() == "example.com");
            BOOST_TEST(v.port_string() == "80");
            BOOST_TEST(*v.port() == 80);
            BOOST_TEST(v.encoded_path() == "/path/to/file.txt");
            BOOST_TEST(v.encoded_query() == "k1=v1&k2=v2");
            BOOST_TEST(v.encoded_fragment() == "");

            BOOST_TEST(v.username() == "user");
            BOOST_TEST(v.password() == "pass");
            BOOST_TEST(v.hostname() == "example.com");
            BOOST_TEST(v.query() == "k1=v1&k2=v2");
            BOOST_TEST(v.fragment() == "");
        }
    }

    void
    testIPv6()
    {
        BOOST_TEST(value("//[::]").host() == host_type::ipv6);
        BOOST_TEST(value("//[0000:0000:0000:0000:0000:0000:0000:0000]").host() == host_type::ipv6);
        BOOST_TEST(value("//[1234:5678:9ABC:DEF0:0000:0000:0000:0000]").host() == host_type::ipv6);
        BOOST_TEST(value("//[3FFE:1900:4545:3:200:F8FF:FE21:67CF]").host() == host_type::ipv6);
        BOOST_TEST(value("//[FE80:0:0:0:200:F8FF:FE21:67CF]").host() == host_type::ipv6);
        BOOST_TEST(value("//[2001:0DB8:0A0B:12F0:0000:0000:0000:0001]").host() == host_type::ipv6);
        BOOST_TEST(value("//[2001:DB8:3333:4444:5555:6666:7777:8888]").host() == host_type::ipv6);
        BOOST_TEST(value("//[2001:DB8:3333:4444:CCCC:DDDD:EEEE:FFFF]").host() == host_type::ipv6);
        BOOST_TEST(value("//[::]").host() == host_type::ipv6);
        BOOST_TEST(value("//[2001:DB8::]").host() == host_type::ipv6);
        BOOST_TEST(value("//[::1234:5678]").host() == host_type::ipv6);
        BOOST_TEST(value("//[2001:DB8::1234:5678]").host() == host_type::ipv6);
        BOOST_TEST(value("//[2001:0DB8:0001:0000:0000:0AB9:C0A8:0102]").host() == host_type::ipv6);
        BOOST_TEST(value("//[2001:DB8:1::AB9:C0A8:102]").host() == host_type::ipv6);
        BOOST_TEST(value("//[684D:1111:222:3333:4444:5555:6:77]").host() == host_type::ipv6);
        BOOST_TEST(value("//[0:0:0:0:0:0:0:0]").host() == host_type::ipv6);
            
        BOOST_TEST(value("//[::1:2:3:4:5]").host() == host_type::ipv6);
        BOOST_TEST(value("//[0:0:0:1:2:3:4:5]").host() == host_type::ipv6);
        BOOST_TEST(value("//[1:2::3:4:5]").host() == host_type::ipv6);
        BOOST_TEST(value("//[1:2:0:0:0:3:4:5]").host() == host_type::ipv6);
        BOOST_TEST(value("//[1:2:3:4:5::]").host() == host_type::ipv6);
        BOOST_TEST(value("//[1:2:3:4:5:0:0:0]").host() == host_type::ipv6);
        BOOST_TEST(value("//[0:0:0:0:0:FFFF:102:405]").host() == host_type::ipv6);
        BOOST_TEST(value("//[::]").host() == host_type::ipv6);
        BOOST_TEST(value("//[::0]").host() == host_type::ipv6);
        BOOST_TEST(value("//[::1]").host() == host_type::ipv6);
        BOOST_TEST(value("//[0:0:0::1]").host() == host_type::ipv6);
        BOOST_TEST(value("//[FFFF::1]").host() == host_type::ipv6);
        BOOST_TEST(value("//[FFFF:0:0:0:0:0:0:1]").host() == host_type::ipv6);
        BOOST_TEST(value("//[2001:0DB8:0A0B:12F0:0:0:0:1]").host() == host_type::ipv6);
        BOOST_TEST(value("//[2001:DB8:A0B:12F0::1]").host() == host_type::ipv6);

        BOOST_TEST(value("//[::FFFF:1.2.3.4]").host() == host_type::ipv6);
        BOOST_TEST(value("//[0:0:0:0:0:0:1.2.3.4]").host() == host_type::ipv6);
        BOOST_TEST(value("//[::1.2.3.4]").host() == host_type::ipv6);

        BOOST_TEST_THROWS(value("http://[0]"), invalid_part);
        BOOST_TEST_THROWS(value("//[0:1.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(value("//[0:0:0:0:0:0:0::1.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(value("http://[0:0:0:0:0:0:0:1.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(value("http://[::FFFF:999.2.3.4]"), invalid_part);

        // coverage
        BOOST_TEST_THROWS(value("//["), invalid_part);
        BOOST_TEST_THROWS(value("//[::"), invalid_part);
        BOOST_TEST_THROWS(value("//[0"), invalid_part);
        BOOST_TEST_THROWS(value("//[:"), invalid_part);
        BOOST_TEST_THROWS(value("//[::0::]"), invalid_part);
        BOOST_TEST_THROWS(value("//[:0::]"), invalid_part);
        BOOST_TEST_THROWS(value("//[0::0:x]"), invalid_part);
        BOOST_TEST_THROWS(value("//[x::]"), invalid_part);
        BOOST_TEST_THROWS(value("//[0:12"), invalid_part);
        BOOST_TEST_THROWS(value("//[0:123"), invalid_part);
        BOOST_TEST_THROWS(value("//[::1.]"), invalid_part);
        BOOST_TEST_THROWS(value("//[::1.2]"), invalid_part);
        BOOST_TEST_THROWS(value("//[::1.2"), invalid_part);
        BOOST_TEST_THROWS(value("//[::1.2x]"), invalid_part);
        BOOST_TEST_THROWS(value("//[::1.2.]"), invalid_part);
        BOOST_TEST_THROWS(value("//[::1.2.3"), invalid_part);
        BOOST_TEST_THROWS(value("//[::1.2.3]"), invalid_part);
        BOOST_TEST_THROWS(value("//[::1.2.3x]"), invalid_part);
        BOOST_TEST_THROWS(value("//[::1.2.3.]"), invalid_part);
        BOOST_TEST_THROWS(value("//[::1.2.3.4x]"), invalid_part);

        BOOST_TEST(value("//[1:2:3:4:5:6::7]").host() == host_type::ipv6);
        BOOST_TEST(value("//[1:2:3:4:5:6:7::]").host() == host_type::ipv6);
    }

    void
    testSegments()
    {
        // segments() const
        {
            value const v("/path/to/file.txt");
            auto ps = v.segments();
            static_assert(
                std::is_same<decltype(ps),
                    view::segments_type>::value, "");
        }
    }

    void
    testParams()
    {
        // params() const
        {
            value const v("?x=1&y=2&y=3&z");
            auto qp = v.params();
            static_assert(
                std::is_same<decltype(qp),
                    view::params_type>::value, "");
        }
    }

    //------------------------------------------------------

    void
    dump(value const& u)
    {
        (void)u;
#if 0
        log <<
            "href     : " << u.encoded_url() << "\n"
            "scheme   : " << u.scheme() << "\n"
            "user     : " << u.encoded_username() << "\n"
            "password : " << u.encoded_password() << "\n"
            "hostname : " << u.encoded_hostname() << "\n"
            "port     : " << u.port_string() << "\n" <<
            "path     : " << u.encoded_path() << "\n"
            "query    : " << u.encoded_query() << "\n"
            "fragment : " << u.encoded_fragment() << "\n"
            "resource : " << u.encoded_resource() << "\n"
            ;
        log.flush();
#endif
    }

    void
    testCtor()
    {
        BOOST_TEST(value().encoded_url() == "");
    }
  
    void
    testScheme()
    {
        BOOST_TEST(value().scheme() == "");
        BOOST_TEST(value("http:").scheme() == "http");
        BOOST_TEST(value("http:").encoded_url() == "http:");
        BOOST_TEST(value("http:").set_scheme("").scheme() == "");
        BOOST_TEST(value("http:").set_scheme("").encoded_url() == "");
        BOOST_TEST(value("http:").set_scheme("ftp").encoded_url() == "ftp:");
        BOOST_TEST(value("ws:").set_scheme("gopher").encoded_url() == "gopher:");
        BOOST_TEST(value("http://example.com").set_scheme("ftp").encoded_url() == "ftp://example.com");
        BOOST_TEST(value("ws://example.com").set_scheme("gopher").encoded_url() == "gopher://example.com");

        BOOST_TEST_THROWS(value().set_scheme("c@t"), invalid_part);
        BOOST_TEST_THROWS(value().set_scheme("1cat"), invalid_part);
        BOOST_TEST_THROWS(value().set_scheme("http:s"), invalid_part);
    }

    void
    testOrigin()
    {
        BOOST_TEST(value().encoded_origin() == "");
        BOOST_TEST(value("http://user:pass@example.com/path/to/file.txt?q").encoded_origin() ==
                         "http://user:pass@example.com");
        BOOST_TEST(value("http://user:pass@example.com/path/to/file.txt?q"
            ).set_encoded_origin("ws://x.com").encoded_url() == "ws://x.com/path/to/file.txt?q");
        BOOST_TEST(
            value("http://host:80/")
            .set_encoded_origin("http://host:443/")
            .port_string() == "443");
    }

    //------------------------------------------------------

    void
    testAuthority()
    {
        BOOST_TEST(value().encoded_authority() == "");
        BOOST_TEST(value("http://@").encoded_authority() == "@");
        BOOST_TEST(value("http://:@").encoded_authority() == ":@");
        BOOST_TEST(value("http://user@").encoded_authority() == "user@");
        BOOST_TEST(value("http://:pass@").encoded_authority() == ":pass@");
        BOOST_TEST(value("http://user:pass@").encoded_authority() == "user:pass@");
        BOOST_TEST(value("http://localhost").encoded_authority() == "localhost");
        BOOST_TEST(value("http://local%20host").encoded_authority() == "local%20host");
        BOOST_TEST(value("http://localhost:443").encoded_authority() == "localhost:443");
        BOOST_TEST(value("http://:443").encoded_authority() == ":443");
        BOOST_TEST(value("http://user:pass@example.com").encoded_authority() == "user:pass@example.com");
        BOOST_TEST(value("http://@").set_encoded_authority("user:pass@example.com").encoded_url() == "http://user:pass@example.com");
    }

    void
    testUserinfo()
    {
        BOOST_TEST(value().set_encoded_userinfo("").encoded_userinfo() == "");
        BOOST_TEST(value().set_encoded_userinfo("user:").encoded_userinfo() == "user:");
        BOOST_TEST(value().set_encoded_userinfo(":pass").encoded_userinfo() == ":pass");
        BOOST_TEST(value().set_encoded_userinfo("user:pass").encoded_userinfo() == "user:pass");

        BOOST_TEST(value("http://x:y@").set_encoded_userinfo("").encoded_userinfo() == "");
        BOOST_TEST(value("http://user:pass@").set_encoded_userinfo("user").encoded_userinfo() == "user");
        BOOST_TEST(value("http://user:pass@").set_encoded_userinfo(":pass").encoded_userinfo() == ":pass");
        BOOST_TEST(value("http://:pass@").set_encoded_userinfo("user").encoded_userinfo() == "user");
        BOOST_TEST(value("http://user:@").set_encoded_userinfo(":pass").encoded_userinfo() == ":pass");
        BOOST_TEST(value("http://z.com/").set_encoded_userinfo("").encoded_url() == "http://z.com/");
        BOOST_TEST(value("http://x:y@z.com").set_encoded_userinfo("").encoded_url() == "http://z.com");
    }

    void
    testUsername()
    {
        BOOST_TEST(value().username() == "");
        BOOST_TEST(value().encoded_username() == "");
        BOOST_TEST(value().set_username("").username() == "");
        BOOST_TEST(value().set_username("user").encoded_url() == "//user@");
        BOOST_TEST(value().set_encoded_username("user%20name").encoded_url() == "//user%20name@");
        BOOST_TEST(value().set_encoded_username("user%3Aname").encoded_url() == "//user%3Aname@");
        BOOST_TEST(value().set_encoded_username("user%3Aname").username() == "user:name");
        BOOST_TEST(value().set_encoded_username("user%40name").encoded_url() == "//user%40name@");
        BOOST_TEST(value().set_encoded_username("user%40name").username() == "user@name");

        BOOST_TEST(value("http:").set_encoded_username("").encoded_url() == "http:");
        BOOST_TEST(value("http://@").set_encoded_username("").encoded_url() == "http://");
        BOOST_TEST(value("http://x@").set_encoded_username("").encoded_url() == "http://");
        BOOST_TEST(value("http://x:@").set_encoded_username("").encoded_url() == "http://:@");
        BOOST_TEST(value("http://:@").set_username("").encoded_url() == "http://:@");
        BOOST_TEST(value("http://:y@").set_encoded_username("x").encoded_url() == "http://x:y@");
        BOOST_TEST(value("ws://user:pass@").set_username("x").encoded_url() == "ws://x:pass@");

        BOOST_TEST_THROWS(value().set_encoded_username("user:pass"), invalid_part);
        BOOST_TEST_THROWS(value().set_encoded_username("user name"), invalid_part);
    }

    void
    testPassword()
    {
        BOOST_TEST(value().password() == "");
        BOOST_TEST(value().encoded_password() == "");
        BOOST_TEST(value().set_encoded_password("").password() == "");
        BOOST_TEST(value().set_password("pass").encoded_url() == "//:pass@");
        BOOST_TEST(value().set_encoded_password("%40pass").encoded_url() == "//:%40pass@");
        BOOST_TEST(value().set_encoded_password("pass%20word").encoded_url() == "//:pass%20word@");
        BOOST_TEST(value().set_encoded_password("pass%42word").encoded_url() == "//:pass%42word@");

        BOOST_TEST(value("http:").set_encoded_password("").encoded_url() == "http:");
        BOOST_TEST(value("http://@").set_encoded_password("").encoded_url() == "http://");
        BOOST_TEST(value("http://x@").set_encoded_password("").encoded_url() == "http://x@");
        BOOST_TEST(value("http://x@").set_encoded_password("y").encoded_url() == "http://x:y@");
        BOOST_TEST(value("http://:@").set_encoded_password("").encoded_url() == "http://");
        BOOST_TEST(value("http://:y@").set_password("pass").encoded_url() == "http://:pass@");
        BOOST_TEST(value("http://x:y@").set_password("pass").encoded_url() == "http://x:pass@");
        BOOST_TEST(value("http://x:pass@").set_password("y").encoded_url() == "http://x:y@");
        BOOST_TEST(value("http://x:pass@example.com").set_password("y").encoded_url() == "http://x:y@example.com");

        BOOST_TEST_THROWS(value().set_encoded_password("pass word"), invalid_part);
        BOOST_TEST_THROWS(value().set_encoded_password(":pass"), invalid_part);
    }

    void
    testHost()
    {
        BOOST_TEST(value().set_encoded_host("example.com").encoded_url() == "//example.com");
        BOOST_TEST(value().set_encoded_host("x:1").encoded_url() == "//x:1");
        BOOST_TEST(value().set_encoded_host("x:1").set_encoded_host("").encoded_url() == "//");
        BOOST_TEST(value().set_encoded_host("x:1").set_encoded_host("example.com:443").encoded_url() == "//example.com:443");
        BOOST_TEST(value().set_encoded_host("local%20host%3A443").encoded_url() == "//local%20host%3A443");
        BOOST_TEST(! value().set_encoded_host("local%20host%3A443").port().has_value());

        BOOST_TEST(value().set_encoded_host(":").encoded_url() == "//:");
        BOOST_TEST(value().set_encoded_host(":").set_port({}).encoded_host() == "");
    }

    void
    testHostname()
    {
        BOOST_TEST(value().hostname() == "");
        BOOST_TEST(value().encoded_hostname() == "");
        BOOST_TEST(value().set_hostname("").encoded_url() == "");
        BOOST_TEST(value().set_hostname("localhost").hostname() == "localhost");
        BOOST_TEST(value().set_hostname("local host").hostname() == "local host");
        BOOST_TEST(value().set_hostname("local host").encoded_hostname() == "local%20host");
        BOOST_TEST(value().set_encoded_hostname("local%20host").hostname() == "local host");

        BOOST_TEST(value("http://x.com").set_encoded_hostname("localhost").encoded_url() == "http://localhost");
        BOOST_TEST(value("http://x.com").set_encoded_hostname("").encoded_url() == "http://");
        BOOST_TEST(value("http://@x.com").set_encoded_hostname("").encoded_url() == "http://@");
        BOOST_TEST(value("http://:@x.com").set_encoded_hostname("").encoded_url() == "http://:@");
        BOOST_TEST(value("http://:@x.com/").set_encoded_hostname("").encoded_url() == "http://:@/");

        BOOST_TEST_THROWS(value().set_encoded_hostname("local host"), invalid_part);
    }

    void
    testPort()
    {
        BOOST_TEST(! value().port().has_value());
        BOOST_TEST(value().port_string() == "");
        BOOST_TEST(value().set_port({}).port_string() == "");
        BOOST_TEST(value().set_port({}).encoded_url() == "");
        BOOST_TEST(value().set_port_string("80").port_string() == "80");
        BOOST_TEST(*value().set_port_string("80").port() == 80);
        BOOST_TEST(value().set_port_string("80").encoded_url() == "//:80");
        BOOST_TEST(value().set_port(80).encoded_url() == "//:80");
        BOOST_TEST(value("http://:80").set_port_string("").encoded_url() == "http://");
        BOOST_TEST(value("http://:80").set_port_string("").set_port(443).encoded_url() == "http://:443");
        BOOST_TEST(value("http://x.com:80").set_port_string("").encoded_url() == "http://x.com");
        BOOST_TEST(value("http://x.com:80").set_port(443).encoded_url() == "http://x.com:443");
        BOOST_TEST(value("http://x.com:80").set_port({}).port().has_value() == false);

        BOOST_TEST_THROWS(value().set_port_string("12bad"), invalid_part);
        BOOST_TEST_THROWS(value().set_port_string("9999999"), invalid_part);
    }

    //------------------------------------------------------

    void
    testPath()
    {
        BOOST_TEST(value("/path/to/file.txt").encoded_path() == "/path/to/file.txt");
        BOOST_TEST(value("http://example.com/path/to/file.txt?query").set_encoded_path("/a/b/c").encoded_path() == "/a/b/c");
#if 0
        BOOST_TEST(
            value("/path/to/file.txt")
            .set_encoded_segment(0, "p")
            .encoded_url() == "/p/to/file.txt");
        BOOST_TEST(
            value("/path/to/file.txt")
            .set_encoded_segment(1, "from")
            .encoded_url() == "/path/from/file.txt");
        BOOST_TEST(
            value("/path/to/file.txt")
            .set_encoded_segment(2, "index.htm")
            .encoded_url() == "/path/to/index.htm");
        BOOST_TEST(
            value("/path/to/file.txt")
            .encoded_segment(2) == "file.txt");
        BOOST_TEST(
            value("/path/to/file.txt")
            .encoded_segment(-1) == "file.txt");
        BOOST_TEST(
            value("/path/to/file.txt")
            .encoded_segment(-2) == "to");
        BOOST_TEST(
            value("/path/to/file.txt")
            .encoded_segment(-3) == "path");
        BOOST_TEST(
            value("/path/to/file.txt")
            .set_segment(0, "pa th")
            .encoded_url() == "/pa%20th/to/file.txt");
        BOOST_TEST_THROWS(
            value("/path/to/file.txt")
            .set_encoded_segment(0, "path/from"),
            invalid_part);
#endif

#if 0
        value u;
        u.set_scheme("http");
        u.set_encoded_userinfo("user:password");
        u.set_encoded_host("example.com:443");
        u.set_encoded_path("/");
        log << u.encoded_url() << std::endl;

        u.set_encoded_path("/path/to/file.txt");
        log << u.encoded_url() << std::endl;
        print(u.segments());
        u.set_segment(0, "pa th");
        log << u.encoded_url() << std::endl;
        u.set_segment(1, "teux");
        log << u.encoded_url() << std::endl;
#endif
    }

    //------------------------------------------------------

    void
    testQuery()
    {
        BOOST_TEST(value("").query() == "");
        BOOST_TEST(value("?").query() == "");
        BOOST_TEST(value("?x").query() == "x");

        BOOST_TEST(value("").encoded_query() == "");
        BOOST_TEST(value("?").encoded_query() == "");
        BOOST_TEST(value("?x").encoded_query() == "x");

        BOOST_TEST(value("").query_part() == "");
        BOOST_TEST(value("?").query_part() == "?");
        BOOST_TEST(value("?x").query_part() == "?x");

        BOOST_TEST(value().set_query("").query_part() == "");
        BOOST_TEST(value().set_query("?").query_part() == "??");
        BOOST_TEST(value().set_query("?x").query_part() == "??x");
        BOOST_TEST(value().set_query("#").query_part() == "?%23");

        BOOST_TEST(value().set_encoded_query("").query_part() == "");
        BOOST_TEST(value().set_encoded_query("x").query_part() == "?x");
        BOOST_TEST(value().set_encoded_query("?").query_part() == "??");
        BOOST_TEST(value().set_encoded_query("%23").query() == "#");
        BOOST_TEST_THROWS(value().set_encoded_query("#"), invalid_part);
        BOOST_TEST_THROWS(value().set_encoded_query("#x"), invalid_part);

        BOOST_TEST(value().set_query_part("").query_part() == "");
        BOOST_TEST(value().set_query_part("?").query_part() == "?");
        BOOST_TEST(value().set_query_part("?x").query_part() == "?x");
        BOOST_TEST(value().set_query_part("??x").query_part() == "??x");
        BOOST_TEST_THROWS(value().set_query_part("x"), invalid_part);
        BOOST_TEST_THROWS(value().set_query_part("%3F"), invalid_part);

        BOOST_TEST(value("//?").set_query("").encoded_url() == "//");
        BOOST_TEST(value("//?x").set_query("").encoded_url() == "//");
        BOOST_TEST(value("//?xy").set_query("y").encoded_url() == "//?y");
        BOOST_TEST(value("//").set_query("?").encoded_url() == "//??");
        BOOST_TEST(value("//").set_query("??").encoded_url() == "//???");

        BOOST_TEST(value("//?").set_encoded_query("").encoded_url() == "//");
        BOOST_TEST(value("//?x").set_encoded_query("").encoded_url() == "//");
        BOOST_TEST(value("//?xy").set_encoded_query("y").encoded_url() == "//?y");
        BOOST_TEST_THROWS(value("//").set_encoded_query("#"), invalid_part);
        BOOST_TEST_THROWS(value("//").set_encoded_query("#?"), invalid_part);

        BOOST_TEST(value("//?").set_query_part("").encoded_url() == "//");
        BOOST_TEST(value("//?x").set_query_part("").encoded_url() == "//");
        BOOST_TEST_THROWS(value("//?xy").set_query_part("y"), invalid_part);
        BOOST_TEST(value("//?xy").set_query_part("?y").encoded_url() == "//?y");
    }

    //------------------------------------------------------

    void
    testFragment()
    {
        BOOST_TEST(value("").fragment() == "");
        BOOST_TEST(value("#").fragment() == "");
        BOOST_TEST(value("#x").fragment() == "x");

        BOOST_TEST(value("").encoded_fragment() == "");
        BOOST_TEST(value("#").encoded_fragment() == "");
        BOOST_TEST(value("#x").encoded_fragment() == "x");

        BOOST_TEST(value("").fragment_part() == "");
        BOOST_TEST(value("#").fragment_part() == "#");
        BOOST_TEST(value("#x").fragment_part() == "#x");

        BOOST_TEST(value().set_fragment("").fragment_part() == "");
        BOOST_TEST(value().set_fragment("#").fragment_part() == "#%23");
        BOOST_TEST(value().set_fragment("#x").fragment_part() == "#%23x");

        BOOST_TEST(value().set_encoded_fragment("").fragment_part() == "");
        BOOST_TEST(value().set_encoded_fragment("x").fragment_part() == "#x");
        BOOST_TEST(value().set_encoded_fragment("%23").fragment_part() == "#%23");
        BOOST_TEST_THROWS(value().set_encoded_fragment("#"), invalid_part);
        BOOST_TEST_THROWS(value().set_encoded_fragment("#x"), invalid_part);

        BOOST_TEST(value().set_fragment_part("").fragment_part() == "");
        BOOST_TEST(value().set_fragment_part("#").fragment_part() == "#");
        BOOST_TEST(value().set_fragment_part("#x").fragment_part() == "#x");
        BOOST_TEST(value().set_fragment_part("#%23x").fragment_part() == "#%23x");
        BOOST_TEST_THROWS(value().set_fragment_part("x"), invalid_part);
        BOOST_TEST_THROWS(value().set_fragment_part("%23"), invalid_part);

        BOOST_TEST(value("//#").set_fragment("").encoded_url() == "//");
        BOOST_TEST(value("//#x").set_fragment("").encoded_url() == "//");
        BOOST_TEST(value("//#xy").set_fragment("y").encoded_url() == "//#y");
        BOOST_TEST(value("//").set_fragment("#").encoded_url() == "//#%23");
        BOOST_TEST(value("//").set_fragment("##").encoded_url() == "//#%23%23");

        BOOST_TEST(value("//#").set_encoded_fragment("").encoded_url() == "//");
        BOOST_TEST(value("//#x").set_encoded_fragment("").encoded_url() == "//");
        BOOST_TEST(value("//#xy").set_encoded_fragment("y").encoded_url() == "//#y");
        BOOST_TEST_THROWS(value("//").set_encoded_fragment("#"), invalid_part);
        BOOST_TEST_THROWS(value("//").set_encoded_fragment("##"), invalid_part);

        BOOST_TEST(value("//#").set_fragment_part("").encoded_url() == "//");
        BOOST_TEST(value("//#x").set_fragment_part("").encoded_url() == "//");
        BOOST_TEST_THROWS(value("//#xy").set_fragment_part("y"), invalid_part);
        BOOST_TEST(value("//#xy").set_fragment_part("#y").encoded_url() == "//#y");
    }

    //------------------------------------------------------

    void
    run()
    {
        testConstValue();
        testIPv6();
        testSegments();
        testParams();

        testCtor();
        testScheme();
        testOrigin();
        testAuthority();
        testUserinfo();
        testUsername();
        testPassword();
        testHostname();
        testPort();
        testHost();
        testPath();
        testQuery();
        testFragment();
    }
};

TEST_SUITE(value_test, "boost.url.value");

} // url
} // boost
