//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/url.hpp>

#include <boost/url/url_view.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

class url_test
{
public:
#if 0
    test_suite::log_type log;

    void
    dump(url const& u)
    {
        (void)u;
        log <<
            "href     : " << u.str() << "\n"
            "scheme   : " << u.scheme() << "\n"
            "user     : " << u.encoded_user() << "\n"
            "password : " << u.encoded_password() << "\n"
            "hostname : " << u.encoded_host() << "\n"
            "port     : " << u.port() << "\n" <<
            "path     : " << u.encoded_path() << "\n"
            "query    : " << u.encoded_query() << "\n"
            "fragment : " << u.encoded_fragment() << "\n"
            //"resource : " << u.encoded_resource() << "\n"
            ;
        log.flush();
    }

    void
    testObservers()
    {
        BOOST_TEST(url("/").size() == 1);
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
            BOOST_TEST(v.str() == "http://user:pass@example.com:80/path/to/file.txt?k1=v1&k2=v2");
            BOOST_TEST(v.encoded_origin() == "http://user:pass@example.com:80");
            BOOST_TEST(v.encoded_authority() == "user:pass@example.com:80");
            BOOST_TEST(v.scheme() == "http");
            BOOST_TEST(v.encoded_user() == "user");
            BOOST_TEST(v.encoded_password() == "pass");
            BOOST_TEST(v.encoded_userinfo() == "user:pass");
            BOOST_TEST(v.encoded_host() == "example.com");
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
        BOOST_TEST(url().str() == "");
    }

    void
    testOrigin()
    {
        BOOST_TEST(url().encoded_origin() == "");
        BOOST_TEST(url("http://user:pass@example.com/path/to/file.txt?q").encoded_origin() ==
                         "http://user:pass@example.com");
        BOOST_TEST(url("http://user:pass@example.com/path/to/file.txt?q"
            ).set_encoded_origin("ws://x.com").str() == "ws://x.com/path/to/file.txt?q");
        BOOST_TEST(
            url("http://host:80/")
            .set_encoded_origin("http://host:443/")
            .port() == "443");
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
        BOOST_TEST(url("").set_encoded_path("").str() == "");
        BOOST_TEST(url("//x#").set_encoded_path("").str() == "//x#");

        // path-abempty
        BOOST_TEST(url("//x#").set_encoded_path("/").str() == "//x/#");
        BOOST_TEST(url("//x#").set_encoded_path("//").str() == "//x//#");
        BOOST_TEST(url("//x#").set_encoded_path("/y").str() == "//x/y#");
        BOOST_TEST_THROWS(url("//x#").set_encoded_path("x"), invalid_part);
        BOOST_TEST_THROWS(url("//x#").set_encoded_path("x/"), invalid_part);
        BOOST_TEST_THROWS(url("//x#").set_encoded_path("/%A"), invalid_part);
        BOOST_TEST_THROWS(url("//x#").set_encoded_path("/#"), invalid_part);

        // path-absolute
        BOOST_TEST(url("?#").set_encoded_path("/x").str() == "/x?#");
        BOOST_TEST(url("x:?#").set_encoded_path("/").str() == "x:/?#");
        BOOST_TEST_THROWS(url("?").set_encoded_path("//x"), invalid_part);
        BOOST_TEST_THROWS(url("?").set_encoded_path("/x%A"), invalid_part);
        BOOST_TEST_THROWS(url("x:?#").set_encoded_path("/x?"), invalid_part);
        BOOST_TEST_THROWS(url("/x/%"), invalid_part);

        // path-noscheme
        BOOST_TEST(url("").set_encoded_path("x").str() == "x");
        BOOST_TEST(url("").set_encoded_path("x/").str() == "x/");
        BOOST_TEST(url("").set_encoded_path("x//").str() == "x//");
        BOOST_TEST(url("?#").set_encoded_path("x").str() == "x?#");
        BOOST_TEST(url("?#").set_encoded_path("x/").str() == "x/?#");
        BOOST_TEST(url("?#").set_encoded_path("x//").str() == "x//?#");
        BOOST_TEST(url("yz/?#").set_encoded_path("x").str() == "x?#");
        BOOST_TEST(url("yz/?#").set_encoded_path("x/").str() == "x/?#");
        BOOST_TEST(url("yz/?#").set_encoded_path("x//").str() == "x//?#");
        BOOST_TEST_THROWS(url("yz/?#").set_encoded_path(":"), invalid_part);
        BOOST_TEST_THROWS(url("yz/?#").set_encoded_path("x:"), invalid_part);
        BOOST_TEST_THROWS(url("yz/?#").set_encoded_path("x:/q"), invalid_part);
        BOOST_TEST_THROWS(url("y/%"), invalid_part);

        // path-rootless
        BOOST_TEST(url("x:?#").set_encoded_path("y").str() == "x:y?#");
        BOOST_TEST(url("x:?#").set_encoded_path("y/").str() == "x:y/?#");
        BOOST_TEST(url("x:?#").set_encoded_path("y//").str() == "x:y//?#");
        BOOST_TEST_THROWS(url("x:?#").set_encoded_path("%A"), invalid_part);
        BOOST_TEST_THROWS(url("x:?#").set_encoded_path("y?"), invalid_part);
        BOOST_TEST_THROWS(url("x:y/%"), invalid_part);
    }

