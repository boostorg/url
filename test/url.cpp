//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

// Test that header file is self-contained.
#include <boost/url/url.hpp>

#include <boost/url/static_pool.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

class url_test
{
public:
    test_suite::log_type log;

    void
    dump(url const& u)
    {
        (void)u;
        log <<
            "href     : " << u.encoded_url() << "\n"
            "scheme   : " << u.scheme() << "\n"
            "user     : " << u.encoded_user() << "\n"
            "password : " << u.encoded_password() << "\n"
            "hostname : " << u.encoded_host() << "\n"
            "port     : " << u.port_part() << "\n" <<
            "path     : " << u.encoded_path() << "\n"
            "query    : " << u.query_part() << "\n"
            "fragment : " << u.fragment_part() << "\n"
            //"resource : " << u.encoded_resource() << "\n"
            ;
        log.flush();
    }

    void
    testObservers()
    {
        BOOST_TEST(url("/").size() == 1);
        BOOST_TEST(string_view(url("/").data()) == "/");
        BOOST_TEST(url("/").capacity() >= 1);
    }

    void
    testConstValue()
    {
        BOOST_TEST(url().host_type() == host_type::none);
        BOOST_TEST(url("//").host_type() == host_type::none);
        BOOST_TEST(url("//127.0.0.1").host_type() == host_type::ipv4);
        BOOST_TEST(url("//0.0.0.0").host_type() == host_type::ipv4);
        BOOST_TEST(url("//255.255.255.255").host_type() == host_type::ipv4);
        BOOST_TEST(url("//0.0.0.").host_type() == host_type::name);
        BOOST_TEST(url("//127.00.0.1").host_type() == host_type::name);
        BOOST_TEST(url("//999.0.0.0").host_type() == host_type::name);
        BOOST_TEST(url("//example.com").host_type() == host_type::name);
        BOOST_TEST(url("//127.0.0.1.9").host_type() == host_type::name);

        {
            url const v("http://user:pass@example.com:80/path/to/file.txt?k1=v1&k2=v2");
            BOOST_TEST(v.encoded_url() == "http://user:pass@example.com:80/path/to/file.txt?k1=v1&k2=v2");
            BOOST_TEST(v.encoded_origin() == "http://user:pass@example.com:80");
            BOOST_TEST(v.encoded_authority() == "user:pass@example.com:80");
            BOOST_TEST(v.scheme() == "http");
            BOOST_TEST(v.encoded_user() == "user");
            BOOST_TEST(v.encoded_password() == "pass");
            BOOST_TEST(v.encoded_userinfo() == "user:pass");
            BOOST_TEST(v.encoded_host() == "example.com");
            BOOST_TEST(v.port_part() == ":80");
            BOOST_TEST(v.port() == "80");
            BOOST_TEST(v.encoded_path() == "/path/to/file.txt");
            BOOST_TEST(v.encoded_query() == "k1=v1&k2=v2");
            BOOST_TEST(v.encoded_fragment() == "");

            BOOST_TEST(v.user() == "user");
            BOOST_TEST(v.password() == "pass");
            BOOST_TEST(v.host() == "example.com");
            BOOST_TEST(v.query() == "k1=v1&k2=v2");
            BOOST_TEST(v.fragment() == "");
        }
    }

    //------------------------------------------------------

    void
    testCtor()
    {
        BOOST_TEST(url().encoded_url() == "");
    }

    void
    testScheme()
    {
        BOOST_TEST(url().scheme() == "");
        BOOST_TEST(url("http:").scheme() == "http");
        BOOST_TEST(url("http:").encoded_url() == "http:");
        BOOST_TEST(url("http:").set_scheme("").scheme() == "");
        BOOST_TEST(url("http:").set_scheme("").encoded_url() == "");
        BOOST_TEST(url("http:").set_scheme("ftp").encoded_url() == "ftp:");
        BOOST_TEST(url("ws:").set_scheme("gopher").encoded_url() == "gopher:");
        BOOST_TEST(url("http://example.com").set_scheme("ftp").encoded_url() == "ftp://example.com");
        BOOST_TEST(url("ws://example.com").set_scheme("gopher").encoded_url() == "gopher://example.com");

        BOOST_TEST_THROWS(url().set_scheme("c@t"), invalid_part);
        BOOST_TEST_THROWS(url().set_scheme("1cat"), invalid_part);
        BOOST_TEST_THROWS(url().set_scheme("http:s"), invalid_part);
    }

    void
    testOrigin()
    {
        BOOST_TEST(url().encoded_origin() == "");
        BOOST_TEST(url("http://user:pass@example.com/path/to/file.txt?q").encoded_origin() ==
                         "http://user:pass@example.com");
        BOOST_TEST(url("http://user:pass@example.com/path/to/file.txt?q"
            ).set_encoded_origin("ws://x.com").encoded_url() == "ws://x.com/path/to/file.txt?q");
        BOOST_TEST(
            url("http://host:80/")
            .set_encoded_origin("http://host:443/")
            .port() == "443");
    }

    //------------------------------------------------------

    void
    testAuthority()
    {
        BOOST_TEST(url().encoded_authority() == "");
        BOOST_TEST(url("http://@").encoded_authority() == "@");
        BOOST_TEST(url("http://:@").encoded_authority() == ":@");
        BOOST_TEST(url("http://user@").encoded_authority() == "user@");
        BOOST_TEST(url("http://:pass@").encoded_authority() == ":pass@");
        BOOST_TEST(url("http://user:pass@").encoded_authority() == "user:pass@");
        BOOST_TEST(url("http://localhost").encoded_authority() == "localhost");
        BOOST_TEST(url("http://local%20host").encoded_authority() == "local%20host");
        BOOST_TEST(url("http://localhost:443").encoded_authority() == "localhost:443");
        BOOST_TEST(url("http://:443").encoded_authority() == ":443");
        BOOST_TEST(url("http://user:pass@example.com").encoded_authority() == "user:pass@example.com");
        BOOST_TEST(url("http://@").set_encoded_authority("user:pass@example.com").encoded_url() == "http://user:pass@example.com");
    }

