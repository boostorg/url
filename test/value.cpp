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
    dump(view const& u)
    {
        (void)u;
#if 0
        log <<
            "href     : " << u.encoded_href() << "\n"
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
        dump(value("http:?query#fragment"));
        dump(value("http://user:pass@example.com/path/to/file.txt?query#fragment"));
        dump(value("http://?query#fragment"));
        dump(value("http:?query#fragment@example"));
        dump(value("http://?query#fragment@example"));

        BOOST_TEST(value().encoded_href() == "");
    }

    //------------------------------------------------------
    
    void
    testScheme()
    {
        BOOST_TEST(value().scheme() == "");
        BOOST_TEST(value("http:").scheme() == "http");
        BOOST_TEST(value("http:").encoded_href() == "http:");
        BOOST_TEST(value("http:").set_scheme("").scheme() == "");
        BOOST_TEST(value("http:").set_scheme("").encoded_href() == "");
        BOOST_TEST(value("http:").set_scheme("ftp").encoded_href() == "ftp:");
        BOOST_TEST(value("ws:").set_scheme("gopher").encoded_href() == "gopher:");
        BOOST_TEST(value("http://example.com").set_scheme("ftp").encoded_href() == "ftp://example.com");
        BOOST_TEST(value("ws://example.com").set_scheme("gopher").encoded_href() == "gopher://example.com");

        BOOST_TEST_THROWS(value().set_scheme("c@t"), invalid_part);
        BOOST_TEST_THROWS(value().set_scheme("1cat"), invalid_part);
        BOOST_TEST_THROWS(value().set_scheme("http:s"), invalid_part);
    }

    //------------------------------------------------------

    void
    testUsername()
    {
        BOOST_TEST(value().username() == "");
        BOOST_TEST(value().encoded_username() == "");
        BOOST_TEST(value().set_username("").username() == "");
        BOOST_TEST(value().set_username("user").encoded_href() == "//user@");
        BOOST_TEST(value().set_encoded_username("user%20name").encoded_href() == "//user%20name@");
        BOOST_TEST(value().set_encoded_username("user%3Aname").encoded_href() == "//user%3Aname@");
        BOOST_TEST(value().set_encoded_username("user%3Aname").username() == "user:name");
        BOOST_TEST(value().set_encoded_username("user%40name").encoded_href() == "//user%40name@");
        BOOST_TEST(value().set_encoded_username("user%40name").username() == "user@name");

        BOOST_TEST(value("http:").set_encoded_username("").encoded_href() == "http:");
        BOOST_TEST(value("http://@").set_encoded_username("").encoded_href() == "http://");
        BOOST_TEST(value("http://x@").set_encoded_username("").encoded_href() == "http://");
        BOOST_TEST(value("http://x:@").set_encoded_username("").encoded_href() == "http://:@");
        BOOST_TEST(value("http://:@").set_username("").encoded_href() == "http://:@");
        BOOST_TEST(value("http://:y@").set_encoded_username("x").encoded_href() == "http://x:y@");
        BOOST_TEST(value("ws://user:pass@").set_username("x").encoded_href() == "ws://x:pass@");

        BOOST_TEST_THROWS(value().set_encoded_username("user:pass"), invalid_part);
        BOOST_TEST_THROWS(value().set_encoded_username("user name"), invalid_part);
    }

    void
    testPassword()
    {
        BOOST_TEST(value().password() == "");
        BOOST_TEST(value().encoded_password() == "");
        BOOST_TEST(value().set_encoded_password("").password() == "");
        BOOST_TEST(value().set_password("pass").encoded_href() == "//:pass@");
        BOOST_TEST(value().set_encoded_password("%40pass").encoded_href() == "//:%40pass@");
        BOOST_TEST(value().set_encoded_password("pass%20word").encoded_href() == "//:pass%20word@");
        BOOST_TEST(value().set_encoded_password("pass%42word").encoded_href() == "//:pass%42word@");

        BOOST_TEST(value("http:").set_encoded_password("").encoded_href() == "http:");
        BOOST_TEST(value("http://@").set_encoded_password("").encoded_href() == "http://");
        BOOST_TEST(value("http://x@").set_encoded_password("").encoded_href() == "http://x@");
        BOOST_TEST(value("http://x@").set_encoded_password("y").encoded_href() == "http://x:y@");
        BOOST_TEST(value("http://:@").set_encoded_password("").encoded_href() == "http://");
        BOOST_TEST(value("http://:y@").set_password("pass").encoded_href() == "http://:pass@");
        BOOST_TEST(value("http://x:y@").set_password("pass").encoded_href() == "http://x:pass@");
        BOOST_TEST(value("http://x:pass@").set_password("y").encoded_href() == "http://x:y@");
        BOOST_TEST(value("http://x:pass@example.com").set_password("y").encoded_href() == "http://x:y@example.com");

        BOOST_TEST_THROWS(value().set_encoded_password("pass word"), invalid_part);
        BOOST_TEST_THROWS(value().set_encoded_password(":pass"), invalid_part);
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
        BOOST_TEST(value("http://z.com/").set_encoded_userinfo("").encoded_href() == "http://z.com/");
        BOOST_TEST(value("http://x:y@z.com").set_encoded_userinfo("").encoded_href() == "http://z.com");
    }

    void
    testHostname()
    {
        BOOST_TEST(value().hostname() == "");
        BOOST_TEST(value().encoded_hostname() == "");
        BOOST_TEST(value().set_hostname("").encoded_href() == "");
        BOOST_TEST(value().set_hostname("localhost").hostname() == "localhost");
        BOOST_TEST(value().set_hostname("local host").hostname() == "local host");
        BOOST_TEST(value().set_hostname("local host").encoded_hostname() == "local%20host");
        BOOST_TEST(value().set_encoded_hostname("local%20host").hostname() == "local host");

        BOOST_TEST(value("http://x.com").set_encoded_hostname("localhost").encoded_href() == "http://localhost");
        BOOST_TEST(value("http://x.com").set_encoded_hostname("").encoded_href() == "http://");
        BOOST_TEST(value("http://@x.com").set_encoded_hostname("").encoded_href() == "http://@");
        BOOST_TEST(value("http://:@x.com").set_encoded_hostname("").encoded_href() == "http://:@");
        BOOST_TEST(value("http://:@x.com/").set_encoded_hostname("").encoded_href() == "http://:@/");

        BOOST_TEST_THROWS(value().set_encoded_hostname("local host"), invalid_part);
    }

    void
    testPort()
    {
        BOOST_TEST(! value().port().has_value());
        BOOST_TEST(value().port_string() == "");
        BOOST_TEST(value().set_port({}).port_string() == "");
        BOOST_TEST(value().set_port({}).encoded_href() == "");
        BOOST_TEST(value().set_port_string("80").port_string() == "80");
        BOOST_TEST(*value().set_port_string("80").port() == 80);
        BOOST_TEST(value().set_port_string("80").encoded_href() == "//:80");
        BOOST_TEST(value().set_port(80).encoded_href() == "//:80");
        BOOST_TEST(value("http://:80").set_port_string("").encoded_href() == "http://");
        BOOST_TEST(value("http://:80").set_port_string("").set_port(443).encoded_href() == "http://:443");
        BOOST_TEST(value("http://x.com:80").set_port_string("").encoded_href() == "http://x.com");
        BOOST_TEST(value("http://x.com:80").set_port(443).encoded_href() == "http://x.com:443");
        BOOST_TEST(value("http://x.com:80").set_port({}).port().has_value() == false);

        BOOST_TEST_THROWS(value().set_port_string("12bad"), invalid_part);
        BOOST_TEST_THROWS(value().set_port_string("9999999"), invalid_part);
    }

    void
    testHost()
    {
        BOOST_TEST(value().set_encoded_host("example.com").encoded_href() == "//example.com");
        BOOST_TEST(value().set_encoded_host("x:1").encoded_href() == "//x:1");
        BOOST_TEST(value().set_encoded_host("x:1").set_encoded_host("").encoded_href() == "//");
        BOOST_TEST(value().set_encoded_host("x:1").set_encoded_host("example.com:443").encoded_href() == "//example.com:443");
        BOOST_TEST(value().set_encoded_host("local%20host%3A443").encoded_href() == "//local%20host%3A443");
        BOOST_TEST(! value().set_encoded_host("local%20host%3A443").port().has_value());

        BOOST_TEST(value().set_encoded_host(":").encoded_href() == "//:");
        BOOST_TEST(value().set_encoded_host(":").set_port({}).encoded_host() == "");
    }

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
        BOOST_TEST(value("http://@").set_encoded_authority("user:pass@example.com").encoded_href() == "http://user:pass@example.com");
    }

    //------------------------------------------------------

    void
    testOrigin()
    {
        BOOST_TEST(value().encoded_origin() == "");
        BOOST_TEST(value("http://user:pass@example.com/path/to/file.txt?q#f").encoded_origin() ==
                         "http://user:pass@example.com");
        BOOST_TEST(value("http://user:pass@example.com/path/to/file.txt?q#f"
            ).set_encoded_origin("ws://x.com").encoded_href() == "ws://x.com/path/to/file.txt?q#f");
        BOOST_TEST(
            value("http://host:80/")
            .set_encoded_origin("http://host:443/")
            .port_string() == "443");
    }

    //------------------------------------------------------

    void
    print(segments const& ss)
    {
        for(auto const e : ss)
            log << "\"" << e << "\"" << std::endl;
    }

    void
    testPath()
    {
        BOOST_TEST(value("/path/to/file.txt").encoded_path() == "/path/to/file.txt");
        BOOST_TEST(
            value("/path/to/file.txt")
            .set_encoded_segment(0, "p")
            .encoded_href() == "/p/to/file.txt");
        BOOST_TEST(
            value("/path/to/file.txt")
            .set_encoded_segment(1, "from")
            .encoded_href() == "/path/from/file.txt");
        BOOST_TEST(
            value("/path/to/file.txt")
            .set_encoded_segment(2, "index.htm")
            .encoded_href() == "/path/to/index.htm");
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
            .encoded_href() == "/pa%20th/to/file.txt");
        BOOST_TEST_THROWS(
            value("/path/to/file.txt")
            .set_encoded_segment(0, "path/from"),
            invalid_part);