    void
    testSegments()
    {
        // path() const
        {
            url const v("/path/to/file.txt");
            auto ps = v.path();
            // ?
        }

        {
            url v("/path/to/file.txt");
            auto ps = v.path();
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
            auto ps = u.path();
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

        BOOST_TEST(url("//?").set_query("").str() == "//");
        BOOST_TEST(url("//?x").set_query("").str() == "//");
        BOOST_TEST(url("//?xy").set_query("y").str() == "//?y");
        BOOST_TEST(url("//").set_query("?").str() == "//??");
        BOOST_TEST(url("//").set_query("??").str() == "//???");

        BOOST_TEST(url("//?").set_encoded_query("").str() == "//");
        BOOST_TEST(url("//?x").set_encoded_query("").str() == "//");
        BOOST_TEST(url("//?xy").set_encoded_query("y").str() == "//?y");
        BOOST_TEST_THROWS(url("//").set_encoded_query("#"), invalid_part);
        BOOST_TEST_THROWS(url("//").set_encoded_query("#?"), invalid_part);

        BOOST_TEST(url("//?").set_query_part("").str() == "//");
        BOOST_TEST(url("//?x").set_query_part("").str() == "//");
        BOOST_TEST_THROWS(url("//?xy").set_query_part("y"), invalid_part);
        BOOST_TEST(url("//?xy").set_query_part("?y").str() == "//?y");

        BOOST_TEST_THROWS(url("?%"), invalid_part);
        BOOST_TEST(url("?x=").str() == "?x=");
        BOOST_TEST_THROWS(url("?x=%"), invalid_part);
        BOOST_TEST(url("?x=#").str() == "?x=#");
    }

    void
    testParams()
    {
        // params() const
        {
            url const v("?x=1&y=2&y=3&z");
            auto qp = v.query_params();
            static_assert(
                std::is_same<decltype(qp),
                    query_params_view>::value, "");
        }

        {
            url v("?x=1&y=2&y=3&z");
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

        BOOST_TEST(url("").fragment() == "");
        BOOST_TEST(url("#").fragment() == "#");
        BOOST_TEST(url("#x").fragment() == "#x");

        BOOST_TEST(url().set_fragment("").fragment() == "");
        BOOST_TEST(url().set_fragment("#").fragment() == "#%23");
        BOOST_TEST(url().set_fragment("#x").fragment() == "#%23x");

        BOOST_TEST(url().set_encoded_fragment("").fragment() == "");
        BOOST_TEST(url().set_encoded_fragment("x").fragment() == "#x");
        BOOST_TEST(url().set_encoded_fragment("%23").fragment() == "#%23");
        BOOST_TEST_THROWS(url().set_encoded_fragment("#"), invalid_part);
        BOOST_TEST_THROWS(url().set_encoded_fragment("#x"), invalid_part);

        BOOST_TEST(url().set_fragment_part("").fragment() == "");
        BOOST_TEST(url().set_fragment_part("#").fragment() == "#");
        BOOST_TEST(url().set_fragment_part("#x").fragment() == "#x");
        BOOST_TEST(url().set_fragment_part("#%23x").fragment() == "#%23x");
        BOOST_TEST_THROWS(url().set_fragment_part("x"), invalid_part);
        BOOST_TEST_THROWS(url().set_fragment_part("%23"), invalid_part);

        BOOST_TEST(url("//#").set_fragment("").str() == "//");
        BOOST_TEST(url("//#x").set_fragment("").str() == "//");
        BOOST_TEST(url("//#xy").set_fragment("y").str() == "//#y");
        BOOST_TEST(url("//").set_fragment("#").str() == "//#%23");
        BOOST_TEST(url("//").set_fragment("##").str() == "//#%23%23");

        BOOST_TEST(url("//#").set_encoded_fragment("").str() == "//");
        BOOST_TEST(url("//#x").set_encoded_fragment("").str() == "//");
        BOOST_TEST(url("//#xy").set_encoded_fragment("y").str() == "//#y");
        BOOST_TEST_THROWS(url("//").set_encoded_fragment("#"), invalid_part);
        BOOST_TEST_THROWS(url("//").set_encoded_fragment("##"), invalid_part);

        BOOST_TEST(url("//#").set_fragment_part("").str() == "//");
        BOOST_TEST(url("//#x").set_fragment_part("").str() == "//");
        BOOST_TEST_THROWS(url("//#xy").set_fragment_part("y"), invalid_part);
        BOOST_TEST(url("//#xy").set_fragment_part("#y").str() == "//#y");

        BOOST_TEST_THROWS(url("#%"), invalid_part);
    }

    //------------------------------------------------------

    void
    testNormalize()
    {
        BOOST_TEST(url("").normalize_scheme().str() == "");
        BOOST_TEST(url("/").normalize_scheme().str() == "/");
        BOOST_TEST(url("http://").normalize_scheme().str() == "http://");
        BOOST_TEST(url("Http://").normalize_scheme().str() == "http://");
        BOOST_TEST(url("HTtp://").normalize_scheme().str() == "http://");
        BOOST_TEST(url("HTTp://").normalize_scheme().str() == "http://");
        BOOST_TEST(url("HTTP://").normalize_scheme().str() == "http://");
    }

    //------------------------------------------------------
#endif

    //--------------------------------------------

    void
    testScheme()
    {
        {
            url u;
            u.reserve(40);
            BOOST_TEST(
                u.set_scheme("http").str() == "http:");
        }

        url u;
        BOOST_TEST(u.set_scheme("").str() == "");
        BOOST_TEST(u.set_scheme(scheme::none).str() == "");
        BOOST_TEST(u.set_scheme("http").str() == "http:");
        BOOST_TEST(u.scheme_id() == scheme::http);
        BOOST_TEST_THROWS(
            u.set_scheme("http:"), std::invalid_argument);
        BOOST_TEST(u.str() == "http:");
        BOOST_TEST(u.scheme_id() == scheme::http);
        BOOST_TEST_THROWS(
            u.set_scheme("1http"), std::invalid_argument);
        BOOST_TEST_THROWS(
            u.set_scheme(scheme::unknown), std::invalid_argument);
        BOOST_TEST(u.scheme_id() == scheme::http);
        BOOST_TEST(u.str() == "http:");
        BOOST_TEST(u.scheme_id() == scheme::http);
        BOOST_TEST(u.set_scheme("ftp").str() == "ftp:");
        BOOST_TEST(u.scheme_id() == scheme::ftp);
        BOOST_TEST(u.set_scheme(scheme::none).str() == "");
        BOOST_TEST(u.scheme_id() == scheme::none);
        BOOST_TEST(u.set_scheme(scheme::ws).str() == "ws:");
        BOOST_TEST(u.scheme_id() == scheme::ws);
        BOOST_TEST(u.set_scheme("").str() == "");
        BOOST_TEST(u.scheme_id() == scheme::none);
        BOOST_TEST(u.set_scheme("x").str() == "x:");
        BOOST_TEST(u.scheme_id() == scheme::unknown);
        u = parse_uri("http:/path/to/file.txt");
        BOOST_TEST(u.set_scheme("").str() == "/path/to/file.txt");
    }

    //--------------------------------------------

    void
    testUser()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(
                u.remove_user().str() == s2);
            BOOST_TEST(u.encoded_user().empty());
            BOOST_TEST(u.user().empty());
        };

        auto const set = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(
                u.set_user(s2).str() == s3);
            BOOST_TEST(u.user() == s2);
            BOOST_TEST(u.has_userinfo());
        };