    void
    testUsername()
    {
        BOOST_TEST(url().user() == "");
        BOOST_TEST(url().encoded_user() == "");
        BOOST_TEST(url().set_user("").user() == "");
        BOOST_TEST(url().set_user("user").encoded_url() == "//user@");
        BOOST_TEST(url().set_encoded_user("user%20name").encoded_url() == "//user%20name@");
        BOOST_TEST(url().set_encoded_user("user%3Aname").encoded_url() == "//user%3Aname@");
        BOOST_TEST(url().set_encoded_user("user%3Aname").user() == "user:name");
        BOOST_TEST(url().set_encoded_user("user%40name").encoded_url() == "//user%40name@");
        BOOST_TEST(url().set_encoded_user("user%40name").user() == "user@name");

        BOOST_TEST(url("http:").set_encoded_user("").encoded_url() == "http:");
        BOOST_TEST(url("http://@").set_encoded_user("").encoded_url() == "http://");
        BOOST_TEST(url("http://x@").set_encoded_user("").encoded_url() == "http://");
        BOOST_TEST(url("http://x:@").set_encoded_user("").encoded_url() == "http://:@");
        BOOST_TEST(url("http://:@").set_user("").encoded_url() == "http://:@");
        BOOST_TEST(url("http://:y@").set_encoded_user("x").encoded_url() == "http://x:y@");
        BOOST_TEST(url("ws://user:pass@").set_user("x").encoded_url() == "ws://x:pass@");

        BOOST_TEST_THROWS(url().set_encoded_user("user:pass"), invalid_part);
        BOOST_TEST_THROWS(url().set_encoded_user("user name"), invalid_part);
    }

    void
    testPassword()
    {
        BOOST_TEST(url().password() == "");
        BOOST_TEST(url().encoded_password() == "");
        BOOST_TEST(url().set_encoded_password("").password() == "");
        BOOST_TEST(url().set_password("pass").encoded_url() == "//:pass@");
        BOOST_TEST(url().set_encoded_password("%40pass").encoded_url() == "//:%40pass@");
        BOOST_TEST(url().set_encoded_password("pass%20word").encoded_url() == "//:pass%20word@");
        BOOST_TEST(url().set_encoded_password("pass%42word").encoded_url() == "//:pass%42word@");

        BOOST_TEST(url("http:").set_encoded_password("").encoded_url() == "http:");
        BOOST_TEST(url("http://@").set_encoded_password("").encoded_url() == "http://");
        BOOST_TEST(url("http://x@").set_encoded_password("").encoded_url() == "http://x@");
        BOOST_TEST(url("http://x@").set_encoded_password("y").encoded_url() == "http://x:y@");
        BOOST_TEST(url("http://:@").set_encoded_password("").encoded_url() == "http://");
        BOOST_TEST(url("http://:y@").set_password("pass").encoded_url() == "http://:pass@");
        BOOST_TEST(url("http://x:y@").set_password("pass").encoded_url() == "http://x:pass@");
        BOOST_TEST(url("http://x:pass@").set_password("y").encoded_url() == "http://x:y@");
        BOOST_TEST(url("http://x:pass@example.com").set_password("y").encoded_url() == "http://x:y@example.com");

        BOOST_TEST_THROWS(url().set_encoded_password("pass word"), invalid_part);
        BOOST_TEST_THROWS(url().set_encoded_password(":pass"), invalid_part);
    }

    //------------------------------------------------------

    void
    testUserinfo()
    {
        BOOST_TEST(url().encoded_userinfo() == "");
        BOOST_TEST(url("//x/").encoded_userinfo() == "");
        BOOST_TEST(url("//x@/").encoded_userinfo() == "x");
        BOOST_TEST(url("//x:@/").encoded_userinfo() == "x:");
        BOOST_TEST(url("//x:y@/").encoded_userinfo() == "x:y");
        BOOST_TEST(url("//:y@/").encoded_userinfo() == ":y");
        BOOST_TEST(url("//:@/").encoded_userinfo() == ":");
        BOOST_TEST(url("//@/").encoded_userinfo() == "");

        BOOST_TEST(url().userinfo_part() == "");
        BOOST_TEST(url("//x/").userinfo_part() == "");
        BOOST_TEST(url("//x@/").userinfo_part() == "x@");
        BOOST_TEST(url("//x:@/").userinfo_part() == "x:@");
        BOOST_TEST(url("//x:y@/").userinfo_part() == "x:y@");
        BOOST_TEST(url("//:y@/").userinfo_part() == ":y@");
        BOOST_TEST(url("//:@/").userinfo_part() == ":@");
        BOOST_TEST(url("//@/").userinfo_part() == "@");

        BOOST_TEST(url().set_encoded_userinfo("").encoded_url() == "");
        BOOST_TEST(url().set_encoded_userinfo("user:").encoded_url() == "//user:@");
        BOOST_TEST(url().set_encoded_userinfo(":pass").encoded_url() == "//:pass@");
        BOOST_TEST(url().set_encoded_userinfo("user:pass").encoded_url() == "//user:pass@");
        BOOST_TEST(url().set_encoded_userinfo("user%3Apass").encoded_url() == "//user%3Apass@");
        BOOST_TEST_THROWS(url().set_encoded_userinfo("user%3pass"), invalid_part);

        BOOST_TEST(url().set_userinfo_part("").encoded_url() == "");
        BOOST_TEST(url().set_userinfo_part("user:@").encoded_url() == "//user:@");
        BOOST_TEST(url().set_userinfo_part(":pass@").encoded_url() == "//:pass@");
        BOOST_TEST(url().set_userinfo_part("user:pass@").encoded_url() == "//user:pass@");
        BOOST_TEST(url().set_userinfo_part("user%3Apass@").encoded_url() == "//user%3Apass@");
        BOOST_TEST_THROWS(url().set_userinfo_part("user%3Apass"), invalid_part);

        BOOST_TEST(url("http://x:y@/").set_encoded_userinfo("").encoded_url() == "http:/");
        BOOST_TEST(url("http://user:pass@/").set_encoded_userinfo("user").encoded_url() == "http://user@/");
        BOOST_TEST(url("http://user:pass@/").set_encoded_userinfo(":pass").encoded_url() == "http://:pass@/");
        BOOST_TEST(url("http://:pass@/").set_encoded_userinfo("user").encoded_url() == "http://user@/");
        BOOST_TEST(url("http://user:@/").set_encoded_userinfo(":pass").encoded_url() == "http://:pass@/");
        BOOST_TEST(url("http://z.com/").set_encoded_userinfo("").encoded_url() == "http://z.com/");
        BOOST_TEST(url("http://x:y@z.com/").set_encoded_userinfo("").encoded_url() == "http://z.com/");
    }

