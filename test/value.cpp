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

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <iostream>

namespace boost {
namespace url {

class value_test : public beast::unit_test::suite
{
public:
    void
    testScheme()
    {
        value u;
        BEAST_EXPECT(u.scheme() == "");
        BEAST_EXPECT(u.set_scheme("ftp").scheme() == "ftp");
        BEAST_EXPECT(u.set_scheme("ws").scheme() == "ws");
        BEAST_EXPECT(u.set_scheme("http").scheme() == "http");
        BEAST_EXPECT(u.set_scheme({}).scheme() == "");

        BEAST_THROWS(u.set_scheme("c@t"), system_error);
        BEAST_THROWS(u.set_scheme("1cat"), system_error);
    }

    //------------------------------------------------------

    void
    testUsername()
    {
        value u;
        BEAST_EXPECT(u.username() == "");
        BEAST_EXPECT(u.set_username({}).username() == "");
        BEAST_EXPECT(u.set_username("user").username() == "user");
        BEAST_EXPECT(u.set_username("user name").username() == "user name");
        BEAST_EXPECT(u.encoded_username() == "user%20name");
        BEAST_EXPECT(u.set_encoded_username("user%20name").username() == "user name");
        BEAST_EXPECT(u.encoded_userinfo() == "user%20name");
        BEAST_EXPECT(u.set_username({}).username() == "");

        BEAST_THROWS(u.set_username("user:pass"), system_error);
        BEAST_THROWS(u.set_encoded_username("user name"), system_error);
    }

    void
    testPassword()
    {
        value u;
        BEAST_EXPECT(u.password() == "");
        BEAST_EXPECT(u.set_password({}).password() == "");
        BEAST_EXPECT(u.set_password("password").password() == "password");
        BEAST_EXPECT(u.set_password("pass word").password() == "pass word");
        BEAST_EXPECT(u.encoded_password() == "pass%20word");
        BEAST_EXPECT(u.set_encoded_password("pass%20word").password() == "pass word");
        BEAST_EXPECT(u.encoded_userinfo() == ":pass%20word");
        BEAST_EXPECT(u.set_password({}).password() == "");

        BEAST_THROWS(u.set_encoded_password("pass word"), system_error);
    }

    void
    testUserinfo()
    {
        {
            value u;
            BEAST_EXPECT(u.set_encoded_userinfo({}).encoded_userinfo() == "");
        }
        {
            value u;
            BEAST_EXPECT(u.set_encoded_userinfo("user:").encoded_userinfo() == "user:");
        }
        {
            value u;
            BEAST_EXPECT(u.set_encoded_userinfo(":pass").encoded_userinfo() == ":pass");
        }
        {
            value u;
            BEAST_EXPECT(u.set_encoded_userinfo("user:pass").encoded_userinfo() == "user:pass");
            BEAST_EXPECT(u.set_encoded_userinfo({}).encoded_userinfo() == "");
        }

        {
            value u;
            BEAST_EXPECT(u.encoded_userinfo() == "");
            u.set_username("user");
            BEAST_EXPECT(u.encoded_userinfo() == "user");
            u.set_password("password");
            BEAST_EXPECT(u.encoded_userinfo() == "user:password");
            u.set_username({});
            BEAST_EXPECT(u.encoded_userinfo() == ":password");
            u.set_encoded_userinfo("user:");
            BEAST_EXPECT(u.encoded_userinfo() == "user:");
        }
    }

    //------------------------------------------------------

    void
    testHostname()
    {
        value u;
        BEAST_EXPECT(u.hostname() == "");
        BEAST_EXPECT(u.set_hostname("localhost").hostname() == "localhost");
        BEAST_EXPECT(u.set_hostname("local host").hostname() == "local host");
        BEAST_EXPECT(u.encoded_hostname() == "local%20host");
        BEAST_EXPECT(u.set_encoded_hostname("local%20host").hostname() == "local host");
        BEAST_EXPECT(u.encoded_host() == "local%20host");

        BEAST_THROWS(u.set_encoded_hostname("local host"), system_error);
    }

    void
    testPort()
    {
        value u;
        BEAST_EXPECT(! u.port());
        BEAST_EXPECT(*u.set_port(80).port() == 80);
        BEAST_EXPECT(u.encoded_host() == ":80");
        BEAST_EXPECT(! u.set_port({}).port().has_value());
        BEAST_EXPECT(u.encoded_host() == "");
        BEAST_EXPECT(*u.set_port_string("20").port() == 20);

        BEAST_THROWS(u.set_port_string("12bad"), system_error);
        BEAST_THROWS(u.set_port_string("9999999"), system_error);
    }

    void
    testHost()
    {
        {
            value u;
            BEAST_EXPECT(u.encoded_host() == "");
            u.set_hostname("example.com");
            BEAST_EXPECT(u.encoded_host() == "example.com");
            u.set_port(443);
            BEAST_EXPECT(u.encoded_host() == "example.com:443");
            u.set_hostname({});
            BEAST_EXPECT(u.encoded_host() == ":443");
        }

        {
            value u;
            u.set_encoded_host("example.com");
            BEAST_EXPECT(u.encoded_host() == "example.com");
            u.set_encoded_host("local%20host");
            BEAST_EXPECT(u.encoded_host() == "local%20host");
            BEAST_EXPECT(u.hostname() == "local host");
        }

        {
            value u;
            u.set_encoded_host("example.com:443");
            BEAST_EXPECT(u.encoded_host() == "example.com:443");
            u.set_encoded_host("local%20host:443");
            BEAST_EXPECT(u.encoded_host() == "local%20host:443");
            BEAST_EXPECT(u.hostname() == "local host");
            BEAST_EXPECT(*u.port() == 443);
        }
    }

    //------------------------------------------------------

    void
    testAuthority()
    {
        value u;
        BEAST_EXPECT(u.encoded_authority() == "");
        u.set_username("user");
        BEAST_EXPECT(u.encoded_authority() == "user@");
        u.set_password("password");
        BEAST_EXPECT(u.encoded_authority() == "user:password@");
        u.set_hostname("example.com");
        BEAST_EXPECT(u.encoded_authority() == "user:password@example.com");
        u.set_port(443);
        BEAST_EXPECT(u.encoded_authority() == "user:password@example.com:443");
        u.set_password({});
        BEAST_EXPECT(u.encoded_authority() == "user@example.com:443");
        u.set_username({});
        BEAST_EXPECT(u.encoded_authority() == "example.com:443");
        BEAST_EXPECT(u.encoded_authority() == u.encoded_host());
        u.set_port({});
        BEAST_EXPECT(u.encoded_authority() == "example.com");
        BEAST_EXPECT(u.encoded_authority() == u.encoded_host());
    }

    void
    testOrigin()
    {
        value u;
        u.set_scheme("http");
        u.set_encoded_userinfo("user:password");
        u.set_encoded_host("example.com:443");
        BEAST_EXPECT(u.encoded_origin() == "http://example.com:443");

        log << u.encoded_href() << "\n";
    }

    //------------------------------------------------------

    void
    run() override
    {
        testScheme();

        testUsername();
        testPassword();
        testUserinfo();

        testHostname();
        testPort();
        testHost();

        testAuthority();
        testOrigin();
    }
};

BEAST_DEFINE_TESTSUITE(boost,url,value);

} // url
} // boost
