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

    request-target = origin-form
                   / absolute-form
                   / authority-form
                   / asterisk-form

    URI-reference   = URI / relative-ref
    URI             = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    absolute-URI    = scheme ":" hier-part [ "?" query ]
    relative-ref    = relative-part [ "?" query ] [ "#" fragment ]
    origin-form     = absolute-path [ "?" query ]
    absolute_form   = absolute-URI
    authority-form  = host [ ":" port ]
        
    kind::url           URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    http://www.example.com:80/path/to/file.txt?query#fragment

    kind::absolute      absolute-URI  = scheme ":" hier-part [ "?" query ]

    hier-part
        "//" authority path-abempty
            //
            //example.com
            //example.com/
            //example.com/path/to/file.txt
        path-absolute
            /
            /path
            /path/to/file.txt
        path-rootless
            path
            path/to/file.txt
        path-empty
            "" (empty)

    relative-part
        "//" authority path-abempty
            //
            //example.com
            //example.com/
            //example.com/path/to/file.txt
        path-absolute
            /
            /path
            /path/to/file.txt
        path-noscheme
            path
            path/to/file.txt
        path-empty
            "" (empty)

    If a URI contains an authority component, then the path component
    must either be empty or begin with a slash ("/") character.  If a URI
    does not contain an authority component, then the path cannot begin
    with two slash characters ("//").  In addition, a URI reference
    (Section 4.1) may be a relative-path reference, in which case the
    first path segment cannot contain a colon (":") character.  The ABNF
    requires five separate rules to disambiguate these cases, only one of
    which will match the path substring within a given URI reference.  We
    use the generic term "path component" to describe the URI substring
    matched by the parser to one of these rules.

        path          = path-abempty    ; begins with "/" or is empty
                      / path-absolute   ; begins with "/" but not "//"
                      / path-noscheme   ; begins with a non-colon segment
                      / path-rootless   ; begins with a segment
                      / path-empty      ; zero characters

        path-abempty  = *( "/" segment )
        path-absolute = "/" [ segment-nz *( "/" segment ) ]
        path-noscheme = segment-nz-nc *( "/" segment )
        path-rootless = segment-nz *( "/" segment )
        path-empty    = 0<pchar>

*/

namespace boost {
namespace url {

class value_test
{
public:
    test_suite::log_type log;

    void
    dump(value const& u)
    {
        (void)u;
        log <<
            "href     : " << u.encoded_url() << "\n"
            "scheme   : " << u.scheme() << "\n"
            "user     : " << u.encoded_username() << "\n"
            "password : " << u.encoded_password() << "\n"
            "hostname : " << u.encoded_hostname() << "\n"
            "port     : " << u.port_part() << "\n" <<
            "path     : " << u.encoded_path() << "\n"
            "query    : " << u.query_part() << "\n"
            "fragment : " << u.fragment_part() << "\n"
            //"resource : " << u.encoded_resource() << "\n"
            ;
        log.flush();
    }

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
            BOOST_TEST(v.port_part() == ":80");
            BOOST_TEST(v.port() == "80");
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
            .port() == "443");
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
        BOOST_TEST(value().set_encoded_host("local%20host%3A443").port_part() == "");

        BOOST_TEST(value().set_encoded_host(":").encoded_url() == "//:");
        BOOST_TEST(value().set_encoded_host(":").set_port("").encoded_host() == "");
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

    //------------------------------------------------------

    void
    testPort()
    {
        BOOST_TEST(value().port() == "");
        BOOST_TEST(value().port_part() == "");
        BOOST_TEST(value("//x:/").port() == "");
        BOOST_TEST(value("//x:/").port_part() == ":");
        BOOST_TEST(value("//x:80/").port() == "80");
        BOOST_TEST(value("//x:80/").port_part() == ":80");

        BOOST_TEST(value().set_port(80).encoded_url() == "//:80");
        BOOST_TEST(value("//:443/").set_port(80).encoded_url() == "//:80/");
        BOOST_TEST(value("//:80/").set_port(443).encoded_url() == "//:443/");

        BOOST_TEST(value().set_port("80").encoded_url() == "//:80");
        BOOST_TEST(value("//:443/").set_port("80").encoded_url() == "//:80/");
        BOOST_TEST(value("//:80/").set_port("443").encoded_url() == "//:443/");
        BOOST_TEST_THROWS(value().set_port("x"), invalid_part);
        BOOST_TEST(value("http://:443/").set_port("").encoded_url() == "http:/");

        BOOST_TEST(value().set_port_part(":80").encoded_url() == "//:80");
        BOOST_TEST(value("//:443/").set_port_part(":80").encoded_url() == "//:80/");
        BOOST_TEST(value("//:80/").set_port_part(":443").encoded_url() == "//:443/");
        BOOST_TEST(value("//:80/").set_port_part(":").encoded_url() == "//:/");
        BOOST_TEST(value("//:80/").set_port_part("").encoded_url() == "/");
        BOOST_TEST_THROWS(value().set_port_part("80"), invalid_part);

#if 0
        BOOST_TEST(value().port() == "");
        BOOST_TEST(value().set_port({}).port() == "");
        BOOST_TEST(value().set_port({}).encoded_url() == "");
        BOOST_TEST(value().set_port("80").port() == "80");
        BOOST_TEST(value().set_port(80).port() == "80");
        BOOST_TEST(value().set_port("80").encoded_url() == "//:80");
        BOOST_TEST(value().set_port(80).encoded_url() == "//:80");
        BOOST_TEST(value("http://:80").set_port("").encoded_url() == "http://");
        BOOST_TEST(value("http://:80").set_port("").set_port(443).encoded_url() == "http://:443");
        BOOST_TEST(value("http://x.com:80").set_port("").encoded_url() == "http://x.com");
        BOOST_TEST(value("http://x.com:80").set_port(443).encoded_url() == "http://x.com:443");
        BOOST_TEST(value("http://x.com:80/").clear_port().encoded_url() == "http://x.com/");

        BOOST_TEST_THROWS(value().set_port("12bad"), invalid_part);
        BOOST_TEST_THROWS(value().set_port("9999999"), invalid_part);
#endif
    }

