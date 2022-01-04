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
#include <boost/url/grammar/parse.hpp>
#include <boost/url/rfc/uri_reference_rule.hpp>
#include "test_rule.hpp"
#include "test_suite.hpp"
#include <sstream>

namespace boost {
namespace urls {

class url_view_test
{
public:
    void
    testSpecialMembers()
    {
        // url_view()
        // ~url_view()
        {
            url_view u;
            BOOST_TEST(u.empty());
            BOOST_TEST(u.size() == 0);
        }

        // url_view(url_view const&)
        {
            url_view u1("x://y/z?#");
            url_view u2(u1);
            BOOST_TEST(u2.data() == u1.data());
            BOOST_TEST(u2.size() == u1.size());
        }

        // operator=(url_view const&)
        {
            url_view u1("x://y/z?#");
            url_view u2;
            u2 = u1;
            BOOST_TEST(u2.data() == u1.data());
        }

        // url_view(string_view)
        {
            BOOST_TEST_NO_THROW(url_view(
                "http://example.com/path/to/file.txt?#"));
            BOOST_TEST_THROWS(url_view("{}"),
                std::invalid_argument);
        }
    }

    void
    testObservers()
    {
        // max_size()
        {
            BOOST_TEST(
                url_view::max_size() > 0);

            url_view u;
            BOOST_TEST(u.max_size() > 0);
        }

        // size()
        {
            url_view u;
            BOOST_TEST(u.size() == 0);
            u = url_view("/");
            BOOST_TEST(u.size() == 1);
        }

        // empty()
        {
            url_view u;
            BOOST_TEST(u.empty());
            u = url_view("/");
            BOOST_TEST(! u.empty());
        }

        // data()
        {
            string_view s = "/index.htm";
            url_view u(s);
            BOOST_TEST(u.data() != nullptr);
            BOOST_TEST(u.data() == s.data());
        }

        // begin()
        // end()
        {
            string_view s = "/index.htm";
            url_view u(s);
            BOOST_TEST(u.begin() == s.data());
            BOOST_TEST(u.begin() == u.data());
            BOOST_TEST(u.end() != u.begin());
            BOOST_TEST(u.end()[-1] == 'm');
        }

        // string()
        {
            string_view s = "/index.htm";
            url_view u = parse_relative_ref(s).value();
            BOOST_TEST(u.string() == s);
            BOOST_TEST(u.string().data() == s.data());
        }

        // collect()
        {
        std::shared_ptr<url_view const> sp;
        {
            std::string s( "http://example.com" );
            url_view u( s );                        // u references characters in s

            assert( u.data() == s.data() );         // same buffer

            sp = u.collect();

            assert( sp->data() != s.data() );       // different buffer
            assert( sp->string() == s);        // same contents

            // s is destroyed and thus u
            // becomes invalid, but sp remains valid.
        }
        }
    }

    void
    testScheme()
    {
        auto const good = [](
            string_view s,
            char const* m = nullptr,
            scheme id = scheme::unknown)
        {
            result<url_view> r =
                parse_uri_reference(s);
            if(! BOOST_TEST(r))
                return;
            url_view u = r.value();
            if(m)
            {
                BOOST_TEST(u.scheme() ==
                    string_view(m));
                BOOST_TEST(
                    u.scheme_id() == id);
            }
            else
            {
                BOOST_TEST(! u.has_scheme());
                BOOST_TEST(u.scheme_id() ==
                    scheme::none);
            }
        };

        auto const bad = [](
            string_view s)
        {
            result<url_view> r =
                parse_uri_reference(s);
            BOOST_TEST(r.has_error());
        };

        good("http://", "http", scheme::http);
        good("ou812://", "ou812");
        good("/x");

        bad("1x:");
    }