    void
    testUser()
    {
        BOOST_TEST(url().user() == "");
        BOOST_TEST(url("//x/").user() == "");
        BOOST_TEST(url("//x@/").user() == "x");
        BOOST_TEST(url("//x:@/").user() == "x");
        BOOST_TEST(url("//x:y@/").user() == "x");
        BOOST_TEST(url("//:y@/").user() == "");
        BOOST_TEST(url("//:@/").user() == "");
        BOOST_TEST(url("//@/").user() == "");
        BOOST_TEST(url("//%3A@/").user() == ":");

        BOOST_TEST(url().encoded_user() == "");
        BOOST_TEST(url("//x/").encoded_user() == "");
        BOOST_TEST(url("//x@/").encoded_user() == "x");
        BOOST_TEST(url("//x:@/").encoded_user() == "x");
        BOOST_TEST(url("//x:y@/").encoded_user() == "x");
        BOOST_TEST(url("//:y@/").encoded_user() == "");
        BOOST_TEST(url("//:@/").encoded_user() == "");
        BOOST_TEST(url("//@/").encoded_user() == "");
        BOOST_TEST(url("//%3A@/").encoded_user() == "%3A");

        BOOST_TEST(url("").set_user("").encoded_url() == "");
        BOOST_TEST(url("").set_user("x").encoded_url() == "//x@");
        BOOST_TEST(url("").set_user("x:").encoded_url() == "//x%3A@");
        BOOST_TEST(url("").set_user("x:y").encoded_url() == "//x%3Ay@");
        BOOST_TEST(url("//yy@").set_user("x").encoded_url() == "//x@");
        BOOST_TEST(url("//:@").set_user("x").encoded_url() == "//x:@");
        BOOST_TEST(url("//:p@").set_user("x").encoded_url() == "//x:p@");
        //BOOST_TEST(url("//yy@").set_user("").encoded_url() == "");
        BOOST_TEST(url("//:p@").set_user("x").encoded_url() == "//x:p@");
        BOOST_TEST(url("//yy:p@").set_user("x").encoded_url() == "//x:p@");
    }

    //------------------------------------------------------

    void
    testHostAndPort()
    {
        BOOST_TEST(url().encoded_host_and_port() == "");
        BOOST_TEST(url("//").encoded_host_and_port() == "");
        BOOST_TEST(url("//x").encoded_host_and_port() == "x");
        BOOST_TEST(url("//x:").encoded_host_and_port() == "x:");
        BOOST_TEST(url("//x:0").encoded_host_and_port() == "x:0");
        BOOST_TEST(url("//x:0/").encoded_host_and_port() == "x:0");
    }

    void
    testIPv4()
    {
        BOOST_TEST(url().host_type() == host_type::none);
        BOOST_TEST(url("//0.0.0.0").host_type() == host_type::ipv4);
        BOOST_TEST(url("//255.255.255.255").host_type() == host_type::ipv4);
        BOOST_TEST(url("//255.255.255.255").host_type() == host_type::ipv4);
        BOOST_TEST(url("//256.255.255.255").host_type() == host_type::name);
        BOOST_TEST(url("//256.255.255.").host_type() == host_type::name);
        BOOST_TEST(url("//00.0.0.0").host_type() == host_type::name);
        BOOST_TEST(url("//1").host_type() == host_type::name);
        BOOST_TEST(url("//1.").host_type() == host_type::name);
        BOOST_TEST(url("//1.2").host_type() == host_type::name);
        BOOST_TEST(url("//1.2.").host_type() == host_type::name);
        BOOST_TEST(url("//1.2.3").host_type() == host_type::name);
        BOOST_TEST(url("//1.2.3.").host_type() == host_type::name);
    }