    //------------------------------------------------------

    void
    testPath()
    {
        BOOST_TEST(value().encoded_path() == "");
        BOOST_TEST(value("x:a").encoded_path() == "a");
        BOOST_TEST(value("x:/a").encoded_path() == "/a");
        BOOST_TEST(value("x://y/a").encoded_path() == "/a");

        // path-empty
        BOOST_TEST(value("").set_encoded_path("").encoded_url() == "");
        BOOST_TEST(value("//x#").set_encoded_path("").encoded_url() == "//x#");

        // path-abempty
        BOOST_TEST(value("//x#").set_encoded_path("/").encoded_url() == "//x/#");
        BOOST_TEST(value("//x#").set_encoded_path("//").encoded_url() == "//x//#");
        BOOST_TEST(value("//x#").set_encoded_path("/y").encoded_url() == "//x/y#");
        BOOST_TEST_THROWS(value("//x#").set_encoded_path("x"), invalid_part);
        BOOST_TEST_THROWS(value("//x#").set_encoded_path("x/"), invalid_part);
        BOOST_TEST_THROWS(value("//x#").set_encoded_path("/%A"), invalid_part);
        BOOST_TEST_THROWS(value("//x#").set_encoded_path("/#"), invalid_part);

        // path-absolute
        BOOST_TEST(value("?#").set_encoded_path("/x").encoded_url() == "/x?#");
        BOOST_TEST_THROWS(value("?").set_encoded_path("//x"), invalid_part);
        BOOST_TEST_THROWS(value("?").set_encoded_path("/x%A"), invalid_part);

        // path-noscheme
        BOOST_TEST(value("").set_encoded_path("x").encoded_url() == "x");
        BOOST_TEST(value("").set_encoded_path("x/").encoded_url() == "x/");
        BOOST_TEST(value("").set_encoded_path("x//").encoded_url() == "x//");
        BOOST_TEST(value("?#").set_encoded_path("x").encoded_url() == "x?#");
        BOOST_TEST(value("?#").set_encoded_path("x/").encoded_url() == "x/?#");
        BOOST_TEST(value("?#").set_encoded_path("x//").encoded_url() == "x//?#");
        BOOST_TEST(value("yz/?#").set_encoded_path("x").encoded_url() == "x?#");
        BOOST_TEST(value("yz/?#").set_encoded_path("x/").encoded_url() == "x/?#");
        BOOST_TEST(value("yz/?#").set_encoded_path("x//").encoded_url() == "x//?#");
        BOOST_TEST_THROWS(value("yz/?#").set_encoded_path(":"), invalid_part);
        BOOST_TEST_THROWS(value("yz/?#").set_encoded_path("x:"), invalid_part);
        BOOST_TEST_THROWS(value("yz/?#").set_encoded_path("x:/q"), invalid_part);

        // path-rootless
        BOOST_TEST(value("x:?#").set_encoded_path("y").encoded_url() == "x:y?#");
        BOOST_TEST(value("x:?#").set_encoded_path("y/").encoded_url() == "x:y/?#");
        BOOST_TEST(value("x:?#").set_encoded_path("y//").encoded_url() == "x:y//?#");
        BOOST_TEST_THROWS(value("x:?#").set_encoded_path("/"), invalid_part);
        BOOST_TEST_THROWS(value("x:?#").set_encoded_path("%A"), invalid_part);
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
        testHost();

        testPort();
        testPath();
        testQuery();
        testFragment();
    }
};

TEST_SUITE(value_test, "boost.url.value");

} // url
} // boost
