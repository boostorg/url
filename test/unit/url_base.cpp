//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/url_base.hpp>

#include <boost/url/url.hpp>
#include "test_suite.hpp"

/*  Legend

    '#' 0x23    '=' 0x3d
    '%' 0x25    '@' 0x40
    '&' 0x26    '[' 0x5b
    '.' 0x2e    ']' 0x5d
    ':' 0x3a
*/

namespace boost {
namespace urls {

struct url_base_test
{
    template<class F>
    static
    void
    modify(
        string_view before,
        string_view after,
        F&& f)
    {
        url u(before);
        f(u);
        auto s = u.string();
        BOOST_TEST_EQ(s, after);
    }

    //--------------------------------------------
    //
    // Scheme
    //
    //--------------------------------------------

    void
    testSetScheme()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST(u.remove_scheme().string() == s2);
            BOOST_TEST(u.scheme_id() == scheme::none);
            BOOST_TEST(u.scheme().empty());
        };

        auto const set = [](
            scheme id, string_view s1,
            string_view s2, string_view s3)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s2));
            BOOST_TEST(u.set_scheme(s1).string() == s3);
            BOOST_TEST_EQ(u.scheme(), s1);
            BOOST_TEST_EQ(u.scheme_id(), id);
        };

        auto const setid = [](
            scheme id, string_view s1, string_view s2)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST(u.set_scheme(id).string() == s2);
            BOOST_TEST_EQ(u.scheme_id(), id);
        };

        remove("",      "");
        remove("x",     "x");
        remove("x:",    "");
        remove("x:/",   "/");
        remove("x:a",   "a");
        remove("x:a/",  "a/");
        remove("x://",  "//");
        remove("x:a:",  "./a:");
        remove("x:a:/", "./a:/");

        remove("x://a.b/1/2",      "//a.b/1/2");
        remove("x://a:b@c.d/1/?#", "//a:b@c.d/1/?#");

        set(scheme::ftp,  "ftp",  "",     "ftp:");
        set(scheme::ws,   "ws",   "/",    "ws:/");
        set(scheme::ws,   "ws",   "a",    "ws:a");
        set(scheme::ws,   "ws",   "a/",   "ws:a/");
        set(scheme::ws,   "ws",   "//",   "ws://");
        set(scheme::ws,   "ws",   "a:/",  "ws:/");
        set(scheme::http, "http", "./a:", "http:a:");

        set(scheme::ws, "ws", "//a.b/1/2",      "ws://a.b/1/2");
        set(scheme::ws, "ws", "//a:b@c.d/1/?#", "ws://a:b@c.d/1/?#");

        setid(scheme::ftp, "",    "ftp:");
        setid(scheme::ws,  "/",   "ws:/");
        setid(scheme::ws,  "a",   "ws:a");
        setid(scheme::ws,  "a/",  "ws:a/");
        setid(scheme::ws,  "//",  "ws://");
        setid(scheme::ws,  "a:/", "ws:/");

        setid(scheme::ws,
            "//a.b/1/2", "ws://a.b/1/2");

        setid(scheme::ws,
            "//a:b@c.d/1/?#",  "ws://a:b@c.d/1/?#");

        setid(scheme::none, "a:/", "/");

        BOOST_TEST_THROWS(
            url().set_scheme(""),
            std::exception);

        BOOST_TEST_THROWS(
            url().set_scheme(scheme::unknown),
            std::invalid_argument);

        // self-intersection
        modify(
            "x://?mailto",
            "mailto://?mailto",
            [](url_base& u)
            {
                u.set_scheme(
                    u.encoded_query());
            });
    }

    //--------------------------------------------
    //
    // Userinfo
    //
    //--------------------------------------------

    void
    testSetUserinfo()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST_EQ(u.remove_userinfo().string(), s2);
            BOOST_TEST(u.encoded_userinfo().empty());
            BOOST_TEST(u.userinfo().empty());
            BOOST_TEST(! u.has_userinfo());
        };

        auto const set = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST_EQ(u.set_userinfo(s2).string(), s3);
        };

        auto const enc = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST(u.set_encoded_userinfo(s2).string() == s3);
            BOOST_TEST_EQ(u.encoded_userinfo(), s2);
            BOOST_TEST(u.has_userinfo());
        };

        BOOST_TEST_THROWS(url().set_encoded_userinfo("%2"), std::exception);

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

        set("", ":", "//:@");
        set("/", "a", "//a@/");
        set("//", "@", "//%40@");
        set("//@", "xyz", "//xyz@");
        set("//a@", ":@", "//:%40@");
        set("//a:@", "x", "//x@");
        set("//a:b@", "p:q", "//p:q@");
        set("//@x", "z", "//z@x");
        set("//a@x", "42", "//42@x");
        set("//a:b@x", "UV", "//UV@x");
        set("//a:b@x/", "NR", "//NR@x/");

        set("w:", ":", "w://:@");
        set("w:/", "a", "w://a@/");
        set("w://", "@", "w://%40@");
        set("w://@", "xyz", "w://xyz@");
        set("w://a@", ":@", "w://:%40@");
        set("w://a:@", "x", "w://x@");
        set("w://a:b@", "p:q", "w://p:q@");
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

        // self-intersection
        modify(
            "x://?user:pass",
            "x://user:pass@?user:pass",
            [](url_base& u)
            {
                u.set_encoded_userinfo(u.encoded_query());
            });
        modify(
            "x://?user:pass",
            "x://user:pass@?user:pass",
            [](url_base& u)
            {
                u.set_userinfo(
                    u.encoded_query());
            });
        modify(
            "x://?user:pass",
            "x://user:pass@?user:pass",
            [](url_base& u)
            {
                u.set_userinfo(
                    u.query());
            });
    }

    void
    testSetUser()
    {
        auto const set = [](
            string_view s0,
            string_view s,
            string_view s1)
        {
            modify(s0, s1,
                [s](url_base& u)
                {
                    u.set_user(s);
                    BOOST_TEST(u.user() == s);
                    BOOST_TEST(u.has_userinfo());
                });
        };

        auto const enc = [](
            string_view s0,
            string_view s,
            string_view s1)
        {
            modify(s0, s1,
                [s](url_base& u)
                {
                    BOOST_TEST_NO_THROW(u.set_encoded_user(s));
                    BOOST_TEST_EQ(
                        decode_view(s),
                        decode_view(u.encoded_user()));
                    BOOST_TEST(u.has_userinfo());
                });
        };

        BOOST_TEST_THROWS(
            url().set_encoded_user("%2"),
            system_error);

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
        set("ws://a:b@c", ":", "ws://%3A:b@c");
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
        enc("x:", "user%3apass", "x://user%3apass@");
        enc("x:", "user@local", "x://user%40local@");

        // self-intersection
        modify(
            "x://u@/?johndoe",
            "x://johndoe@/?johndoe",
            [](url_base& u)
            {
                u.set_encoded_user(
                    u.encoded_query());
            });
        modify(
            "x://u@/?johndoe",
            "x://johndoe@/?johndoe",
            [](url_base& u)
            {
                u.set_user(
                    u.query());
            });
        modify(
            "x://u@/?johndoe",
            "x://johndoe@/?johndoe",
            [](url_base& u)
            {
                u.set_user(
                    u.encoded_query());
            });
    }

    void
    testSetPassword()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST(u.remove_password().string() == s2);
            BOOST_TEST_EQ(u.encoded_password(), "");
            BOOST_TEST_EQ(u.password(), "");
        };

        auto const set = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST(u.set_password(s2).string() == s3);
        };

        auto const enc = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST(u.set_encoded_password(
                s2).string() == s3);
            BOOST_TEST_EQ(u.encoded_password(), s2);
            BOOST_TEST(u.has_userinfo());
        };

        BOOST_TEST_THROWS(url().set_encoded_password(
            "%2"), system_error);

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

        // self-intersection
        modify(
            "x://:p@/?johndoe",
            "x://:johndoe@/?johndoe",
            [](url_base& u)
            {
                u.set_encoded_password(
                    u.encoded_query());
            });
        modify(
            "x://:p@/?johndoe",
            "x://:johndoe@/?johndoe",
            [](url_base& u)
            {
                u.set_password(
                    u.query());
            });
        modify(
            "x://:p@/?johndoe",
            "x://:johndoe@/?johndoe",
            [](url_base& u)
            {
                u.set_password(
                    u.encoded_query());
            });
    }

    //--------------------------------------------
    //
    // Host
    //
    //--------------------------------------------

    static
    std::string
    bracketed(
        std::string s)
    {
        return
            std::string("[") + s +
            std::string("]");
    }

    void
    testSetHost()
    {
        auto const set_host = [](
            string_view s,
            string_view s1,
            host_type ht)
        {
            url u;
            BOOST_TEST_NO_THROW(u.set_host(s));
            BOOST_TEST_EQ(u.host_type(), ht);
            BOOST_TEST_EQ(u.string(), s1);
            BOOST_TEST_EQ(u.host(), u.encoded_host().decode_to_string());
            switch(ht)
            {
            case host_type::none:
                BOOST_TEST_FAIL();
                break;
            case host_type::ipv4:
                BOOST_TEST_EQ(u.encoded_host(), s);
                BOOST_TEST_EQ(u.host_address(), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address(s));
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::ipv6:
                BOOST_TEST_EQ(u.encoded_host(), s);
                BOOST_TEST_EQ(bracketed(u.host_address()), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address(u.host_address()));
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::ipvfuture:
                BOOST_TEST_EQ(u.encoded_host(), s);
                BOOST_TEST_EQ(bracketed(u.host_address()), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), u.host_address());
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::name:
                BOOST_TEST_EQ(u.host_address(), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), s);
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            }
        };

        auto const set_encoded_host = [](
            string_view s,
            string_view s1,
            host_type ht)
        {
            url u;
            BOOST_TEST_NO_THROW(u.set_encoded_host(s));
            BOOST_TEST_EQ(u.host_type(), ht);
            BOOST_TEST_EQ(u.string(), s1);
            BOOST_TEST_EQ(u.host(), u.encoded_host().decode_to_string());
            switch(ht)
            {
            case host_type::none:
                BOOST_TEST_FAIL();
                break;
            case host_type::ipv4:
                BOOST_TEST_EQ(u.encoded_host(), s);
                BOOST_TEST_EQ(u.host_address(), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address(s));
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::ipv6:
                BOOST_TEST_EQ(u.encoded_host(), s);
                BOOST_TEST_EQ(bracketed(u.host_address()), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address(u.host_address()));
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::ipvfuture:
                BOOST_TEST_EQ(u.encoded_host(), s);
                BOOST_TEST_EQ(bracketed(u.host_address()), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), u.host_address());
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::name:
                BOOST_TEST_EQ(u.host_address(), pct_string_view(s).decode_to_string());
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), pct_string_view(s).decode_to_string());
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            }
        };

        auto const set_host_address = [](
            string_view s,
            string_view s1,
            host_type ht)
        {
            url u;
            BOOST_TEST_NO_THROW(u.set_host_address(s));
            BOOST_TEST_EQ(u.host_type(), ht);
            BOOST_TEST_EQ(u.string(), s1);
            BOOST_TEST_EQ(u.host(), u.encoded_host().decode_to_string());
            switch(ht)
            {
            case host_type::none:
                BOOST_TEST_FAIL();
                break;
            case host_type::ipv4:
                BOOST_TEST_EQ(u.encoded_host(), s);
                BOOST_TEST_EQ(u.host_address(), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address(s));
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::ipv6:
                BOOST_TEST_EQ(u.encoded_host(), bracketed(s));
                BOOST_TEST_EQ(u.host_address(), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address(s));
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::ipvfuture:
                BOOST_TEST_EQ(u.encoded_host(), bracketed(s));
                BOOST_TEST_EQ(u.host_address(), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), s);
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::name:
                BOOST_TEST_EQ(u.host_address(), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), s);
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            }
        };

        auto const set_encoded_host_address = [](
            string_view s,
            string_view s1,
            host_type ht)
        {
            url u;
            BOOST_TEST_NO_THROW(u.set_encoded_host_address(s));
            BOOST_TEST_EQ(u.host_type(), ht);
            BOOST_TEST_EQ(u.string(), s1);
            BOOST_TEST_EQ(u.host(), u.encoded_host().decode_to_string());
            switch(ht)
            {
            case host_type::none:
                BOOST_TEST_FAIL();
                break;
            case host_type::ipv4:
                BOOST_TEST_EQ(u.encoded_host(), s);
                BOOST_TEST_EQ(u.host_address(), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address(s));
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::ipv6:
                BOOST_TEST_EQ(u.encoded_host(), bracketed(s));
                BOOST_TEST_EQ(u.host_address(), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address(s));
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::ipvfuture:
                BOOST_TEST_EQ(u.encoded_host(), bracketed(s));
                BOOST_TEST_EQ(u.host_address(), s);
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), s);
                BOOST_TEST_EQ(u.host_name(), "");
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            case host_type::name:
                BOOST_TEST_EQ(u.host_address(), pct_string_view(s).decode_to_string());
                BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
                BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
                BOOST_TEST_EQ(u.host_ipvfuture(), "");
                BOOST_TEST_EQ(u.host_name(), pct_string_view(s).decode_to_string());
                BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
                break;
            }
        };

        auto const set_host_ipv4_address = [](
            string_view s,
            string_view s1)
        {
            url u;
            BOOST_TEST_NO_THROW(u.set_host_ipv4_address(ipv4_address(s)));
            BOOST_TEST_EQ(u.host_type(), host_type::ipv4);
            BOOST_TEST_EQ(u.string(), s1);
            BOOST_TEST_EQ(u.host(), u.encoded_host().decode_to_string());
            BOOST_TEST_EQ(u.encoded_host(), s);
            BOOST_TEST_EQ(u.host_address(), s);
            BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address(s));
            BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
            BOOST_TEST_EQ(u.host_ipvfuture(), "");
            BOOST_TEST_EQ(u.host_name(), "");
            BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
        };

        auto const set_host_ipv6_address = [](
            string_view s,
            string_view s1)
        {
            url u;
            BOOST_TEST_NO_THROW(u.set_host_ipv6_address(ipv6_address(s)));
            BOOST_TEST_EQ(u.host_type(), host_type::ipv6);
            BOOST_TEST_EQ(u.string(), s1);
            BOOST_TEST_EQ(u.host(), u.encoded_host().decode_to_string());
            BOOST_TEST_EQ(u.encoded_host(), bracketed(s));
            BOOST_TEST_EQ(u.host_address(), s);
            BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
            BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address(s));
            BOOST_TEST_EQ(u.host_ipvfuture(), "");
            BOOST_TEST_EQ(u.host_name(), "");
            BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
        };

        auto const set_host_ipvfuture = [](
            string_view s,
            string_view s1)
        {
            url u;
            BOOST_TEST_NO_THROW(u.set_host_ipvfuture(s))
            BOOST_TEST_EQ(u.host_type(), host_type::ipvfuture);
            BOOST_TEST_EQ(u.string(), s1);
            BOOST_TEST_EQ(u.host(), u.encoded_host().decode_to_string());
            BOOST_TEST_EQ(u.encoded_host(), bracketed(s));
            BOOST_TEST_EQ(u.host_address(), s);
            BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
            BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
            BOOST_TEST_EQ(u.host_ipvfuture(), s);
            BOOST_TEST_EQ(u.host_name(), "");
            BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
        };

        auto const set_host_name = [](
            string_view s,
            string_view s1)
        {
            url u;
            BOOST_TEST_NO_THROW(u.set_host_name(s))
            BOOST_TEST_EQ(u.host_type(), host_type::name);
            BOOST_TEST_EQ(u.string(), s1);
            BOOST_TEST_EQ(u.host(), s);
            BOOST_TEST_EQ(u.host(), u.encoded_host().decode_to_string());
            BOOST_TEST_EQ(u.host_address(), s);
            BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
            BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
            BOOST_TEST_EQ(u.host_ipvfuture(), "");
            BOOST_TEST_EQ(u.host_name(), s);
            BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
        };

        auto const set_encoded_host_name = [](
            string_view s,
            string_view s1)
        {
            url u;
            BOOST_TEST_NO_THROW(u.set_encoded_host_name(s))
            BOOST_TEST_EQ(u.host_type(), host_type::name);
            BOOST_TEST_EQ(u.string(), s1);
            BOOST_TEST_EQ(u.host(), u.encoded_host().decode_to_string());
            auto rv = parse_ipv4_address(s);
            if(! rv)
                BOOST_TEST_EQ(u.encoded_host(), s);
            else
                BOOST_TEST_EQ(u.host(), rv->to_string());
            BOOST_TEST_EQ(u.host_address(), pct_string_view(s).decode_to_string());
            BOOST_TEST_EQ(u.host_ipv4_address(), ipv4_address());
            BOOST_TEST_EQ(u.host_ipv6_address(), ipv6_address());
            BOOST_TEST_EQ(u.host_ipvfuture(), "");
            BOOST_TEST_EQ(u.host_name(), pct_string_view(s).decode_to_string());
            BOOST_TEST_EQ(u.host_name(), u.encoded_host_name().decode_to_string());
        };

        set_host("", "//", host_type::name);
        set_host("127.0.0.1", "//127.0.0.1", host_type::ipv4);
        set_host("[1::6:c0a8:1]", "//[1::6:c0a8:1]", host_type::ipv6 );
        set_host("[v42.69]", "//[v42.69]", host_type::ipvfuture );
        set_host("www.example.com", "//www.example.com", host_type::name);
        set_host("%5b%3a", "//%255b%253a", host_type::name);

        set_encoded_host("", "//", host_type::name);
        set_encoded_host("127.0.0.1", "//127.0.0.1", host_type::ipv4);
        set_encoded_host("[1::6:c0a8:1]", "//[1::6:c0a8:1]", host_type::ipv6 );
        set_encoded_host("[v42.69]", "//[v42.69]", host_type::ipvfuture );
        set_encoded_host("www.example.com", "//www.example.com", host_type::name);
        set_encoded_host("%5b%3a", "//%5b%3a", host_type::name);
        set_encoded_host("%00", "//%00", host_type::name);

        set_host_address("", "//", host_type::name);
        set_host_address("127.0.0.1", "//127.0.0.1", host_type::ipv4);
        set_host_address("1::6:c0a8:1", "//[1::6:c0a8:1]", host_type::ipv6 );
        set_host_address("v42.69", "//[v42.69]", host_type::ipvfuture );
        set_host_address("www.example.com", "//www.example.com", host_type::name);
        set_host_address("%5b%3a", "//%255b%253a", host_type::name);

        set_encoded_host_address("", "//", host_type::name);
        set_encoded_host_address("127.0.0.1", "//127.0.0.1", host_type::ipv4);
        set_encoded_host_address("127%2e0.0.1", "//127%2e0.0.1", host_type::name);
        set_encoded_host_address("1::6:c0a8:1", "//[1::6:c0a8:1]", host_type::ipv6 );
        set_encoded_host_address("v42.69", "//[v42.69]", host_type::ipvfuture );
        set_encoded_host_address("www.example.com", "//www.example.com", host_type::name);
        set_encoded_host_address("%5b%3a", "//%5b%3a", host_type::name);

        set_host_ipv4_address("0.0.0.0", "//0.0.0.0");
        set_host_ipv4_address("127.0.0.1", "//127.0.0.1");
        set_host_ipv4_address("255.255.255.255", "//255.255.255.255");

        set_host_ipv6_address("1::6:c0a8:1", "//[1::6:c0a8:1]");

        set_host_ipvfuture("v42.69", "//[v42.69]");
        BOOST_TEST_THROWS(url().set_host_ipvfuture("127.0.0.1"), system_error);

        set_host_name("www.example.com", "//www.example.com");
        set_host_name("%5b%3a", "//%255b%253a");
        set_host_name("127.0.0.1", "//127%2E0%2E0%2E1");

        set_encoded_host_name("www.example.com", "//www.example.com");
        set_encoded_host_name("%5b%3a", "//%5b%3a");
        set_encoded_host_name("127.0.0.1", "//127%2E0%2E0%2E1");
        BOOST_TEST_THROWS(url().set_encoded_host_name("%go"), system_error);

        // self-intersection
        modify(
            "x://@?www.example.com",
            "x://@www.example.com?www.example.com",
            [](url_base& u)
            {
                u.set_encoded_host(
                    u.encoded_query());
            });
        modify(
            "x://@?www.example.com",
            "x://@www.example.com?www.example.com",
            [](url_base& u)
            {
                u.set_host(
                    u.encoded_query());
            });
        modify(
            "x://@?www.example.com",
            "x://@www.example.com?www.example.com",
            [](url_base& u)
            {
                u.set_host(
                    u.query());
            });
    }

    void
    testSetPort()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST(u.remove_port().string() == s2);
            BOOST_TEST(! u.has_port());
            BOOST_TEST(u.port().empty());
            BOOST_TEST_EQ(u.port_number(), 0);
        };

        auto const setn = [](string_view s1,
            std::uint16_t n, string_view s2)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST(u.set_port(n).string() == s2);
            BOOST_TEST(u.has_port());
            BOOST_TEST_EQ(u.port_number(), n);
        };

        auto const set = [](string_view s1,
            std::uint16_t n, string_view s2,
                string_view s3)
        {
            url u;
            BOOST_TEST_NO_THROW(u = url(s1));
            BOOST_TEST(u.set_port(s2).string() == s3);
            BOOST_TEST(u.has_port());
            BOOST_TEST_EQ(u.port_number(), n);
            BOOST_TEST_EQ(u.port(), s2);
        };

        BOOST_TEST_THROWS(
            url().set_port("x"),
            std::exception);

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

        // self-intersection
        modify(
            "x://@?65535",
            "x://@:65535?65535",
            [](url_base& u)
            {
                u.set_port(u.encoded_query());
            });
    }

    void
    run()
    {
        testSetScheme();
        testSetUserinfo();
        testSetUser();
        testSetPassword();
        testSetHost();
        testSetPort();
    }
};

TEST_SUITE(
    url_base_test,
    "boost.url.url_base");

} // urls
} // boost
