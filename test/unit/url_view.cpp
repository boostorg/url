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
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/uri_reference_bnf.hpp>
#include "test_bnf.hpp"
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

        // url_view(string_view)
        {
            url_view u("http://example.com/path/to/file.txt?#");
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
            u = parse_relative_ref("/").value();
            BOOST_TEST(u.size() == 1);
        }

        // empty()
        {
            url_view u;
            BOOST_TEST(u.empty());
            u = parse_relative_ref("/").value();
            BOOST_TEST(! u.empty());
        }

        // data()
        {
            string_view s = "/index.htm";
            url_view u = parse_relative_ref(s).value();
            BOOST_TEST(u.data() != nullptr);
            BOOST_TEST(u.data() == s.data());
        }

        // at(std::size_t)
        {
            string_view s = "/index.htm";
            url_view u = parse_relative_ref(s).value();
            BOOST_TEST(u.at(0) == '/');
            BOOST_TEST(u.at(9) == 'm');
            BOOST_TEST_THROWS(u.at(10),
                std::out_of_range);
        }

        // operator[](std::size_t)
        {
            string_view s = "/index.htm";
            url_view u = parse_relative_ref(s).value();
            BOOST_TEST(u[0] == '/');
            BOOST_TEST(u[9] == 'm');
            BOOST_TEST(&u[3] == &s[3]);
        }

        // begin()
        // end()
        {
            string_view s = "/index.htm";
            url_view u = parse_relative_ref(s).value();
            BOOST_TEST(u.begin() == s.data());
            BOOST_TEST(u.begin() == u.data());
            BOOST_TEST(u.end() != u.begin());
            BOOST_TEST(u.end()[-1] == 'm');
        }

        // encoded_url()
        {
            string_view s = "/index.htm";
            url_view u = parse_relative_ref(s).value();
            BOOST_TEST(u.encoded_url() == s);
            BOOST_TEST(u.encoded_url().data() == s.data());
        }

        // collect()
        {
        std::shared_ptr<url_view const> sp;
        {
            std::string s( "http://example.com" );
            url_view u = parse_uri( s ).value();    // u references characters in s

            assert( u.data() == s.data() );         // same buffer

            sp = u.collect();

            assert( sp->data() != s.data() );       // different buffer
            assert( sp->encoded_url() == s);        // same contents

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
            BOOST_TEST_NO_THROW(
            [s]{
                auto u = parse_uri(s).value();
                BOOST_TEST(! u.has_authority());
            }());
        };
        auto const yes =
            [](string_view s, string_view m)
        {
            BOOST_TEST_NO_THROW(
            [&]{
                auto u = parse_uri(s).value();
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
            auto u = parse_uri("http:/path").value();
            BOOST_TEST(u.encoded_host() == "");
        }

        // Docs
        assert( parse_uri( "http://www.example.com/index.htm" ).value().has_authority() == true );

        assert( parse_relative_ref( "//" ).value().has_authority() == true );

        assert( parse_relative_ref( "/file.txt" ).value().has_authority() == false );
    }

    void
    testUserinfo()
    {
        auto const no =
            [](string_view s)
        {
            BOOST_TEST_NO_THROW(
            [s]{
                auto u = parse_uri(s).value();
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
                auto u = parse_uri(s).value();
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
            auto u = parse_uri("x://@").value();
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
            auto u = parse_uri("x://:@").value();
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
            auto u = parse_uri("x://a%41:@").value();
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "a%41:");
            BOOST_TEST(u.encoded_user() == "a%41");
            BOOST_TEST(u.user() == "aA");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        }
        {
            auto u = parse_uri("x://:b%42@").value();
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == ":b%42");
            BOOST_TEST(u.encoded_user() == "");
            BOOST_TEST(u.user() == "");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "b%42");
            BOOST_TEST(u.password() == "bB");
        }
        {
            auto u = parse_uri("x://a:b@").value();
            BOOST_TEST(u.has_userinfo());
            BOOST_TEST(u.encoded_userinfo() == "a:b");
            BOOST_TEST(u.encoded_user() == "a");
            BOOST_TEST(u.has_password() == true);
            BOOST_TEST(u.encoded_password() == "b");
        }
        {
            auto u = parse_uri("x://%3a:%3a@").value();
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
            auto u = parse_uri("x://%2525@").value();
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
            auto u = parse_uri("res:foo/").value();
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
            auto u = parse_uri("http://").value();
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST(u.encoded_host() ==
                "");
        }
        {
            auto u = parse_uri("http:///").value();
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST(u.encoded_host() ==
                "");
        }
        {
            auto u = parse_uri("http://www.example.com/").value();
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST(u.encoded_host() ==
                "www.example.com");
            BOOST_TEST(u.host() ==
                "www.example.com");
        }
        {
            auto u = parse_uri("http://192.168.0.1/").value();
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
                "http://[1::6:192.168.0.1]:8080/").value();
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
            auto u = parse_uri(
                "http://[v1.x]:8080/").value();
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
            auto u = parse_uri("http://").value();
            BOOST_TEST(! u.has_port());
            BOOST_TEST(u.port() == "");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            auto u = parse_uri("http://www").value();
            BOOST_TEST(! u.has_port());
            BOOST_TEST(u.port() == "");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            auto u = parse_uri("http://:").value();
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            auto u = parse_uri("http://:0").value();
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "0");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            auto u = parse_uri("http://:42").value();
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "42");
            BOOST_TEST(u.port_number() == 42);
        }
        {
            auto u = parse_uri("http://:00000").value();
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "00000");
            BOOST_TEST(u.port_number() == 0);
        }
        {
            auto u = parse_uri("http://:000001").value();
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "000001");
            BOOST_TEST(u.port_number() == 1);
        }
        {
            auto u = parse_uri("http://:65535").value();
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "65535");
            BOOST_TEST(u.port_number() == 65535);
        }
        {
            auto u = parse_uri("http://:65536").value();
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port() == "65536");
            BOOST_TEST(u.port_number() == 0);
        }
    }

    void
    testHostAndPort()
    {
        {
            auto u = parse_uri("http://x:1").value();
            BOOST_TEST(u.encoded_host_and_port() ==
                "x:1");
        }
        {
            auto u = parse_uri("http://x%3a:1").value();
            BOOST_TEST(u.encoded_host_and_port() ==
                "x%3a:1");
        }
        {
            auto u = parse_uri("http://:1").value();
            BOOST_TEST(u.encoded_host_and_port() ==
                ":1");
        }
        {
            auto u = parse_uri("http://:000001").value();
            BOOST_TEST(u.encoded_host_and_port() ==
                ":000001");
        }
        {
            auto u = parse_uri("http://xyz:99999").value();
            BOOST_TEST(u.encoded_host_and_port() ==
                "xyz:99999");
        }
    }

    void
    testOrigin()
    {
        BOOST_TEST(parse_uri(
            "x://p:q@a.b.c/f.z?a=b#frag"
                ).value().encoded_origin() == "x://p:q@a.b.c");
        BOOST_TEST(parse_relative_ref(
            "/file.txt").value().encoded_origin() == "");
        BOOST_TEST(parse_uri("x:/path/file/txt"
            ).value().encoded_origin() == "");
    }

    void
    testPath()
    {
        {
            url_view u;
            BOOST_TEST_NO_THROW(
                u = parse_relative_ref(
                    "/path/to/file.htm").value());
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
            auto u = parse_uri("http://").value();
            BOOST_TEST(! u.has_query());
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.query() == "");
        }
        {
            auto u = parse_uri("http://?").value();
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.query() == "");
        }
        {
            auto u = parse_uri("http://?k").value();
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "k");
            BOOST_TEST(u.query() == "k");
        }
        {
            auto u = parse_uri("http://?k=").value();
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "k=");
            BOOST_TEST(u.query() == "k=");
        }
        {
            auto u = parse_uri("http://?#").value();
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.query() == "");
        }
        {
            auto u = parse_uri("http://?%3f").value();
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "%3f");
            BOOST_TEST(u.query() == "?");
        }
        {
            auto u = parse_uri("http://?%25").value();
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "%25");
            BOOST_TEST(u.query() == "%");
        }
        {
            auto u = parse_uri("http://?&").value();
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "&");
            BOOST_TEST(u.query() == "&");
        }
        {
            auto u = parse_uri("http://?%26").value();
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "%26");
            BOOST_TEST(u.query() == "&");
        }
        {
            auto u = parse_uri("http://?a%3db%26").value();
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

            r = parse_absolute_uri(
                "");
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

            r = parse_uri(
                "");
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

            r = parse_relative_ref(
                "http:file.txt");
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
                "//example.cmo/path/to/file.txt?q#f");
            BOOST_TEST(r.has_value());

            r = parse_uri_reference(
                "");
            BOOST_TEST(r.has_value());
            BOOST_TEST_NO_THROW(r.value());

            r = parse_uri_reference(
                "1000://");
            BOOST_TEST(r.has_error());
            BOOST_TEST_THROWS(
                r.value(), std::exception);
        }

    }

    void
    testOutput()
    {
        auto u = parse_uri( "http://example.com" ).value();
        std::stringstream ss;
        ss << u;
        BOOST_TEST(
            ss.str() == "http://example.com");
    }

    void
    testCases()
    {
        BOOST_TEST(is_valid<uri_reference_bnf>(
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