    void
    testIPv6()
    {
        BOOST_TEST(url("//[::]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[0000:0000:0000:0000:0000:0000:0000:0000]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[1234:5678:9ABC:DEF0:0000:0000:0000:0000]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[3FFE:1900:4545:3:200:F8FF:FE21:67CF]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[FE80:0:0:0:200:F8FF:FE21:67CF]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[2001:0DB8:0A0B:12F0:0000:0000:0000:0001]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[2001:DB8:3333:4444:5555:6666:7777:8888]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[2001:DB8:3333:4444:CCCC:DDDD:EEEE:FFFF]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[::]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[2001:DB8::]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[::1234:5678]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[2001:DB8::1234:5678]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[2001:0DB8:0001:0000:0000:0AB9:C0A8:0102]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[2001:DB8:1::AB9:C0A8:102]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[684D:1111:222:3333:4444:5555:6:77]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[0:0:0:0:0:0:0:0]").host_type() == host_type::ipv6);

        BOOST_TEST(url("//[::1:2:3:4:5]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[0:0:0:1:2:3:4:5]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[1:2::3:4:5]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[1:2:0:0:0:3:4:5]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[1:2:3:4:5::]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[1:2:3:4:5:0:0:0]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[0:0:0:0:0:FFFF:102:405]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[::]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[::0]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[::1]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[0:0:0::1]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[FFFF::1]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[FFFF:0:0:0:0:0:0:1]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[2001:0DB8:0A0B:12F0:0:0:0:1]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[2001:DB8:A0B:12F0::1]").host_type() == host_type::ipv6);

        BOOST_TEST(url("//[::FFFF:1.2.3.4]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[0:0:0:0:0:0:1.2.3.4]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[::1.2.3.4]").host_type() == host_type::ipv6);
        BOOST_TEST_THROWS(url("//[::1A0.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(url("//[::10A.2.3.4]"), invalid_part);

        BOOST_TEST_THROWS(url("http://[0]"), invalid_part);
        BOOST_TEST_THROWS(url("//[0:1.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(url("//[0:0:0:0:0:0:0::1.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(url("http://[0:0:0:0:0:0:0:1.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(url("http://[::FFFF:999.2.3.4]"), invalid_part);
        BOOST_TEST_THROWS(url("//[0:"), invalid_part);

        // coverage
        BOOST_TEST_THROWS(url("//["), invalid_part);
        BOOST_TEST_THROWS(url("//[::"), invalid_part);
        BOOST_TEST_THROWS(url("//[0"), invalid_part);
        BOOST_TEST_THROWS(url("//[:"), invalid_part);
        BOOST_TEST_THROWS(url("//[::0::]"), invalid_part);
        BOOST_TEST_THROWS(url("//[:0::]"), invalid_part);
        BOOST_TEST_THROWS(url("//[0::0:x]"), invalid_part);
        BOOST_TEST_THROWS(url("//[x::]"), invalid_part);
        BOOST_TEST_THROWS(url("//[0:12"), invalid_part);
        BOOST_TEST_THROWS(url("//[0:123"), invalid_part);
        BOOST_TEST_THROWS(url("//[::1.]"), invalid_part);
        BOOST_TEST_THROWS(url("//[::1.2]"), invalid_part);
        BOOST_TEST_THROWS(url("//[::1.2"), invalid_part);
        BOOST_TEST_THROWS(url("//[::1.2x]"), invalid_part);
        BOOST_TEST_THROWS(url("//[::1.2.]"), invalid_part);
        BOOST_TEST_THROWS(url("//[::1.2.3"), invalid_part);
        BOOST_TEST_THROWS(url("//[::1.2.3]"), invalid_part);
        BOOST_TEST_THROWS(url("//[::1.2.3x]"), invalid_part);
        BOOST_TEST_THROWS(url("//[::1.2.3.]"), invalid_part);
        BOOST_TEST_THROWS(url("//[::1.2.3.4x]"), invalid_part);

        BOOST_TEST(url("//[1:2:3:4:5:6::7]").host_type() == host_type::ipv6);
        BOOST_TEST(url("//[1:2:3:4:5:6:7::]").host_type() == host_type::ipv6);
    }

    void
    testHost()
    {
        BOOST_TEST(url().host() == "");
        BOOST_TEST(url("//?#").host() == "");
        BOOST_TEST(url("//x?#").host() == "x");
        BOOST_TEST(url("//%2F").host() == "/");
        BOOST_TEST(url("//%2F?#").host() == "/");

        BOOST_TEST(url().encoded_host() == "");
        BOOST_TEST(url("//?#").encoded_host() == "");
        BOOST_TEST(url("//x?#").encoded_host() == "x");
        BOOST_TEST(url("//%2F").encoded_host() == "%2F");
        BOOST_TEST(url("//%2F?#").encoded_host() == "%2F");

        BOOST_TEST(url().set_host("x").encoded_url() == "//x");
        BOOST_TEST(url().set_host("local host").encoded_url() == "//local%20host");
        BOOST_TEST(url("z://").set_host("x").encoded_url() == "z://x");
        BOOST_TEST(url("z://x/").set_host("yy").encoded_url() == "z://yy/");
        BOOST_TEST(url("z://yy/").set_host("x").encoded_url() == "z://x/");
        BOOST_TEST(url("z://yy/").set_host("").encoded_url() == "z:/");
        BOOST_TEST(url("z://yy:80/").set_host("").encoded_url() == "z://:80/");
        BOOST_TEST(url("z://@").set_host("x").encoded_url() == "z://@x");
        BOOST_TEST(url("z://@yy").set_host("x").encoded_url() == "z://@x");
        BOOST_TEST(url("z://@yy").set_host("").encoded_url() == "z://@");
        BOOST_TEST(url("z://:@").set_host("x").encoded_url() == "z://:@x");
        BOOST_TEST(url("z://:@yy").set_host("").encoded_url() == "z://:@");
        BOOST_TEST(url("z:").set_host("yy").host_type() == host_type::name);
        BOOST_TEST(url("z:").set_host("1.2.3.4").encoded_url() == "z://1.2.3.4");
        BOOST_TEST(url("z://x").set_host("1.2.3.4").host_type() == host_type::ipv4);
        BOOST_TEST(url("z://x").set_host("1.2.3.4x").host_type() == host_type::name);
        BOOST_TEST(url("z://x").set_host("[::]").host_type() == host_type::ipv6);
        BOOST_TEST(url("z://x").set_host("[::]").encoded_host() == "[::]");
        BOOST_TEST(url("z://x").set_host("[::]x").host_type() == host_type::name);
        BOOST_TEST(url("z://x").set_host("[::]x").encoded_host() == "%5B%3A%3A%5Dx");
        BOOST_TEST(url("z://x").set_host("[::").encoded_host() == "%5B%3A%3A");

        BOOST_TEST(url().set_encoded_host("x").encoded_url() == "//x");
        BOOST_TEST(url().set_encoded_host("local%20host").host() == "local host");
        BOOST_TEST(url("z://").set_encoded_host("x").encoded_url() == "z://x");
        BOOST_TEST(url("z://x/").set_encoded_host("yy").encoded_url() == "z://yy/");
        BOOST_TEST(url("z://yy/").set_encoded_host("x").encoded_url() == "z://x/");
        BOOST_TEST(url("z://yy/").set_encoded_host("").encoded_url() == "z:/");
        BOOST_TEST(url("z://yy:80/").set_encoded_host("").encoded_url() == "z://:80/");
        BOOST_TEST(url("z://@").set_encoded_host("x").encoded_url() == "z://@x");
        BOOST_TEST(url("z://@yy").set_encoded_host("x").encoded_url() == "z://@x");
        BOOST_TEST(url("z://@yy").set_encoded_host("").encoded_url() == "z://@");
        BOOST_TEST(url("z://:@").set_encoded_host("x").encoded_url() == "z://:@x");
        BOOST_TEST(url("z://:@yy").set_encoded_host("").encoded_url() == "z://:@");
        BOOST_TEST(url("z:").set_encoded_host("yy").host_type() == host_type::name);
        BOOST_TEST(url("z:").set_encoded_host("1.2.3.4").encoded_url() == "z://1.2.3.4");
        BOOST_TEST(url("z://x").set_encoded_host("1.2.3.4").host_type() == host_type::ipv4);
        BOOST_TEST(url("z://x").set_encoded_host("1.2.3.4x").host_type() == host_type::name);
        BOOST_TEST(url("z://x").set_encoded_host("[::]").host_type() == host_type::ipv6);
        BOOST_TEST(url("z://x").set_encoded_host("[::]").encoded_host() == "[::]");
        BOOST_TEST_THROWS(url("z://x").set_encoded_host("[::]x"), invalid_part);
        BOOST_TEST_THROWS(url("z://x").set_encoded_host("[::"), invalid_part);
        BOOST_TEST_THROWS(url().set_encoded_host("x%"), invalid_part);
        BOOST_TEST_THROWS(url().set_encoded_host("1.2.3.4%"), invalid_part);

        testIPv4();
        testIPv6();
    }

    void
    testPort()
    {
        BOOST_TEST(url().port() == "");
        BOOST_TEST(url().port_part() == "");
        BOOST_TEST(url("//x:/").port() == "");
        BOOST_TEST(url("//x:/").port_part() == ":");
        BOOST_TEST(url("//x:80/").port() == "80");
        BOOST_TEST(url("//x:80/").port_part() == ":80");

        BOOST_TEST(url().set_port(80).encoded_url() == "//:80");
        BOOST_TEST(url("//:443/").set_port(80).encoded_url() == "//:80/");
        BOOST_TEST(url("//:80/").set_port(443).encoded_url() == "//:443/");

        BOOST_TEST(url().set_port("80").encoded_url() == "//:80");
        BOOST_TEST(url("//:443/").set_port("80").encoded_url() == "//:80/");
        BOOST_TEST(url("//:80/").set_port("443").encoded_url() == "//:443/");
        BOOST_TEST_THROWS(url().set_port("x"), invalid_part);
        BOOST_TEST(url("http://:443/").set_port("").encoded_url() == "http:/");

        BOOST_TEST(url().set_port_part(":80").encoded_url() == "//:80");
        BOOST_TEST(url("//:443/").set_port_part(":80").encoded_url() == "//:80/");
        BOOST_TEST(url("//:80/").set_port_part(":443").encoded_url() == "//:443/");
        BOOST_TEST(url("//:80/").set_port_part(":").encoded_url() == "//:/");
        BOOST_TEST(url("//:80/").set_port_part("").encoded_url() == "/");
        BOOST_TEST_THROWS(url().set_port_part("80"), invalid_part);
    }

    //------------------------------------------------------

    void
    testPath()
    {
        BOOST_TEST(url().encoded_path() == "");
        BOOST_TEST(url("x:a").encoded_path() == "a");
        BOOST_TEST(url("x:/a").encoded_path() == "/a");
        BOOST_TEST(url("x://y/a").encoded_path() == "/a");

        BOOST_TEST(url("x").encoded_path() == "x");
        BOOST_TEST(url("x/").encoded_path() == "x/");
        BOOST_TEST(url("x//").encoded_path() == "x//");

        BOOST_TEST(url("/").encoded_path() == "/");

        // path-empty
        BOOST_TEST(url("").set_encoded_path("").encoded_url() == "");
        BOOST_TEST(url("//x#").set_encoded_path("").encoded_url() == "//x#");

        // path-abempty
        BOOST_TEST(url("//x#").set_encoded_path("/").encoded_url() == "//x/#");
        BOOST_TEST(url("//x#").set_encoded_path("//").encoded_url() == "//x//#");
        BOOST_TEST(url("//x#").set_encoded_path("/y").encoded_url() == "//x/y#");
        BOOST_TEST_THROWS(url("//x#").set_encoded_path("x"), invalid_part);
        BOOST_TEST_THROWS(url("//x#").set_encoded_path("x/"), invalid_part);
        BOOST_TEST_THROWS(url("//x#").set_encoded_path("/%A"), invalid_part);
        BOOST_TEST_THROWS(url("//x#").set_encoded_path("/#"), invalid_part);

        // path-absolute
        BOOST_TEST(url("?#").set_encoded_path("/x").encoded_url() == "/x?#");
        BOOST_TEST(url("x:?#").set_encoded_path("/").encoded_url() == "x:/?#");
        BOOST_TEST_THROWS(url("?").set_encoded_path("//x"), invalid_part);
        BOOST_TEST_THROWS(url("?").set_encoded_path("/x%A"), invalid_part);
        BOOST_TEST_THROWS(url("x:?#").set_encoded_path("/x?"), invalid_part);
        BOOST_TEST_THROWS(url("/x/%"), invalid_part);

        // path-noscheme
        BOOST_TEST(url("").set_encoded_path("x").encoded_url() == "x");
        BOOST_TEST(url("").set_encoded_path("x/").encoded_url() == "x/");
        BOOST_TEST(url("").set_encoded_path("x//").encoded_url() == "x//");
        BOOST_TEST(url("?#").set_encoded_path("x").encoded_url() == "x?#");
        BOOST_TEST(url("?#").set_encoded_path("x/").encoded_url() == "x/?#");
        BOOST_TEST(url("?#").set_encoded_path("x//").encoded_url() == "x//?#");
        BOOST_TEST(url("yz/?#").set_encoded_path("x").encoded_url() == "x?#");
        BOOST_TEST(url("yz/?#").set_encoded_path("x/").encoded_url() == "x/?#");
        BOOST_TEST(url("yz/?#").set_encoded_path("x//").encoded_url() == "x//?#");
        BOOST_TEST_THROWS(url("yz/?#").set_encoded_path(":"), invalid_part);
        BOOST_TEST_THROWS(url("yz/?#").set_encoded_path("x:"), invalid_part);
        BOOST_TEST_THROWS(url("yz/?#").set_encoded_path("x:/q"), invalid_part);
        BOOST_TEST_THROWS(url("y/%"), invalid_part);

        // path-rootless
        BOOST_TEST(url("x:?#").set_encoded_path("y").encoded_url() == "x:y?#");
        BOOST_TEST(url("x:?#").set_encoded_path("y/").encoded_url() == "x:y/?#");
        BOOST_TEST(url("x:?#").set_encoded_path("y//").encoded_url() == "x:y//?#");
        BOOST_TEST_THROWS(url("x:?#").set_encoded_path("%A"), invalid_part);
        BOOST_TEST_THROWS(url("x:?#").set_encoded_path("y?"), invalid_part);
        BOOST_TEST_THROWS(url("x:y/%"), invalid_part);

        testSegments();
    }

    void
    testSegments()
    {
        // segments() const
        {
            url const v("/path/to/file.txt");
            auto ps = v.segments();
            static_assert(
                std::is_same<decltype(ps),
                    url_view::segments_type>::value, "");
        }

        {
            url::segments_type ps;
            BOOST_TEST(ps.empty());
            BOOST_TEST(ps.size() == 0);
            BOOST_TEST(ps.begin() == ps.end());
            BOOST_TEST(
                url::segments_type::iterator() ==
                url::segments_type::iterator());
        }
        {
            url v("/path/to/file.txt");
            auto ps = v.segments();
            BOOST_TEST(! ps.empty());
            BOOST_TEST(ps.size() == 3);
            BOOST_TEST(ps.begin() != ps.end());
            BOOST_TEST(ps.end() == ps.end());
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 3);

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
        {
            url u("http://user:pass@example.com:80?k1=v1&k2=v2");
            auto ps = u.segments();
            BOOST_TEST(ps.empty());
            ps.insert_encoded(
                ps.insert_encoded(
                    ps.insert_encoded(
                        ps.insert_encoded(
                            ps.insert_encoded(ps.end(), "a" ), "b" ), "c" ), "d" ), "file.txt" );
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 5);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 5);
            BOOST_TEST(u.encoded_path() == "/a/b/c/d/file.txt");

            {
                auto e = ps.begin();
                ++e;
                ++e;
                ps.erase( ps.begin(), e );
            }
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 3);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 3);
            BOOST_TEST(u.encoded_path() == "/c/d/file.txt");

            auto const after = ps.insert_encoded(ps.begin(), "a");
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 4);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 4);
            BOOST_TEST(u.encoded_path() == "/a/c/d/file.txt");
            ps.insert_encoded(after, "b");
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 5);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 5);
            BOOST_TEST(u.encoded_path() == "/a/b/c/d/file.txt");

            ps.insert_encoded(ps.begin(), "");
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 6);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 6);
            BOOST_TEST(u.encoded_path() == "//a/b/c/d/file.txt");

            ps.erase(ps.begin());
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 5);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 5);
            BOOST_TEST(u.encoded_path() == "/a/b/c/d/file.txt");

            ps.insert_encoded(ps.end(), "");
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 6);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 6);
            BOOST_TEST(u.encoded_path() == "/a/b/c/d/file.txt/");

            {
                auto e = ps.end();
                --e;
                ps.erase(e);
            }
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 5);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 5);
            BOOST_TEST(u.encoded_path() == "/a/b/c/d/file.txt");

            BOOST_TEST_THROWS(ps.insert_encoded(ps.begin(), "/"), invalid_part);
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 5);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 5);
            BOOST_TEST(u.encoded_path() == "/a/b/c/d/file.txt");

            ps.insert(ps.begin(), "/");
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 6);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 6);
            BOOST_TEST(u.encoded_path() == "/%2F/a/b/c/d/file.txt");

            {
                auto n = u.capacity();
                std::string s(n - u.size() + ps.begin()->encoded_string().size(), 'a');
                ps.replace_encoded(ps.begin(), s);
                BOOST_TEST(n == u.capacity());
                BOOST_TEST(u.size() == u.capacity());
                BOOST_TEST(!ps.empty());
                BOOST_TEST(ps.size() == 6);
                BOOST_TEST(std::distance(ps.begin(), ps.end()) == 6);
                BOOST_TEST(u.encoded_path() == "/" + s + "/a/b/c/d/file.txt");

                s += 'a';
                ps.replace_encoded(ps.begin(), s);
                BOOST_TEST(n < u.capacity());
                BOOST_TEST(!ps.empty());
                BOOST_TEST(ps.size() == 6);
                BOOST_TEST(std::distance(ps.begin(), ps.end()) == 6);
                BOOST_TEST(u.encoded_path() == "/" + s + "/a/b/c/d/file.txt");

                ps.replace_encoded(ps.begin(), "%2F");
            }

            BOOST_TEST_THROWS(ps.replace_encoded(ps.begin(), "/"), invalid_part);
            BOOST_TEST(!ps.empty());
            BOOST_TEST(ps.size() == 6);
            BOOST_TEST(std::distance(ps.begin(), ps.end()) == 6);
            BOOST_TEST(u.encoded_path() == "/%2F/a/b/c/d/file.txt");

            {
                auto n = u.capacity();
                std::string s(n - u.size() + ps.begin()->encoded_string().size(), 'a');
                ps.replace(ps.begin(), s);
                BOOST_TEST(n == u.capacity());
                BOOST_TEST(u.size() == u.capacity());
                BOOST_TEST(!ps.empty());
                BOOST_TEST(ps.size() == 6);
                BOOST_TEST(std::distance(ps.begin(), ps.end()) == 6);
                BOOST_TEST(u.encoded_path() == "/" + s + "/a/b/c/d/file.txt");

                ps.replace(ps.begin(), s + '/');
                BOOST_TEST(n < u.capacity());
                BOOST_TEST(!ps.empty());
                BOOST_TEST(ps.size() == 6);
                BOOST_TEST(std::distance(ps.begin(), ps.end()) == 6);
                BOOST_TEST(u.encoded_path() == "/" + s + "%2F/a/b/c/d/file.txt");

                ps.replace_encoded(ps.begin(), "%2F");
            }
        }
    }

    //------------------------------------------------------

    void
    testQuery()
    {
        BOOST_TEST(url("").query() == "");
        BOOST_TEST(url("?").query() == "");
        BOOST_TEST(url("?x").query() == "x");

        BOOST_TEST(url("").encoded_query() == "");
        BOOST_TEST(url("?").encoded_query() == "");
        BOOST_TEST(url("?x").encoded_query() == "x");

        BOOST_TEST(url("").query_part() == "");
        BOOST_TEST(url("?").query_part() == "?");
        BOOST_TEST(url("?x").query_part() == "?x");

        BOOST_TEST(url().set_query("").query_part() == "");
        BOOST_TEST(url().set_query("?").query_part() == "??");
        BOOST_TEST(url().set_query("?x").query_part() == "??x");
        BOOST_TEST(url().set_query("#").query_part() == "?%23");

        BOOST_TEST(url().set_encoded_query("").query_part() == "");
        BOOST_TEST(url().set_encoded_query("x").query_part() == "?x");
        BOOST_TEST(url().set_encoded_query("?").query_part() == "??");
        BOOST_TEST(url().set_encoded_query("%23").query() == "#");
        BOOST_TEST_THROWS(url().set_encoded_query("#"), invalid_part);
        BOOST_TEST_THROWS(url().set_encoded_query("#x"), invalid_part);

        BOOST_TEST(url().set_query_part("").query_part() == "");
        BOOST_TEST(url().set_query_part("?").query_part() == "?");
        BOOST_TEST(url().set_query_part("?x").query_part() == "?x");
        BOOST_TEST(url().set_query_part("??x").query_part() == "??x");
        BOOST_TEST_THROWS(url().set_query_part("x"), invalid_part);
        BOOST_TEST_THROWS(url().set_query_part("%3F"), invalid_part);

        BOOST_TEST(url("//?").set_query("").encoded_url() == "//");
        BOOST_TEST(url("//?x").set_query("").encoded_url() == "//");
        BOOST_TEST(url("//?xy").set_query("y").encoded_url() == "//?y");
        BOOST_TEST(url("//").set_query("?").encoded_url() == "//??");
        BOOST_TEST(url("//").set_query("??").encoded_url() == "//???");

        BOOST_TEST(url("//?").set_encoded_query("").encoded_url() == "//");
        BOOST_TEST(url("//?x").set_encoded_query("").encoded_url() == "//");
        BOOST_TEST(url("//?xy").set_encoded_query("y").encoded_url() == "//?y");
        BOOST_TEST_THROWS(url("//").set_encoded_query("#"), invalid_part);
        BOOST_TEST_THROWS(url("//").set_encoded_query("#?"), invalid_part);

        BOOST_TEST(url("//?").set_query_part("").encoded_url() == "//");
        BOOST_TEST(url("//?x").set_query_part("").encoded_url() == "//");
        BOOST_TEST_THROWS(url("//?xy").set_query_part("y"), invalid_part);
        BOOST_TEST(url("//?xy").set_query_part("?y").encoded_url() == "//?y");

        BOOST_TEST_THROWS(url("?%"), invalid_part);
        BOOST_TEST(url("?x=").encoded_url() == "?x=");
        BOOST_TEST_THROWS(url("?x=%"), invalid_part);
        BOOST_TEST(url("?x=#").encoded_url() == "?x=#");

        testParams();
    }

    void
    testParams()
    {
        // params() const
        {
            url const v("?x=1&y=2&y=3&z");
            auto qp = v.params();
            static_assert(
                std::is_same<decltype(qp),
                    url_view::params_type>::value, "");
        }

        {
            url::params_type qp;
            BOOST_TEST(qp.empty());
            BOOST_TEST(qp.size() == 0);
            BOOST_TEST(qp.begin() == qp.end());
            BOOST_TEST(! qp.contains("x"));
            BOOST_TEST(qp.count("x") == 0);
            BOOST_TEST(qp.find("x") == qp.end());
            BOOST_TEST_THROWS(qp.at("x"), out_of_range);

            BOOST_TEST(
                url::params_type::iterator() ==
                url::params_type::iterator());
        }
        {
            url v("?x=1&y=2&y=3&z");
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
        BOOST_TEST(url("").fragment() == "");
        BOOST_TEST(url("#").fragment() == "");
        BOOST_TEST(url("#x").fragment() == "x");

        BOOST_TEST(url("").encoded_fragment() == "");
        BOOST_TEST(url("#").encoded_fragment() == "");
        BOOST_TEST(url("#x").encoded_fragment() == "x");

        BOOST_TEST(url("").fragment_part() == "");
        BOOST_TEST(url("#").fragment_part() == "#");
        BOOST_TEST(url("#x").fragment_part() == "#x");

        BOOST_TEST(url().set_fragment("").fragment_part() == "");
        BOOST_TEST(url().set_fragment("#").fragment_part() == "#%23");
        BOOST_TEST(url().set_fragment("#x").fragment_part() == "#%23x");

        BOOST_TEST(url().set_encoded_fragment("").fragment_part() == "");
        BOOST_TEST(url().set_encoded_fragment("x").fragment_part() == "#x");
        BOOST_TEST(url().set_encoded_fragment("%23").fragment_part() == "#%23");
        BOOST_TEST_THROWS(url().set_encoded_fragment("#"), invalid_part);
        BOOST_TEST_THROWS(url().set_encoded_fragment("#x"), invalid_part);

        BOOST_TEST(url().set_fragment_part("").fragment_part() == "");
        BOOST_TEST(url().set_fragment_part("#").fragment_part() == "#");
        BOOST_TEST(url().set_fragment_part("#x").fragment_part() == "#x");
        BOOST_TEST(url().set_fragment_part("#%23x").fragment_part() == "#%23x");
        BOOST_TEST_THROWS(url().set_fragment_part("x"), invalid_part);
        BOOST_TEST_THROWS(url().set_fragment_part("%23"), invalid_part);

        BOOST_TEST(url("//#").set_fragment("").encoded_url() == "//");
        BOOST_TEST(url("//#x").set_fragment("").encoded_url() == "//");
        BOOST_TEST(url("//#xy").set_fragment("y").encoded_url() == "//#y");
        BOOST_TEST(url("//").set_fragment("#").encoded_url() == "//#%23");
        BOOST_TEST(url("//").set_fragment("##").encoded_url() == "//#%23%23");

        BOOST_TEST(url("//#").set_encoded_fragment("").encoded_url() == "//");
        BOOST_TEST(url("//#x").set_encoded_fragment("").encoded_url() == "//");
        BOOST_TEST(url("//#xy").set_encoded_fragment("y").encoded_url() == "//#y");
        BOOST_TEST_THROWS(url("//").set_encoded_fragment("#"), invalid_part);
        BOOST_TEST_THROWS(url("//").set_encoded_fragment("##"), invalid_part);

        BOOST_TEST(url("//#").set_fragment_part("").encoded_url() == "//");
        BOOST_TEST(url("//#x").set_fragment_part("").encoded_url() == "//");
        BOOST_TEST_THROWS(url("//#xy").set_fragment_part("y"), invalid_part);
        BOOST_TEST(url("//#xy").set_fragment_part("#y").encoded_url() == "//#y");

        BOOST_TEST_THROWS(url("#%"), invalid_part);
    }

    //------------------------------------------------------

    void
    testNormalize()
    {
        BOOST_TEST(url("").normalize_scheme().encoded_url() == "");
        BOOST_TEST(url("/").normalize_scheme().encoded_url() == "/");
        BOOST_TEST(url("http://").normalize_scheme().encoded_url() == "http://");
        BOOST_TEST(url("Http://").normalize_scheme().encoded_url() == "http://");
        BOOST_TEST(url("HTtp://").normalize_scheme().encoded_url() == "http://");
        BOOST_TEST(url("HTTp://").normalize_scheme().encoded_url() == "http://");
        BOOST_TEST(url("HTTP://").normalize_scheme().encoded_url() == "http://");
    }

    //------------------------------------------------------

    void
    run()
    {
        testObservers();

        testConstValue();

        testCtor();
        testScheme();
        testOrigin();
        testAuthority();
        testUsername();
        testPassword();

        testUserinfo();
        testUser();
        testHostAndPort();
        testHost();
        testPort();
        testPath();
        testQuery();
        testFragment();

        testNormalize();
    }
};

TEST_SUITE(url_test, "boost.url.url");

} // urls
} // boost