#if 0
        value u;
        u.set_scheme("http");
        u.set_encoded_userinfo("user:password");
        u.set_encoded_host("example.com:443");
        u.set_encoded_path("/");
        log << u.encoded_href() << std::endl;

        u.set_encoded_path("/path/to/file.txt");
        log << u.encoded_href() << std::endl;
        print(u.segments());
        u.set_segment(0, "pa th");
        log << u.encoded_href() << std::endl;
        u.set_segment(1, "teux");
        log << u.encoded_href() << std::endl;
#endif
    }

    //------------------------------------------------------

    void
    testQuery()
    {
    }

    //------------------------------------------------------

    void
    testFragment()
    {
        BOOST_TEST(value("").fragment().empty());
        BOOST_TEST(value("#").fragment() == "#");
        BOOST_TEST(value("#x").fragment() == "#x");

        BOOST_TEST(value("").encoded_fragment().empty());
        BOOST_TEST(value("#").encoded_fragment() == "#");
        BOOST_TEST(value("#x").encoded_fragment() == "#x");

        BOOST_TEST(value().set_fragment("#").fragment() == "#");
        BOOST_TEST(value().set_fragment("#").encoded_fragment() == "#");
        BOOST_TEST(value().set_encoded_fragment("#").fragment() == "#");
        BOOST_TEST(value().set_encoded_fragment("#").encoded_fragment() == "#");
        BOOST_TEST(value().set_fragment("#x").fragment() == "#x");
        BOOST_TEST(value().set_fragment("x").fragment() == "#x");
        BOOST_TEST(value().set_encoded_fragment("#x").fragment() == "#x");
        BOOST_TEST(value().set_encoded_fragment("x").fragment() == "#x");
        BOOST_TEST(value("http://#").set_fragment("").encoded_href() == "http://");
        BOOST_TEST(value("http://").set_fragment("#").encoded_href() == "http://#");
        BOOST_TEST(value("http://").set_fragment("##").encoded_href() == "http://#%23");
        BOOST_TEST(value().set_encoded_fragment("%23").encoded_href() == "#%23");
    }

    //------------------------------------------------------

    void
    run()
    {
        //value u("foof://:;@[::]/@;:??:;@/~@;://#//:;@~/@;:\\?\\?//:foof");
        //dump(u);
        testCtor();
        testScheme();
        testUsername();
        testPassword();
        testUserinfo();
        testHostname();
        testPort();
        testHost();
        testAuthority();
        testOrigin();
        testPath();
        testQuery();
        testFragment();
    }
};

TEST_SUITE(value_test, "boost.url.value");

} // url
} // boost