    void
    testAuthority()
    {
        auto const no =
            [](string_view s)
        {
            BOOST_TEST_NO_THROW([s]
            {
                url_view u(s);
                BOOST_TEST(! u.has_authority());
            }());
        };
        auto const yes =
            [](string_view s, string_view m)
        {
            BOOST_TEST_NO_THROW(
            [&]{
                url_view u(s);
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

        {
            url_view u("http:/path");
            BOOST_TEST(u.encoded_host() == "");
        }

        // Docs
        assert( url_view( "http://www.example.com/index.htm" ).has_authority() == true );

        assert( url_view( "//" ).has_authority() == true );

        assert( url_view( "/file.txt" ).has_authority() == false );
    }

    void
    testUserinfo()
    {
        auto const no =
            [](string_view s)
        {
            BOOST_TEST_NO_THROW(
            [s]{
                url_view u(s);
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
                url_view u(s);
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
            url_view u("x://@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "");
            BOOST_TEST(u.userinfo() == "");
            BOOST_TEST(u.encoded_user() == "");
            BOOST_TEST(u.user() == "");
            BOOST_TEST(u.has_password() == false);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        }
        {
            url_view u("x://:@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == ":");
            BOOST_TEST(u.userinfo() == ":");
            BOOST_TEST(u.encoded_user() == "");
            BOOST_TEST(u.user() == "");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        }
        {
            url_view u("x://a%41:@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "a%41:");
            BOOST_TEST(u.encoded_user() == "a%41");
            BOOST_TEST(u.user() == "aA");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        }
        {
            url_view u("x://:b%42@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == ":b%42");
            BOOST_TEST(u.encoded_user() == "");
            BOOST_TEST(u.user() == "");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "b%42");
            BOOST_TEST(u.password() == "bB");
        }
        {
            url_view u("x://a:b@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "a:b");
            BOOST_TEST(u.encoded_user() == "a");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "b");
        }
        {
            url_view u("x://%3a:%3a@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "%3a:%3a");
            BOOST_TEST(u.userinfo() == ":::");
            BOOST_TEST(u.encoded_user() == "%3a");
            BOOST_TEST(u.user() == ":");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "%3a");
            BOOST_TEST(u.password() == ":");
        }
        {
            url_view u("x://%2525@");
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "%2525");
            BOOST_TEST(u.userinfo() == "%25");
            BOOST_TEST(u.encoded_user() == "%2525");
            BOOST_TEST(u.user() == "%25");
            BOOST_TEST(u.has_password() == false);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        }
    }

    void
    testHost()
    {
        {
            url_view u("res:foo/");
            BOOST_TEST(u.host_type() ==
                host_type::none);
            BOOST_TEST(u.encoded_host() ==
                "");
            BOOST_TEST(u.ipv4_address()
                == ipv4_address());
            BOOST_TEST(u.ipv6_address()
                == ipv6_address());
            BOOST_TEST(
                u.ipv_future() == "");
        }
        {
            url_view u("http://");
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST(u.encoded_host() ==
                "");
        }
        {
            url_view u("http:///");
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST(u.encoded_host() ==
                "");
        }
        {
            url_view u("http://www.example.com/");
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST(u.encoded_host() ==
                "www.example.com");
            BOOST_TEST(u.host() ==
                "www.example.com");
        }
        {
            url_view u("http://192.168.0.1/");
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
            url_view u(
                "http://[1::6:192.168.0.1]:8080/");
            BOOST_TEST(u.host_type() ==
                host_type::ipv6);
            BOOST_TEST(u.encoded_host() ==
                "[1::6:192.168.0.1]");
            BOOST_TEST(u.host() ==
                "[1::6:192.168.0.1]");
            BOOST_TEST(u.ipv6_address() ==
                ipv6_address("1::6:c0a8:1"));
        }
        {
            url_view u("http://[v1.x]:8080/");
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
            url_view u("http://");
            BOOST_TEST(! u.has_port());
            BOOST_TEST(u.port() == "");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            url_view u("http://www");
            BOOST_TEST(! u.has_port());
            BOOST_TEST(u.port() == "");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            url_view u("http://:");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            url_view u("http://:0");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "0");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            url_view u("http://:42");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "42");
            BOOST_TEST(u.port_number() == 42);
        }
        {
            url_view u("http://:00000");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "00000");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            url_view u("http://:000001");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "000001");
            BOOST_TEST(u.port_number() == 1);
        }
        {
            url_view u("http://:65535");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "65535");
            BOOST_TEST(u.port_number() == 65535);
        }
        {
            url_view u("http://:65536");
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "65536");
            BOOST_TEST(u.port_number() == 0);
        }
    }

    void
    testHostAndPort()
    {
        {
            url_view u("http://x:1");
            BOOST_TEST(u.encoded_host_and_port() ==
                "x:1");
        }
        {
            url_view u("http://x%3a:1");
            BOOST_TEST(u.encoded_host_and_port() ==
                "x%3a:1");
        }
        {
            url_view u("http://:1");
            BOOST_TEST(u.encoded_host_and_port() ==
                ":1");
        }
        {
            url_view u("http://:000001");
            BOOST_TEST(u.encoded_host_and_port() ==
                ":000001");
        }
        {
            url_view u("http://xyz:99999");
            BOOST_TEST(u.encoded_host_and_port() ==
                "xyz:99999");
        }
    }

    void
    testOrigin()
    {
        BOOST_TEST(url_view(
            "x://p:q@a.b.c/f.z?a=b#frag"
                ).encoded_origin() == "x://p:q@a.b.c");
        BOOST_TEST(url_view(
            "/file.txt").encoded_origin() == "");
        BOOST_TEST(url_view("x:/path/file/txt"
            ).encoded_origin() == "");
    }

    void
    testPath()
    {
        {
            url_view u;
            BOOST_TEST_NO_THROW(u =
                url_view("/path/to/file.htm"));
            BOOST_TEST(u.encoded_path() ==
                "/path/to/file.htm");
            auto const p = u.encoded_segments();
            BOOST_TEST(! p.empty());
            BOOST_TEST(p.size() == 3);
            auto it = p.begin();
            BOOST_TEST(*it == "path");
            ++it;
            BOOST_TEST(*it == "to");
            ++it;
            BOOST_TEST(*it == "file.htm");
            ++it;
            BOOST_TEST(it == p.end());
        }
    }

    void
    testQuery()
    {
        {
            url_view u("http://");
            BOOST_TEST(! u.has_query());
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.query() == "");
        }
        {
            url_view u("http://?");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.query() == "");
        }
        {
            url_view u("http://?k");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "k");
            BOOST_TEST(u.query() == "k");
        }
        {
            url_view u("http://?k=");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "k=");
            BOOST_TEST(u.query() == "k=");
        }
        {
            url_view u("http://?#");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.query() == "");
        }
        {
            url_view u("http://?%3f");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "%3f");
            BOOST_TEST(u.query() == "?");
        }
        {
            url_view u("http://?%25");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "%25");
            BOOST_TEST(u.query() == "%");
        }
        {
            url_view u("http://?&");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "&");
            BOOST_TEST(u.query() == "&");
        }
        {
            url_view u("http://?%26");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "%26");
            BOOST_TEST(u.query() == "&");
        }
        {
            url_view u("http://?a%3db%26");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "a%3db%26");
            BOOST_TEST(u.query() == "a=b&");
        }

        // VFALCO TODO test params()
    }

    void
    testFragment()
    {
        auto const good = [](
            string_view s,
            char const* encoded = nullptr,
            string_view plain = {})
        {
            result<url_view> r =
                parse_uri_reference(s);
            if(! BOOST_TEST(r))
                return;
            url_view u = r.value();
            if(encoded)
            {
                BOOST_TEST(u.has_fragment());
                BOOST_TEST(u.encoded_fragment() ==
                    string_view(encoded));
                BOOST_TEST(u.fragment() == plain);
            }
            else
            {
                BOOST_TEST(! u.has_fragment());
            }
        };

        auto const bad = [](
            string_view s)
        {
            result<url_view> r =
                parse_uri_reference(s);
            BOOST_TEST(r.has_error());
        };

        good("");
        good("#", "", "");
        good("/#", "", "");
        good("/#A", "A", "A");
        good("/#%41", "%41", "A");
        good("/?#%41", "%41", "A");
        good("#/?:@!$&'()*+,;=",
              "/?:@!$&'()*+,;=",
              "/?:@!$&'()*+,;=");

        bad("#%%");

        // coverage
        {
            url_view u;
            BOOST_TEST(
                u.encoded_fragment() == "");
            BOOST_TEST(u.fragment() == "");
        }
    }

    //--------------------------------------------

    void
    testParse()
    {
        // parse_absolute_uri
        {
            result<url_view> r;

            r = parse_absolute_uri(
                "http://user:pass@example.com:443/path/to/file.txt?q");
            BOOST_TEST(r.has_value());
            BOOST_TEST_NO_THROW(r.value());

            r = parse_absolute_uri("");
            BOOST_TEST(r.has_error());
            BOOST_TEST_THROWS(
                r.value(), std::exception);
        }

        // parse_uri
        {
            result<url_view> r;

            r = parse_uri(
                "http://user:pass@example.com:443/path/to/file.txt?q#f");
            BOOST_TEST(r.has_value());
            BOOST_TEST_NO_THROW(r.value());

            r = parse_uri("");
            BOOST_TEST(r.has_error());
            BOOST_TEST_THROWS(
                r.value(), std::exception);
        }

        // parse_relative_ref
        {
            result<url_view> r;

            r = parse_relative_ref(
                "//example.com/path/to/file.txt?q#f");
            BOOST_TEST(r.has_value());
            BOOST_TEST_NO_THROW(r.value());

            r = parse_relative_ref("http:file.txt");
            BOOST_TEST(r.has_error());
            BOOST_TEST_THROWS(
                r.value(), std::exception);
        }

        // parse_uri_reference
        {
            result<url_view> r;

            r = parse_uri_reference(
                "http://user:pass@example.com:443/path/to/file.txt?q#f");
            BOOST_TEST(r.has_value());
            BOOST_TEST_NO_THROW(r.value());

            r = parse_uri_reference(
                "//example.com/path/to/file.txt?q#f");
            BOOST_TEST(r.has_value());

            r = parse_uri_reference("");
            BOOST_TEST(r.has_value());
            BOOST_TEST_NO_THROW(r.value());

            r = parse_uri_reference("1000://");
            BOOST_TEST(r.has_error());
            BOOST_TEST_THROWS(r.value(),
                std::exception);
        }

    }

    void
    testOutput()
    {
        url_view u( "http://example.com" );
        std::stringstream ss;
        ss << u;
        BOOST_TEST(
            ss.str() == "http://example.com");
    }

    void
    testCases()
    {
        BOOST_TEST_NO_THROW(url_view(
            "javascript:alert(1)"));
    }

    void
    run()
    {
        testSpecialMembers();
        testObservers();
        testScheme();
        testAuthority();
        testUserinfo();
        testHost();
        testPort();
        testHostAndPort();
        testOrigin();
        testPath();
        testQuery();
        testFragment();

        testParse();
        testOutput();
        testCases();
    }
};

TEST_SUITE(
    url_view_test,
    "boost.url.url_view");

} // urls
} // boost
