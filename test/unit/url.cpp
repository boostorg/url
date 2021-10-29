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

#include "test_suite.hpp"
#include <algorithm>
#include <sstream>

namespace boost {
namespace urls {

class url_test
{
public:
    template<class Segments>
    static
    void
    equal(
        Segments const& segs,
        std::initializer_list<
            string_view> init)
    {
        if(! BOOST_TEST(segs.size() ==
            init.size()))
            return;
        BOOST_TEST(std::equal(
            segs.begin(),
            segs.end(),
            init.begin()));
    }

    static
    void
    equal(
        url& u,
        std::initializer_list<
            string_view> init)
    {
        url_view const& uv = u;
        equal(u.segments(), init);
        equal(u.encoded_segments(), init);
        equal(uv.segments(), init);
        equal(uv.encoded_segments(), init);
    }

    //--------------------------------------------

    void
    testSpecial()
    {
        // copy
        {
            url u = parse_uri_reference("x://y/z?q#f").value();
            url u2(u);
            BOOST_TEST(u2.encoded_url() == u.encoded_url());
        }
        {
            url u = parse_uri_reference("x://y/z?q#f").value();
            url u2 = parse_relative_ref("./").value();
            u2 = u;
            BOOST_TEST(u2.encoded_url() == u.encoded_url());
        }

        // move
        {
            url u = parse_uri_reference("x://y/z?q#f").value();
            url u2(std::move(u));
            BOOST_TEST(u.empty());
            BOOST_TEST(u2.encoded_url() == "x://y/z?q#f");
        }
        {
            url u = parse_uri_reference("x://y/z?q#f").value();
            url u2 = parse_relative_ref("./").value();
            u2 = std::move(u);
            BOOST_TEST(u.empty());
            BOOST_TEST(u2.encoded_url() == "x://y/z?q#f");
        }
    }

    //--------------------------------------------

    void
    testCapacity()
    {
        // capacity_in_bytes
        {
            url u;
            BOOST_TEST(u.capacity_in_bytes() == 0);
            BOOST_TEST(u.empty());
        }

        // clear
        {
            url u = parse_uri(
                "http://example.com/index.htm?q#f").value();
            BOOST_TEST(u.capacity_in_bytes() > 0);
            BOOST_TEST(! u.empty());
            u.clear();
            BOOST_TEST(u.capacity_in_bytes() > 0);
            BOOST_TEST(u.empty());
            BOOST_TEST(u.size() == 0);
        }
    }

    //--------------------------------------------

    void
    testScheme()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.remove_scheme().encoded_url() == s2);
            BOOST_TEST(u.scheme().empty());
            BOOST_TEST(u.scheme_id() ==
                scheme::none);
        };