        auto const enc = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(u.set_encoded_user(
                s2).str() == s3);
            BOOST_TEST(u.encoded_user() == s2);
            BOOST_TEST(u.has_userinfo());
        };

        BOOST_TEST_THROWS(url().set_encoded_user(
            "%2"), std::invalid_argument);

        remove("", "");
        remove("/x", "/x");
        remove("//", "//");
        remove("//x", "//x");
        remove("//@", "//");
        remove("//:@", "//:@");
        remove("//x@", "//");
        remove("//x@z", "//z");
        remove("//x:@", "//:@");
        remove("//x:y@", "//:y@");
        remove("//x:y@z", "//:y@z");

        remove("ws:", "ws:");
        remove("ws:/x", "ws:/x");
        remove("ws://", "ws://");
        remove("ws://x", "ws://x");
        remove("ws://@", "ws://");
        remove("ws://:@", "ws://:@");
        remove("ws://x@", "ws://");
        remove("ws://x@z", "ws://z");
        remove("ws://x:@", "ws://:@");
        remove("ws://x:y@", "ws://:y@");
        remove("ws://x:y@z", "ws://:y@z");

        set("", "", "//@");
        set("/y", "", "//@/y");
        set("//", "", "//@");
        set("//y", "", "//@y");
        set("//@", "", "//@");
        set("//:@", "", "//:@");
        set("//y@", "", "//@");
        set("//y@z", "", "//@z");
        set("//y:@", "", "//:@");
        set("//y:z@", "", "//:z@");
        set("//a:b@c", "", "//:b@c");

        set("ws:", "", "ws://@");
        set("ws:/y", "", "ws://@/y");
        set("ws://", "", "ws://@");
        set("ws://y", "", "ws://@y");
        set("ws://@", "", "ws://@");
        set("ws://:@", "", "ws://:@");
        set("ws://y@", "", "ws://@");
        set("ws://y@z", "", "ws://@z");
        set("ws://y:@", "", "ws://:@");
        set("ws://y:z@", "", "ws://:z@");
        set("ws://a:b@c", "", "ws://:b@c");

        set("", "", "//@");
        set("", "x", "//x@");
        set("/y", "x", "//x@/y");
        set("//", "x", "//x@");
        set("//y", "x", "//x@y");
        set("//@", "x", "//x@");
        set("//:@", "x", "//x:@");
        set("//y@", "x", "//x@");
        set("//y@z", "x", "//x@z");
        set("//y:@", "x", "//x:@");
        set("//y:z@", "x", "//x:z@");
        set("//a:b@c", "x", "//x:b@c");

        set("ws:", "x", "ws://x@");
        set("ws:/y", "x", "ws://x@/y");
        set("ws://", "x", "ws://x@");
        set("ws://y", "x", "ws://x@y");
        set("ws://@", "x", "ws://x@");
        set("ws://:@", "x", "ws://x:@");
        set("ws://y@", "x", "ws://x@");
        set("ws://y@z", "x", "ws://x@z");
        set("ws://y:@", "x", "ws://x:@");
        set("ws://y:z@", "x", "ws://x:z@");
        set("ws://a:b@c", "x", "ws://x:b@c");

        set("ws://a:b@c", ":", "ws://%3a:b@c");
        set("ws://a:b@c", "@", "ws://%40:b@c");

        enc("", "", "//@");
        enc("", "%41", "//%41@");
        enc("/y", "%41", "//%41@/y");
        enc("//", "%41", "//%41@");
        enc("//y", "%41", "//%41@y");
        enc("//@", "%41", "//%41@");
        enc("//:@", "%41", "//%41:@");
        enc("//y@", "%41", "//%41@");
        enc("//y@z", "%41", "//%41@z");
        enc("//y:@", "%41", "//%41:@");
        enc("//y:z@", "%41", "//%41:z@");
        enc("//a:b@c", "%41", "//%41:b@c");

        enc("ws:", "%41", "ws://%41@");
        enc("ws:/y", "%41", "ws://%41@/y");
        enc("ws://", "%41", "ws://%41@");
        enc("ws://y", "%41", "ws://%41@y");
        enc("ws://@", "%41", "ws://%41@");
        enc("ws://:@", "%41", "ws://%41:@");
        enc("ws://y@", "%41", "ws://%41@");
        enc("ws://y@z", "%41", "ws://%41@z");
        enc("ws://y:@", "%41", "ws://%41:@");
        enc("ws://y:z@", "%41", "ws://%41:z@");
        enc("ws://a:b@c", "%41", "ws://%41:b@c");
    }

    //--------------------------------------------

    void
    testPassword()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(
                u.remove_password().str() == s2);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        };

        auto const set = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(
                u.set_password(s2).str() == s3);
            BOOST_TEST(u.password() == s2);
            BOOST_TEST(u.has_userinfo());
        };

        auto const enc = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(u.set_encoded_password(
                s2).str() == s3);
            BOOST_TEST(u.encoded_password() == s2);
            BOOST_TEST(u.has_userinfo());
        };

        BOOST_TEST_THROWS(url().set_encoded_password(
            "%2"), std::invalid_argument);

        remove("", "");
        remove("/", "/");
        remove("//", "//");
        remove("//", "//");
        remove("//@", "//@");
        remove("//y@", "//y@");
        remove("//:y@", "//@");
        remove("//y:z@", "//y@");
        remove("//y:z@a", "//y@a");

        remove("x:", "x:");
        remove("x:/", "x:/");
        remove("x://", "x://");
        remove("x://", "x://");
        remove("x://@", "x://@");
        remove("x://y@", "x://y@");
        remove("x://:y@", "x://@");
        remove("x://y:z@", "x://y@");
        remove("x://y:z@a", "x://y@a");

        set("", "", "//:@");
        set("/", "", "//:@/");
        set("//", "", "//:@");
        set("//@", "", "//:@");
        set("//y@", "", "//y:@");
        set("//:y@", "", "//:@");
        set("//y:z@", "", "//y:@");
        set("//y:z@a", "", "//y:@a");

        set("x:", "", "x://:@");
        set("x:/", "", "x://:@/");
        set("x://", "", "x://:@");
        set("x://@", "", "x://:@");
        set("x://y@", "", "x://y:@");
        set("x://:y@", "", "x://:@");
        set("x://y:z@", "", "x://y:@");
        set("x://y:z@a", "", "x://y:@a");

        set("", "x", "//:x@");
        set("/", "x", "//:x@/");
        set("//", "x", "//:x@");
        set("//x", "y", "//:y@x");
        set("//x@", "y", "//x:y@");
        set("//x:y@", "z", "//x:z@");
        set("//x:abc@", "z", "//x:z@");
        set("//x:z@", "abc", "//x:abc@");

        set("w:", "x", "w://:x@");
        set("w:/", "x", "w://:x@/");
        set("w://", "x", "w://:x@");
        set("w://x", "y", "w://:y@x");
        set("w://x@", "y", "w://x:y@");
        set("w://x:y@", "z", "w://x:z@");
        set("w://x:abc@", "z", "w://x:z@");
        set("w://x:z@", "abc", "w://x:abc@");

        set("w://x:z@", ":", "w://x::@");
        set("w://x:z@", "@", "w://x:%40@");

        enc("", "", "//:@");
        enc("", "%41", "//:%41@");
        enc("/y", "%41", "//:%41@/y");
        enc("//", "%41", "//:%41@");
        enc("//y", "%41", "//:%41@y");
        enc("//@", "%41", "//:%41@");
        enc("//:@", "%41", "//:%41@");
        enc("//y@", "%41", "//y:%41@");
        enc("//y@z", "%41", "//y:%41@z");
        enc("//y:@", "%41", "//y:%41@");
        enc("//y:z@", "%41", "//y:%41@");
        enc("//a:b@c", "%41", "//a:%41@c");

        enc("ws:", "%41", "ws://:%41@");
        enc("ws:/y", "%41", "ws://:%41@/y");
        enc("ws://", "%41", "ws://:%41@");
        enc("ws://y", "%41", "ws://:%41@y");
        enc("ws://@", "%41", "ws://:%41@");
        enc("ws://:@", "%41", "ws://:%41@");
        enc("ws://y@", "%41", "ws://y:%41@");
        enc("ws://y@z", "%41", "ws://y:%41@z");
        enc("ws://y:@", "%41", "ws://y:%41@");
        enc("ws://y:z@", "%41", "ws://y:%41@");
        enc("ws://a:b@c", "%41", "ws://a:%41@c");
    }

    //--------------------------------------------

    void
    testUserinfo()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(u.remove_userinfo().str() == s2);
            BOOST_TEST(u.encoded_userinfo().empty());
            BOOST_TEST(u.userinfo().empty());
            BOOST_TEST(! u.has_userinfo());
        };

        auto const set = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(
                u.set_userinfo(s2).str() == s3);
            BOOST_TEST(u.userinfo() == s2);
            BOOST_TEST(u.has_userinfo());
        };

        auto const enc = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(
                u.set_encoded_userinfo(s2).str() == s3);
            BOOST_TEST(u.encoded_userinfo() == s2);
            BOOST_TEST(u.has_userinfo());
        };

        BOOST_TEST_THROWS(url().set_encoded_userinfo(
            "%2"), std::invalid_argument);
        BOOST_TEST_THROWS(url().set_encoded_userinfo(
            "@"), std::invalid_argument);

        remove("", "");
        remove("/", "/");
        remove("//", "//");
        remove("//@", "//");
        remove("//a@", "//");
        remove("//a:@", "//");
        remove("//a:b@", "//");
        remove("//@x", "//x");
        remove("//a@x", "//x");
        remove("//a:b@x", "//x");
        remove("//a:b@x/", "//x/");

        remove("z:", "z:");
        remove("z:/", "z:/");
        remove("z://", "z://");
        remove("z://@", "z://");
        remove("z://a@", "z://");
        remove("z://a:@", "z://");
        remove("z://a:b@", "z://");
        remove("z://@x", "z://x");
        remove("z://a@x", "z://x");
        remove("z://a:b@x", "z://x");

        set("", "", "//@");
        set("/", "", "//@/");
        set("//", "", "//@");
        set("//@", "", "//@");
        set("//a@", "", "//@");
        set("//a:@", "", "//@");
        set("//a:b@", "", "//@");
        set("//@x", "", "//@x");
        set("//a@x", "", "//@x");
        set("//a:b@x", "", "//@x");
        set("//a:b@x/", "", "//@x/");

        set("w:", "", "w://@");
        set("w:/", "", "w://@/");
        set("w://", "", "w://@");
        set("w://@", "", "w://@");
        set("w://a@", "", "w://@");
        set("w://a:@", "", "w://@");
        set("w://a:b@", "", "w://@");
        set("w://@x", "", "w://@x");
        set("w://a@x", "", "w://@x");
        set("w://a:b@x", "", "w://@x");
        set("w://a:b@x/", "", "w://@x/");

        set("", ":", "//%3a@");
        set("/", "a", "//a@/");
        set("//", "@", "//%40@");
        set("//@", "xyz", "//xyz@");
        set("//a@", ":@", "//%3a%40@");
        set("//a:@", "x", "//x@");
        set("//a:b@", "p:q", "//p%3aq@");
        set("//@x", "z", "//z@x");
        set("//a@x", "42", "//42@x");
        set("//a:b@x", "UV", "//UV@x");
        set("//a:b@x/", "NR", "//NR@x/");

        set("w:", ":", "w://%3a@");
        set("w:/", "a", "w://a@/");
        set("w://", "@", "w://%40@");
        set("w://@", "xyz", "w://xyz@");
        set("w://a@", ":@", "w://%3a%40@");
        set("w://a:@", "x", "w://x@");
        set("w://a:b@", "p:q", "w://p%3aq@");
        set("w://@x", "z", "w://z@x");
        set("w://a@x", "42", "w://42@x");
        set("w://a:b@x", "UV", "w://UV@x");
        set("w://a:b@x/", "NR", "w://NR@x/");

        enc("", "", "//@");
        enc("/", "", "//@/");
        enc("//", "", "//@");
        enc("//@", "", "//@");
        enc("//a@", "", "//@");
        enc("//a:@", "", "//@");
        enc("//a:b@", "", "//@");
        enc("//@x", "", "//@x");
        enc("//a@x", "", "//@x");
        enc("//a:b@x", "", "//@x");
        enc("//a:b@x/", "", "//@x/");

        enc("w:", "", "w://@");
        enc("w:/", "", "w://@/");
        enc("w://", "", "w://@");
        enc("w://@", "", "w://@");
        enc("w://a@", "", "w://@");
        enc("w://a:@", "", "w://@");
        enc("w://a:b@", "", "w://@");
        enc("w://@x", "", "w://@x");
        enc("w://a@x", "", "w://@x");
        enc("w://a:b@x", "", "w://@x");
        enc("w://a:b@x/", "", "w://@x/");

        enc("", ":", "//:@");
        enc("", "%3a", "//%3a@");
        enc("/", "%41", "//%41@/");
        enc("//", "x", "//x@");
        enc("//@", "xyz", "//xyz@");
        enc("//a@", "%3a%40", "//%3a%40@");
        enc("//a:@", "x", "//x@");
        enc("//a:b@", "p:q", "//p:q@");
        enc("//@x", "z", "//z@x");
        enc("//a@x", "42", "//42@x");
        enc("//a:b@x", "UV", "//UV@x");
        enc("//a:b@x/", "NR", "//NR@x/");

        enc("w:", ":", "w://:@");
        enc("w:", "%3a", "w://%3a@");
        enc("w:/", "%41", "w://%41@/");
        enc("w://", "x", "w://x@");
        enc("w://@", "xyz", "w://xyz@");
        enc("w://a@", "%3a%40", "w://%3a%40@");
        enc("w://a:@", "x", "w://x@");
        enc("w://a:b@", "p:q", "w://p:q@");
        enc("w://@x", "z", "w://z@x");
        enc("w://a@x", "42", "w://42@x");
        enc("w://a:b@x", "UV", "w://UV@x");
        enc("w://a:b@x/", "NR", "w://NR@x/");
    }
    //--------------------------------------------

    void
    testHost()
    {
        // ipv4
        {
            url u;
            u.set_host(ipv4_address(0x01020304));
            BOOST_TEST(u.str() == "//1.2.3.4");
            BOOST_TEST(u.host_type() ==
                host_type::ipv4);
            BOOST_TEST(u.ipv4_address() ==
                ipv4_address(0x01020304));
            BOOST_TEST(
                u.encoded_host() == "1.2.3.4");
            BOOST_TEST(u.host() == "1.2.3.4");
        }
        {
            url u;
            u.set_host("1.2.3.4");
            BOOST_TEST(u.str() == "//1.2.3.4");
            BOOST_TEST(u.host_type() ==
                host_type::ipv4);
            BOOST_TEST(u.ipv4_address() ==
                ipv4_address(0x01020304));
            BOOST_TEST(
                u.encoded_host() == "1.2.3.4");
            BOOST_TEST(u.host() == "1.2.3.4");
        }
        {
            url u;
            u.set_encoded_host("1.2.3.4");
            BOOST_TEST(u.str() == "//1.2.3.4");
            BOOST_TEST(u.host_type() ==
                host_type::ipv4);
            BOOST_TEST(u.ipv4_address() ==
                ipv4_address(0x01020304));
            BOOST_TEST(
                u.encoded_host() == "1.2.3.4");
            BOOST_TEST(u.host() == "1.2.3.4");
        }
        // ipv6
        {
            url u;
            u.set_host(ipv6_address());
            BOOST_TEST(u.str() == "//[::]");
            BOOST_TEST(u.host_type() ==
                host_type::ipv6);
            BOOST_TEST(u.ipv6_address() ==
                ipv6_address());
        }
        {
            url u;
            u.set_encoded_host("[1:2:3:4::]");
            BOOST_TEST(u.str() == "//[1:2:3:4::]");
            BOOST_TEST(u.host_type() ==
                host_type::ipv6);
            BOOST_TEST(
                u.encoded_host() == "[1:2:3:4::]");
            BOOST_TEST(u.host() == "[1:2:3:4::]");
        }
        // reg-name
        {
            url u;
            u.set_host("example.com");
            BOOST_TEST(u.str() == "//example.com");
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST(u.host() == "example.com");
            BOOST_TEST(u.encoded_host() == "example.com");
        }
    }

    //--------------------------------------------

    void
    testPort()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(
                u.remove_port().str() == s2);
            BOOST_TEST(! u.has_port());
            BOOST_TEST(u.port().empty());
            BOOST_TEST(u.port_number() == 0);
        };

        auto const setn = [](string_view s1,
            std::uint16_t n, string_view s2)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(
                u.set_port(n).str() == s2);
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port_number() == n);
        };

        auto const set = [](string_view s1,
            std::uint16_t n, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(
                u.set_port(s2).str() == s3);
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port_number() == n);
            BOOST_TEST(u.port() == s2);
        };

        BOOST_TEST_THROWS(
            url().set_port("x"),
            std::invalid_argument);

        remove("", "");
        remove("/", "/");
        remove("//", "//");
        remove("//:0", "//");
        remove("//:80", "//");
        remove("//:65535", "//");
        remove("//:999999", "//");
        remove("//:999999/", "///");
        remove("//x:999999/", "//x/");
        remove("//a:b@x.y:8080/path/to/file.txt?#",
               "//a:b@x.y/path/to/file.txt?#");

        remove("x:", "x:");
        remove("x:/", "x:/");
        remove("x://", "x://");
        remove("x://:0", "x://");
        remove("x://:80", "x://");
        remove("x://:65535", "x://");
        remove("x://:999999", "x://");
        remove("x://:999999/", "x:///");
        remove("x://x:999999/", "x://x/");
        remove("x://a:b@x.y:8080/path/to/file.txt?#",
               "x://a:b@x.y/path/to/file.txt?#");

        setn("", 0, "//:0");
        setn("", 443, "//:443");
        setn("", 65535, "//:65535");
        setn("/", 0, "//:0/");
        setn("//", 0, "//:0");
        setn("///", 0, "//:0/");
        setn("//x/", 0, "//x:0/");
        setn("//x/y", 0, "//x:0/y");
        setn("//a:b@/y", 0, "//a:b@:0/y");
        setn("//a:b@c/y", 0, "//a:b@c:0/y");
        setn("//a:b@x.y/path/to/file.txt?#", 8080,
             "//a:b@x.y:8080/path/to/file.txt?#");

        setn("g:", 0, "g://:0");
        setn("g:", 443, "g://:443");
        setn("g:", 65535, "g://:65535");
        setn("g:/", 0, "g://:0/");
        setn("g://", 0, "g://:0");
        setn("g:///", 0, "g://:0/");
        setn("g://x/", 0, "g://x:0/");
        setn("g://x/y", 0, "g://x:0/y");
        setn("g://a:b@/y", 0, "g://a:b@:0/y");
        setn("g://a:b@c/y", 0, "g://a:b@c:0/y");
        setn("g://a:b@x.y/path/to/file.txt?#", 8080,
            "g://a:b@x.y:8080/path/to/file.txt?#");

        set("", 0, "", "//:");
        set("/", 0, "", "//:/");
        set("//", 0, "", "//:");
        set("///", 0, "", "//:/");
        set("//x/", 0, "", "//x:/");
        set("//x/y", 0, "", "//x:/y");
        set("//a:b@/y", 0, "", "//a:b@:/y");
        set("//a:b@c/y", 0, "", "//a:b@c:/y");
        set("//a:b@x.y/path/to/file.txt?#", 0, "",
            "//a:b@x.y:/path/to/file.txt?#");

        set("g:", 0, "", "g://:");
        set("g:/", 0, "", "g://:/");
        set("g://", 0, "", "g://:");
        set("g:///", 0, "", "g://:/");
        set("g://x/", 0, "", "g://x:/");
        set("g://x/y", 0, "", "g://x:/y");
        set("g://a:b@/y", 0, "", "g://a:b@:/y");
        set("g://a:b@c/y", 0, "", "g://a:b@c:/y");
        set("g://a:b@x.y/path/to/file.txt?#", 0, "",
            "g://a:b@x.y:/path/to/file.txt?#");

        set("", 0, "0", "//:0");
        set("", 443, "443", "//:443");
        set("", 65535, "65535", "//:65535");
        set("/", 0, "0", "//:0/");
        set("//", 0, "0", "//:0");
        set("///", 0, "0", "//:0/");
        set("//x/", 0, "0", "//x:0/");
        set("//x/y", 0, "0", "//x:0/y");
        set("//a:b@/y", 0, "0", "//a:b@:0/y");
        set("//a:b@c/y", 0, "0", "//a:b@c:0/y");
        set("//a:b@x.y/path/to/file.txt?#", 8080, "8080",
            "//a:b@x.y:8080/path/to/file.txt?#");

        set("g:", 0, "0", "g://:0");
        set("g:", 443, "443", "g://:443");
        set("g:", 65535, "65535", "g://:65535");
        set("g:/", 0, "0", "g://:0/");
        set("g://", 0, "0", "g://:0");
        set("g:///", 0, "0", "g://:0/");
        set("g://x/", 0, "0", "g://x:0/");
        set("g://x/y", 0, "0", "g://x:0/y");
        set("g://a:b@/y", 0, "0", "g://a:b@:0/y");
        set("g://a:b@c/y", 0, "0", "g://a:b@c:0/y");
        set("g://a:b@x.y/path/to/file.txt?#", 8080, "8080",
            "g://a:b@x.y:8080/path/to/file.txt?#");
    }

    //--------------------------------------------

    void
    testAuthority()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(u.remove_authority().str() == s2);
            BOOST_TEST(u.encoded_authority().empty());
            BOOST_TEST(! u.has_authority());
        };

        auto const set = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u = parse_uri_reference(s1);
            BOOST_TEST(
                u.set_encoded_authority(s2).str() == s3);
            BOOST_TEST(u.encoded_authority() == s2);
            BOOST_TEST(u.has_authority());
        };

        BOOST_TEST_THROWS(
            url().set_encoded_authority("x:y"),
            std::invalid_argument);

        BOOST_TEST_THROWS(
            url().set_encoded_authority("%2"),
            std::invalid_argument);

        remove("", "");
        remove("/", "/");
        remove("/x", "/x");
        remove("/x/", "/x/");
        remove("/x/y", "/x/y");
        remove("x/", "x/");
        remove("x/y", "x/y");
        remove("x/y/", "x/y/");
        remove("x/y/?#", "x/y/?#");

        remove("z:", "z:");
        remove("z:/", "z:/");
        remove("z:/x", "z:/x");
        remove("z:/x/", "z:/x/");
        remove("z:/x/y", "z:/x/y");
        remove("z:x/", "z:x/");
        remove("z:x/y", "z:x/y");
        remove("z:x/y/", "z:x/y/");
        remove("z:x/y/?#", "z:x/y/?#");
        remove("z:x:/y/?#", "z:x:/y/?#");

        remove("//", "");
        remove("///", "/");
        remove("///x", "/x");
        remove("///x/", "/x/");
        remove("///x/y", "/x/y");
        remove("//x/", "/");
        remove("//x/y", "/y");
        remove("//x/y/", "/y/");
        remove("//x/y/?#", "/y/?#");

        remove("z://", "z:");
        remove("z:///", "z:/");
        remove("z:///x", "z:/x");
        remove("z:///x/", "z:/x/");
        remove("z:///x/y", "z:/x/y");
        remove("z://x/", "z:/");
        remove("z://x/y", "z:/y");
        remove("z://x/y/", "z:/y/");
        remove("z://x/y/?#", "z:/y/?#");
        remove("z://x:/y/?#", "z:/y/?#");

        set("", "", "//");
        set("", "x@", "//x@");
        set("", ":x@", "//:x@");
        set("", "x:y@", "//x:y@");
        set("", "x", "//x");
        set("", "x.y", "//x.y");
        set("", "x:", "//x:");
        set("", ":", "//:");
        set("", ":0", "//:0");
        set("", ":443", "//:443");
        set("", ":65536", "//:65536");
        set("", "[::]", "//[::]");
        set("", "[::ffff:127.0.0.1]",
                "//[::ffff:127.0.0.1]");
        set("", "[::ffff:127.0.0.1]:80",
                "//[::ffff:127.0.0.1]:80");
        set("", "user:pass@example.com:80",
                "//user:pass@example.com:80");
        set("ws:",
                "user:pass@example.com:80",
                "ws://user:pass@example.com:80");

        set("///a", "", "///a");
        set("///a", "x@", "//x@/a");
        set("///a", ":x@", "//:x@/a");
        set("///a", "x:y@", "//x:y@/a");
        set("///a", "x", "//x/a");
        set("///a", "x.y", "//x.y/a");
        set("///a", "x:", "//x:/a");
        set("///a", ":", "//:/a");
        set("///a", ":0", "//:0/a");
        set("///a", ":443", "//:443/a");
        set("///a", ":65536", "//:65536/a");
        set("///a", "[::]", "//[::]/a");
        set("///a", "[::ffff:127.0.0.1]",
                    "//[::ffff:127.0.0.1]/a");
        set("///a", "[::ffff:127.0.0.1]:80",
                    "//[::ffff:127.0.0.1]:80/a");
        set("///a", "user:pass@example.com:80",
                    "//user:pass@example.com:80/a");
        set("ws:///a",
                    "user:pass@example.com:80",
                    "ws://user:pass@example.com:80/a");
    }

    //--------------------------------------------

    void
    run()
    {
        testScheme();

        testUser();
        testPassword();
        testUserinfo();
        testHost();
        testPort();
        testAuthority();
#if 0
        testObservers();

        testConstValue();

        testCtor();
        testOrigin();
        testAuthority();
        testPassword();

        testHostAndPort();
        testHost();
        testPort();
        testPath();
        testSegments();
        testQuery();
        testParams();
        testFragment();

        testNormalize();
#endif
    }
};

TEST_SUITE(url_test, "boost.url.url");

} // urls
} // boost