        auto const set = [](
            string_view s1, string_view s2,
            string_view s3, scheme id)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_scheme(s2).encoded_url() == s3);
            BOOST_TEST(u.scheme() == s2);
            BOOST_TEST(u.scheme_id() == id);
        };

        auto const setid = [](
            string_view s1, scheme id,
            string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_scheme(id).encoded_url() == s2);
            BOOST_TEST(u.scheme_id() == id);
        };

        remove("", "");
        remove("x", "x");
        remove("x:", "");
        remove("x:/", "/");
        remove("x:a", "a");
        remove("x:a/", "a/");
        remove("x://", "//");
        remove("x:a:", "./a:");
        remove("x:a:/", "./a:/");
        remove("x://a.b/1/2", "//a.b/1/2");
        remove("x://a:b@c.d/1/?#", "//a:b@c.d/1/?#");

        set("", "ftp", "ftp:", scheme::ftp);
        set("/", "ws", "ws:/", scheme::ws);
        set("a", "ws", "ws:a", scheme::ws);
        set("a/", "ws", "ws:a/", scheme::ws);
        set("//", "ws", "ws://", scheme::ws);
        set("a:/", "ws", "ws:/", scheme::ws);
        set("./a:", "http", "http:a:", scheme::http);
        set("//a.b/1/2", "ws","ws://a.b/1/2", scheme::ws);
        set("//a:b@c.d/1/?#", "ws",
            "ws://a:b@c.d/1/?#", scheme::ws);

        setid("", scheme::ftp, "ftp:");
        setid("/", scheme::ws, "ws:/");
        setid("a", scheme::ws, "ws:a");
        setid("a/", scheme::ws, "ws:a/");
        setid("//", scheme::ws, "ws://");
        setid("a:/", scheme::ws, "ws:/");
        setid("//a.b/1/2", scheme::ws, "ws://a.b/1/2");
        setid("//a:b@c.d/1/?#", scheme::ws,
            "ws://a:b@c.d/1/?#");
        setid("a:/", scheme::none, "/");

        BOOST_TEST_THROWS(
            url().set_scheme(""),
            std::invalid_argument);

        BOOST_TEST_THROWS(
            url().set_scheme(scheme::unknown),
            std::invalid_argument);
    }

    //--------------------------------------------

    void
    testUser()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.remove_user().encoded_url() == s2);
            BOOST_TEST(u.encoded_user().empty());
            BOOST_TEST(u.user().empty());
        };

        auto const set = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_user(s2).encoded_url() == s3);
            BOOST_TEST(u.user() == s2);
            BOOST_TEST(u.has_userinfo());
        };

        auto const enc = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(u.set_encoded_user(
                s2).encoded_url() == s3);
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
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.remove_password().encoded_url() == s2);
            BOOST_TEST(u.encoded_password() == "");
            BOOST_TEST(u.password() == "");
        };

        auto const set = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_password(s2).encoded_url() == s3);
            BOOST_TEST(u.password() == s2);
            BOOST_TEST(u.has_userinfo());
        };

        auto const enc = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(u.set_encoded_password(
                s2).encoded_url() == s3);
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
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(u.remove_userinfo().encoded_url() == s2);
            BOOST_TEST(u.encoded_userinfo().empty());
            BOOST_TEST(u.userinfo().empty());
            BOOST_TEST(! u.has_userinfo());
        };

        auto const set = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_userinfo(s2).encoded_url() == s3);
            BOOST_TEST(u.userinfo() == s2);
            BOOST_TEST(u.has_userinfo());
        };

        auto const enc = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_encoded_userinfo(s2).encoded_url() == s3);
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
        auto const set = [](string_view s1,
            string_view s2, string_view s3,
            host_type ht = host_type::name)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_encoded_host(s2).encoded_url() == s3);
            BOOST_TEST(u.encoded_host() == s2);
            BOOST_TEST(u.host_type() == ht);
        };

        auto const bad = [](string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST_THROWS(u.set_encoded_host(s2),
                std::invalid_argument);
        };

        set("/", "x", "//x/");
        set("//x", "yz", "//yz");
        set("//x/", "yz", "//yz/");
        set("//x/", "1.2.3.4", "//1.2.3.4/", host_type::ipv4);
        set("//x/", "[::]", "//[::]/", host_type::ipv6);
        set("", "1.2.3.4", "//1.2.3.4", host_type::ipv4);
        set("", "[v1.0]", "//[v1.0]", host_type::ipvfuture);

        bad("/", { "\0", 1 });

        // ipv4
        {
            url u;
            u.set_host(ipv4_address(0x01020304));
            BOOST_TEST(u.encoded_url() == "//1.2.3.4");
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
            BOOST_TEST(u.encoded_url() == "//1.2.3.4");
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
            BOOST_TEST(u.encoded_url() == "//1.2.3.4");
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
            BOOST_TEST(u.encoded_url() == "//[::]");
            BOOST_TEST(u.host_type() ==
                host_type::ipv6);
            BOOST_TEST(u.ipv6_address() ==
                ipv6_address());
        }
        {
            url u;
            u.set_encoded_host("[1:2:3:4::]");
            BOOST_TEST(u.encoded_url() == "//[1:2:3:4::]");
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
            BOOST_TEST(u.encoded_url() == "//example.com");
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
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.remove_port().encoded_url() == s2);
            BOOST_TEST(! u.has_port());
            BOOST_TEST(u.port().empty());
            BOOST_TEST(u.port_number() == 0);
        };

        auto const setn = [](string_view s1,
            std::uint16_t n, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_port(n).encoded_url() == s2);
            BOOST_TEST(u.has_port());
            BOOST_TEST(u.port_number() == n);
        };

        auto const set = [](string_view s1,
            std::uint16_t n, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_port(s2).encoded_url() == s3);
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
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(u.remove_authority().encoded_url() == s2);
            BOOST_TEST(u.encoded_authority().empty());
            BOOST_TEST(! u.has_authority());
        };

        auto const set = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_encoded_authority(s2).encoded_url() == s3);
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
        remove("z://x//y/?q#f", "z:/.//y/?q#f");

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
        set("", "1.2.3.4", "//1.2.3.4");
        set("", "[v1.0]", "//[v1.0]");
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
        set("///a", "1.2.3.4", "//1.2.3.4/a");
        set("///a", "[v1.0]", "//[v1.0]/a");
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
    testOrigin()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(u.remove_origin().encoded_url() == s2);
            BOOST_TEST(u.encoded_origin().empty());
            BOOST_TEST(! u.has_authority());
        };

        remove("", "");
        remove("w", "w");
        remove("w/", "w/");
        remove("/", "/");
        remove("/x", "/x");
        remove("/x/", "/x/");
        remove("/x/?#", "/x/?#");
        remove("w:", "");
        remove("w::", "./:");
        remove("x://y//z", ".//z");
        remove("http://user:pass@example.com:80/path/to/file.txt",
               "/path/to/file.txt"); 
    }

    //--------------------------------------------

    void
    testPath()
    {
        // set_path_absolute
        {
            url u;
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.encoded_url() == "");
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(u.encoded_url() == "/");
        }
        {
            url u = parse_relative_ref("/").value();
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(u.encoded_url() == "/");
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.encoded_url() == "");
        }
        {
            url u = parse_relative_ref("//").value();
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(u.encoded_url() == "///");
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.encoded_url() == "//");
        }
        {
            url u = parse_relative_ref("//x/y").value();
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(! u.set_path_absolute(false));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(u.encoded_url() == "//x/y");
        }
        {
            url u = parse_uri("x:y").value();
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(u.encoded_url() == "x:/y");
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.encoded_url() == "x:y");
        }

        // set_encoded_path
        {
            // empty
            url u = parse_uri("x://y/path/to/file.txt?q#f").value();
            u.set_encoded_path("");
            BOOST_TEST(u.encoded_path() == "/");
            BOOST_TEST(u.encoded_url() == "x://y/?q#f");
        }
        {
            // path-abempty
            url u = parse_uri("x://y/path/to/file.txt?q#f").value();
            u.set_encoded_path("/x");
            BOOST_TEST(u.encoded_path() == "/x");
            BOOST_TEST(u.encoded_url() == "x://y/x?q#f");
            u.set_encoded_path("x/");
            BOOST_TEST(u.encoded_url() == "x://y/x/?q#f");
        }
        {
            // path-absolute
            url u = parse_relative_ref("/path/to/file.txt").value();
            u.set_encoded_path("/home/file.txt");
            BOOST_TEST(u.encoded_path() == "/home/file.txt");
            BOOST_TEST(u.encoded_url() == "/home/file.txt");
            u.set_encoded_path("//home/file.txt");
            equal(u, { "", "home", "file.txt" });
            BOOST_TEST(u.encoded_path() == "/.//home/file.txt");
            BOOST_TEST_THROWS(u.set_encoded_path("/home/%ile.txt"),
                std::invalid_argument);
        }
        {
            // path-rootless
            url u = parse_uri("x:mailto").value();
            u.set_encoded_path("file.txt");
            BOOST_TEST(u.encoded_path() == "file.txt");
            BOOST_TEST(u.encoded_url() == "x:file.txt");
            u.set_encoded_path(":file.txt");
            BOOST_TEST(u.encoded_path() == ":file.txt");
            BOOST_TEST(u.encoded_url() == "x::file.txt");
            // to path-absolute
            u.set_encoded_path("/file.txt");
            BOOST_TEST(u.encoded_path() == "/file.txt");
            BOOST_TEST(u.encoded_url() == "x:/file.txt");
        }
        {
            // path-noscheme
            url u = parse_relative_ref("mailto").value();
            u.set_encoded_path("file.txt");
            BOOST_TEST(u.encoded_path() == "file.txt");
            BOOST_TEST(u.encoded_url() == "file.txt");
            u.set_encoded_path(":file.txt");
            BOOST_TEST(u.encoded_path() == "./:file.txt");
            u.set_encoded_path("http:index.htm");
            BOOST_TEST(u.encoded_path() == "./http:index.htm");
        }

        // set_encoded_path
        {
            auto const check =
            [&](string_view s0,
                string_view arg,
                string_view match)
            {
                url u = parse_uri_reference(s0).value();
                u.set_encoded_path(arg);
                BOOST_TEST(
                    u.encoded_url() == match);
            };
            check(
                "",
                "path/to/file.txt",
                "path/to/file.txt");
            check(
                "",
                "/path/to/file.txt",
                "/path/to/file.txt");
            check(
                "",
                "//index.htm",
                "/.//index.htm");
            check(
                "http://example.com?q#f",
                "path/to/file.txt",
                "http://example.com/path/to/file.txt?q#f");
            check(
                "http://example.com?q#f",
                "/path/to/file.txt",
                "http://example.com/path/to/file.txt?q#f");
            check(
                "x",
                "http:path/to/file.",
                "./http:path/to/file.");
            check(
                "x:",
                "y:z/",
                "x:y:z/");
        }

        // set_path
        {
            auto const check =
            [&](string_view s0,
                string_view arg,
                string_view match)
            {
                url u = parse_uri_reference(s0).value();
                u.set_path(arg);
                BOOST_TEST(
                    u.encoded_url() == match);
            };
            check(
                "",
                "path/to/file.txt",
                "path/to/file.txt");
            check(
                "",
                "/path/to/file.txt",
                "/path/to/file.txt");
            check(
                "",
                "//index.htm",
                "/.//index.htm");
            check(
                "http://example.com?q#f",
                "path/to/file.txt",
                "http://example.com/path/to/file.txt?q#f");
            check(
                "http://example.com?q#f",
                "/path/to/file.txt",
                "http://example.com/path/to/file.txt?q#f");
            check(
                "x",
                "http:path/to/file.",
                "./http:path/to/file.");
            check(
                "x:",
                "y:z/",
                "x:y:z/");
            check(
                "x:y:z/",
                "",
                "x:");
            check(
                "x:y:z/",
                "abc",
                "x:abc");
        }
    }

    //--------------------------------------------

    void
    testQuery()
    {
        // has_query
        {
            url u = parse_relative_ref("?query").value();
            BOOST_TEST(u.has_query());
            u.clear();
            BOOST_TEST(! u.has_query());
            u = parse_relative_ref("?").value();
            BOOST_TEST(u.has_query());
        }

        // remove_query
        {
            url u = parse_relative_ref("?query").value();
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() == "query");
            BOOST_TEST(u.params().size() == 1);
            BOOST_TEST(u.remove_query().has_query() == false);
            BOOST_TEST(u.encoded_query() == "");
            BOOST_TEST(u.query() == "");
            BOOST_TEST(u.params().size() == 0);
            BOOST_TEST(u.encoded_params().size() == 0);
        }

        // set_encoded_query
        {
            url u;
            BOOST_TEST(! u.has_query());
            u.set_encoded_query("k1=v1&k2=v2");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.params().size() == 2);
            BOOST_TEST(u.params()[0].key == "k1");
            BOOST_TEST(u.params()[0].value == "v1");
            BOOST_TEST(u.params()[1].key == "k2");
            BOOST_TEST(u.params()[1].value == "v2");

            u.set_encoded_query("");
            BOOST_TEST(! u.has_query());
            BOOST_TEST(u.encoded_query().empty());
            BOOST_TEST(u.params().size() == 0);

            BOOST_TEST_THROWS(
                u.set_encoded_query("\x01"),
                std::invalid_argument);
        }

        // set_query
        {
            url u;
            BOOST_TEST(! u.has_query());
            u.set_query("!@#$%^&*()_+=-;:'{}[]|\\?/>.<,");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() ==
                "!@%23$%25%5e&*()_+=-;:'%7b%7d%5b%5d%7c%5c?/%3e.%3c,");
            BOOST_TEST(u.params().size() == 2);
            BOOST_TEST(u.params()[0].key == "!@#$%^");
            BOOST_TEST(u.params()[0].value == "");
            BOOST_TEST(u.params()[1].key == "*()_ ");
            BOOST_TEST(u.params()[1].value ==
                "-;:'{}[]|\\?/>.<,");
        }
    }

    void
    testFragment()
    {
        // has_fragment
        {
            url u = parse_relative_ref("#frag").value();
            BOOST_TEST(u.has_fragment());
            u.clear();
            BOOST_TEST(! u.has_fragment());
            u = parse_relative_ref("#").value();
            BOOST_TEST(u.has_fragment());
        }

        // remove_fragment
        {
            url u = parse_relative_ref( "/#frag" ).value();
            BOOST_TEST(u.has_fragment());
            BOOST_TEST(! u.remove_fragment().has_fragment());
            BOOST_TEST(u.fragment() == "");
        }

        // set_encoded_fragment
        {
            url u = parse_relative_ref( "/" ).value();
            BOOST_TEST(! u.has_fragment());
            u.set_encoded_fragment("fr%20ag");
            BOOST_TEST(u.fragment() == "fr ag");
            u.remove_fragment();
            u.set_encoded_fragment("");
            BOOST_TEST(u.has_fragment());
            BOOST_TEST(u.encoded_url() == "/#");
        }

        // set_fragment
        {
            url u = parse_relative_ref( "/" ).value();
            BOOST_TEST(! u.has_fragment());
            u.set_fragment("fr ag");
            BOOST_TEST(u.fragment() == "fr ag");
            BOOST_TEST(u.encoded_fragment() == "fr%20ag");
        }
    }

    //--------------------------------------------

    template<class F>
    static
    void
    perform(
        string_view s0,
        string_view s1,
        std::initializer_list<
            string_view> init,
        F const& f)
    {
        url u = parse_uri_reference(s0).value();
        f(u);
        equal(u.segments(), init);
        equal(u.encoded_segments(), init);
        BOOST_TEST(u.encoded_url() == s1);
    }

    void
    testSegments()
    {
        auto const check = [](
            string_view s,
            std::initializer_list<
                string_view> init,
            bool abs)
        {
            url u =
                parse_uri_reference(
                    s).value();
            url_view const& uv = u;
            BOOST_TEST(
                u.is_path_absolute() == abs);
            BOOST_TEST(
                uv.is_path_absolute() == abs);
            equal(uv.segments(), init);
            equal(uv.encoded_segments(), init);
            equal(u.segments(), init);
            equal(u.encoded_segments(), init);
        };

        auto const abs = [&check](
            string_view s,
            std::initializer_list<
                string_view> init)
        {
            check(s, init, true);
        };

        auto const rel = [&check](
            string_view s,
            std::initializer_list<
                string_view> init)
        {
            check(s, init, false);
        };

        auto const assign = [](
            string_view s0,
            string_view s1,
            std::initializer_list<
                string_view> init)
        {
            url u0 = parse_uri_reference(s0).value();
            {
                url u(u0);
                u.segments() = init;
                equal(u.segments(), init);
                equal(u.encoded_segments(), init);
                BOOST_TEST(u.encoded_url() == s1);
            }
            {
                url u(u0);
                u.encoded_segments() = init;
                equal(u.segments(), init);
                equal(u.encoded_segments(), init);
                BOOST_TEST(u.encoded_url() == s1);
            }
        };

        rel("", {});
        rel("./", { "" });
        rel("././", { ".", "" });
        rel("index.htm", { "index.htm" });
        rel("path/to/file.txt", { "path", "to", "file.txt" });
        rel("//example.com", {} );
        rel("x:y:z", { "y:z" });
        rel("x:y:z/", { "y:z", "" });
        rel("./y:z", { "y:z" });
        rel("./y:z/", { "y:z", "" });

        abs("/", {});
        abs("/./", { "" });
        abs("/././", { ".", "" });
        abs("//example.com/", {} );
        abs("//example.com/./", { "" } );
        abs("/index.htm", { "index.htm" });
        abs("/home/", { "home", "" });
        abs("//x//", { "", "" });
        abs("/.//", { "", "" });
        abs("//x/y", { "y" });
        abs("/././/", { ".", "", "" });
        abs("/.//", { "", "" });
        abs("x:/.//", { "", "" });

        assign( "", "./", { "" });
        assign( "/", "/./", { "" });
        assign( "//x", "//x/./", { "" });
        assign( "//x/", "//x/./", { "" });
        assign( "", "x", { "x" });
        assign( "/", "/x", { "x" });
        assign( "", "x/y/z", { "x", "y", "z" });
        assign( "/", "/x/y/z", { "x", "y", "z" });
        assign( "/", "/.", { "." });
        assign( "/", "/././", { ".", "" });
        assign( "/", "/././/", { ".", "", "" });
        assign( "//x/", "//x/.", { "." });
        assign( "//x/", "//x/././", { ".", "" });
        assign( "//x/", "//x/././/", { ".", "", "" });

        perform( "/", "/", {}, [](url& u) { u.segments().clear(); });
        perform( "/", "/", {}, [](url& u) { u.encoded_segments().clear(); });
        perform( "//x/", "//x/", {}, [](url& u) { u.segments().clear(); });
        perform( "//x/", "//x/", {}, [](url& u) { u.encoded_segments().clear(); });
        perform( "/x", "/x/y", { "x", "y" }, [](url& u) { u.segments().push_back("y"); });
        perform( "/x", "/x/y", { "x", "y" }, [](url& u) { u.encoded_segments().push_back("y"); });
        perform( "/x/", "/x//y", { "x", "", "y" }, [](url& u) { u.segments().push_back("y"); });
        perform( "/x/", "/x//y", { "x", "", "y" }, [](url& u) { u.encoded_segments().push_back("y"); });
        perform( "//x//", "/.//", { "", "" }, [](url& u) { u.remove_authority(); });
        perform( "x:y:z", "./y:z", { "y:z" }, [](url& u) { u.remove_scheme(); });
        perform( "x:y:z/", "./y:z/", { "y:z", "" }, [](url& u) { u.remove_scheme(); });
        perform( "./y:z", "x:y:z", { "y:z" }, [](url& u) { u.set_scheme("x"); });
        perform( "./y:z/", "x:y:z/", { "y:z", "" }, [](url& u) { u.set_scheme("x"); });
        perform( "y", "//x/y", { "y" }, [](url& u) { u.set_encoded_authority("x"); });
        perform( "//x/y", "/y", { "y" }, [](url& u) { u.remove_authority(); });
        perform( "y", "//x:1/y", { "y" }, [](url& u) { u.set_encoded_authority("x:1"); });
        perform( "/y", "//x:1/y", { "y" }, [](url& u) { u.set_encoded_authority("x:1"); });
        perform( "x:", "x:y", { "y" }, [](url& u) { u.segments().push_back("y"); });
        perform( "x:", "x:y", { "y" }, [](url& u) { u.encoded_segments().push_back("y"); });
        perform( "/.//", "x:/.//", { "", "" }, [](url& u) { u.set_scheme("x"); });

        perform( "//x/y/z", "//x/z", { "z" }, [](url& u) {
            u.segments().erase(u.segments().begin());
            });

        perform( "//x", "//x/", {}, [](url& u) {
            BOOST_TEST(u.set_path_absolute(true));
            });
        
        perform( "//x/", "//x", {}, [](url& u) {
            BOOST_TEST(u.set_path_absolute(false));
            });
        
        perform( "//x/y", "//x/y", { "y" }, [](url& u) {
            BOOST_TEST(! u.set_path_absolute(false));
            });

        perform( "//x/y", "//x/y", { "y" }, [](url& u) {
            BOOST_TEST(u.set_path_absolute(true));
            });

        perform( "x:", "x:/y", { "y" }, [](url& u) {
            BOOST_TEST(u.set_path_absolute(true));
            u.encoded_segments().push_back("y");
            });
    }

    //--------------------------------------------

    void
    testResolution()
    {
        auto ub = parse_uri(
            "http://a/b/c/d;p?q").value();

        auto const check = [&ub](
            string_view r,
            string_view m)
        {
            auto ur =
                parse_uri_reference(r).value();
            url u = parse_uri(
                "z://y:x@p.q:69/x/f?q#f" ).value();
            error_code ec;
            resolve(ub, ur, u, ec);
            if(! BOOST_TEST(! ec.failed()))
                return;
            BOOST_TEST(u.encoded_url() == m);
        };

        check("g:h"          , "g:h");
        check("g"            , "http://a/b/c/g");
        check("./g"          , "http://a/b/c/g");
        check("g/"           , "http://a/b/c/g/");
        check("/g"           , "http://a/g");
        check("//g"          , "http://g");
        check("?y"           , "http://a/b/c/d;p?y");
        check("g?y"          , "http://a/b/c/g?y");
        check("#s"           , "http://a/b/c/d;p?q#s");
        check("g#s"          , "http://a/b/c/g#s");
        check("g?y#s"        , "http://a/b/c/g?y#s");
        check(";x"           , "http://a/b/c/;x");
        check("g;x"          , "http://a/b/c/g;x");
        check("g;x?y#s"      , "http://a/b/c/g;x?y#s");
        check(""             , "http://a/b/c/d;p?q");
        check("."            , "http://a/b/c/");
        check("./"           , "http://a/b/c/");
        check(".."           , "http://a/b/");
        check("../"          , "http://a/b/");
        check("../g"         , "http://a/b/g");
        check("../.."        , "http://a/");
        check("../../"       , "http://a/");
        check("../../g"      , "http://a/g");

        /*  Errata 4547
            https://www.rfc-editor.org/errata/eid4547
        */
        //check("../../../g",    "http://a/g");
        //check("../../../../g", "http://a/g");
        check("../../../g",    "http://a/../g");
        check("../../../../g", "http://a/../../g");

        check("/./g"         , "http://a/g");

        // VFALCO RFC says this:
        //check("/../g"        , "http://a/g");
        // but this seems more logical
        check("/../g"        , "http://a/../g");

        check("g."           , "http://a/b/c/g.");
        check(".g"           , "http://a/b/c/.g");
        check("g.."          , "http://a/b/c/g..");
        check("..g"          , "http://a/b/c/..g");
                             
        check("./../g"       , "http://a/b/g");
        check("./g/."        , "http://a/b/c/g/");
        check("g/./h"        , "http://a/b/c/g/h");
        check("g/../h"       , "http://a/b/c/h");
        check("g;x=1/./y"    , "http://a/b/c/g;x=1/y");
        check("g;x=1/../y"   , "http://a/b/c/y");
                             
        check("g?y/./x"      , "http://a/b/c/g?y/./x");
        check("g?y/../x"     , "http://a/b/c/g?y/../x");
        check("g#s/./x"      , "http://a/b/c/g#s/./x");
        check("g#s/../x"     , "http://a/b/c/g#s/../x");
    }

    //--------------------------------------------

    void
    testOstream()
    {
        {
            url u = parse_uri(
                "http://example.com/index.htm?q#f").value();
            std::stringstream ss;
            ss << u;
            BOOST_TEST(ss.str() ==
                "http://example.com/index.htm?q#f");
        }
        {
            std::stringstream ss;
            ss <<
                std::setfill('*') <<
                std::left <<
                std::setw(11) <<
                parse_uri("http://x").value();
            BOOST_TEST(ss.str() == "http://x***");
        }
    }

    //--------------------------------------------

    void
    run()
    {
        testSpecial();
        testCapacity();
        testScheme();
        testUser();
        testPassword();
        testUserinfo();
        testHost();
        testPort();
        testAuthority();
        testOrigin();
        testPath();
        testQuery();
        testFragment();
        testSegments();
        testResolution();
        testOstream();
    }
};

TEST_SUITE(url_test, "boost.url.url");

} // urls
} // boost

