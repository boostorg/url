//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/url.hpp>

#include <boost/url/url_view.hpp>
#include <boost/url/rfc/detail/charsets.hpp>

#include "test_suite.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace boost {
namespace urls {

// Transparent equal_to
template <class T = void>
struct equal_to {
    bool operator()( const T& lhs, const T& rhs ) const
    {
        return lhs == rhs;
    }
};

template <>
struct equal_to<void> {
    template <class T1, class T2>
    bool operator()( const T1& lhs, const T2& rhs ) const
    {
        return lhs == rhs;
    }
};

class url_test
{
public:
    static
    void
    modify(
        string_view before,
        string_view after,
        void (*pf)(url_base&))
    {
        url u(before);
        (*pf)(u);
        auto s = u.string();
        BOOST_TEST_EQ(s, after);
    }

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
            init.begin(),
            equal_to<>{}));
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
            BOOST_TEST_EQ(u2.string(), u.string());
        }
        {
            url u = parse_uri_reference("x://y/z?q#f").value();
            url u2 = parse_relative_ref("./").value();
            u2 = u;
            BOOST_TEST_EQ(u2.string(), u.string());
        }

        // move
        {
            url u = parse_uri_reference("x://y/z?q#f").value();
            url u2(std::move(u));
            BOOST_TEST(u.empty());
            BOOST_TEST_EQ(u2.string(), "x://y/z?q#f");
        }
        {
            url u = parse_uri_reference("x://y/z?q#f").value();
            url u2 = parse_relative_ref("./").value();
            u2 = std::move(u);
            BOOST_TEST(u.empty());
            BOOST_TEST_EQ(u2.string(), "x://y/z?q#f");
        }

        // url(string_view)
        {
            url u("http://example.com/path/to/file.txt?#");
        }
    }

    //--------------------------------------------

    void
    testCapacity()
    {
        // capacity
        {
            url u;
            BOOST_TEST_EQ(u.capacity(), 0u);
            BOOST_TEST(u.empty());
        }

        // reserve
        {
            url u;
            u.reserve(32);
            BOOST_TEST_GE(u.capacity(), 32u);
            u.reserve(16);
            BOOST_TEST_GE(u.capacity(), 16u);
            u.reserve(64);
            BOOST_TEST_GE(u.capacity(), 64u);
            u = url_view("http://example.com/path/to/file.txt?k=v");
            u.reserve(128);
            BOOST_TEST_GE(u.capacity(), 128u);
        }

        // clear
        {
            url u = parse_uri(
                "http://example.com/index.htm?q#f").value();
            BOOST_TEST_GT(u.capacity(), 0u);
            BOOST_TEST(! u.empty());
            u.clear();
            BOOST_TEST_GT(u.capacity(), 0u);
            BOOST_TEST(u.empty());
            BOOST_TEST_EQ(u.size(), 0u);
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
                u.remove_scheme().string() == s2);
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
                u.set_scheme(s2).string() == s3);
            BOOST_TEST_EQ(u.scheme(), s2);
            BOOST_TEST_EQ(u.scheme_id(), id);
        };

        auto const setid = [](
            string_view s1, scheme id,
            string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_scheme(id).string() == s2);
            BOOST_TEST_EQ(u.scheme_id(), id);
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

    void
    testUser()
    {
        auto const set = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_user(s2).string() == s3);
            auto s = pct_encode_to_string(
                s2, unreserved_chars + sub_delim_chars);
            u.set_user(pct_encoded_view(s));
            BOOST_TEST(u.string() == s3);
            BOOST_TEST_EQ(u.user(), s2);
            BOOST_TEST(u.has_userinfo());
        };

        auto const enc = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(u.set_encoded_user(
                s2).string() == s3);
            BOOST_TEST_EQ(u.encoded_user(), s2);
            BOOST_TEST(u.has_userinfo());
        };

        BOOST_TEST_THROWS(url().set_encoded_user(
            "%2"), std::invalid_argument);

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

    //--------------------------------------------

    void
    testPassword()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.remove_password().string() == s2);
            BOOST_TEST_EQ(u.encoded_password(), "");
            BOOST_TEST_EQ(u.password(), "");
        };

        auto const set = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_password(s2).string() == s3);
            auto s = pct_encode_to_string(
                s2, unreserved_chars +
                    sub_delim_chars + ':');
            BOOST_TEST(
                u.set_password(
                     pct_encoded_view(s)).string() == s3);
            BOOST_TEST_EQ(u.password(), s2);
            BOOST_TEST(u.has_userinfo());
        };

        auto const enc = [](
            string_view s1, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(u.set_encoded_password(
                s2).string() == s3);
            BOOST_TEST_EQ(u.encoded_password(), s2);
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

    void
    testUserinfo()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST_EQ(u.remove_userinfo().string(), s2);
            BOOST_TEST(u.encoded_userinfo().empty());
            BOOST_TEST(u.userinfo().empty());
            BOOST_TEST(! u.has_userinfo());
        };

        auto const set = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST_EQ(
                u.set_userinfo(s2).string(), s3);
            auto s = pct_encode_to_string(
                s2, unreserved_chars +
                    sub_delim_chars + ':');
            BOOST_TEST(
                u.set_userinfo(
                     pct_encoded_view(s)).string() == s3);
            BOOST_TEST_EQ(u.userinfo(), s2);
            BOOST_TEST(u.has_userinfo());
        };

        auto const enc = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_encoded_userinfo(s2).string() == s3);
            BOOST_TEST_EQ(u.encoded_userinfo(), s2);
            BOOST_TEST(u.has_userinfo());
        };

        BOOST_TEST_THROWS(url().set_encoded_userinfo(
            "%2"), std::exception);
        BOOST_TEST_THROWS(url().set_encoded_userinfo(
            "@"), std::exception);

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
    //--------------------------------------------

    void
    set(string_view s1,
        string_view s2,
        string_view s3,
        host_type ht = host_type::name)
    {
        url u = parse_uri_reference(s1).value();
        BOOST_TEST(
            u.set_encoded_host(s2).string() == s3);
        BOOST_TEST_EQ(u.encoded_host(), s2);
        BOOST_TEST_EQ(u.host_type(), ht);
    };

    void
    testHost()
    {
        auto const bad = [](string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST_THROWS(u.set_encoded_host(s2),
                std::exception);
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
            BOOST_TEST_EQ(u.string(), "//1.2.3.4");
            BOOST_TEST(u.host_type() ==
                host_type::ipv4);
            BOOST_TEST(u.ipv4_address() ==
                ipv4_address(0x01020304));
            BOOST_TEST(
                u.encoded_host() == "1.2.3.4");
            BOOST_TEST_EQ(u.host(), "1.2.3.4");
        }
        {
            url u;
            u.set_host("1.2.3.4");
            BOOST_TEST_EQ(u.string(), "//1.2.3.4");
            BOOST_TEST(u.host_type() ==
                host_type::ipv4);
            BOOST_TEST(u.ipv4_address() ==
                ipv4_address(0x01020304));
            BOOST_TEST(
                u.encoded_host() == "1.2.3.4");
            BOOST_TEST_EQ(u.host(), "1.2.3.4");
        }
        {
            url u;
            u.set_host(pct_encoded_view("1.2.3.4"));
            BOOST_TEST_EQ(u.string(), "//1.2.3.4");
            BOOST_TEST(u.host_type() ==
                host_type::ipv4);
            BOOST_TEST(u.ipv4_address() ==
                ipv4_address(0x01020304));
            BOOST_TEST(
                u.encoded_host() == "1.2.3.4");
            BOOST_TEST_EQ(u.host(), "1.2.3.4");
        }
        {
            url u;
            u.set_encoded_host("1.2.3.4");
            BOOST_TEST_EQ(u.string(), "//1.2.3.4");
            BOOST_TEST(u.host_type() ==
                host_type::ipv4);
            BOOST_TEST(u.ipv4_address() ==
                ipv4_address(0x01020304));
            BOOST_TEST(
                u.encoded_host() == "1.2.3.4");
            BOOST_TEST_EQ(u.host(), "1.2.3.4");
        }
        // ipv6
        {
            url u;
            u.set_host(ipv6_address());
            BOOST_TEST_EQ(u.string(), "//[::]");
            BOOST_TEST(u.host_type() ==
                host_type::ipv6);
            BOOST_TEST(u.ipv6_address() ==
                ipv6_address());
        }
        {
            url u;
            u.set_encoded_host("[1:2:3:4::]");
            BOOST_TEST_EQ(u.string(), "//[1:2:3:4::]");
            BOOST_TEST(u.host_type() ==
                host_type::ipv6);
            BOOST_TEST(
                u.encoded_host() == "[1:2:3:4::]");
            BOOST_TEST_EQ(u.host(), "[1:2:3:4::]");
        }
        // reg-name
        {
            url u;
            u.set_host("example.com");
            BOOST_TEST_EQ(u.string(), "//example.com");
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST_EQ(u.host(), "example.com");
            BOOST_TEST_EQ(u.encoded_host(), "example.com");
        }
        // reg-name
        {
            url u;
            u.set_host(pct_encoded_view("example.com"));
            BOOST_TEST_EQ(u.string(), "//example.com");
            BOOST_TEST(u.host_type() ==
                host_type::name);
            BOOST_TEST_EQ(u.host(), "example.com");
            BOOST_TEST_EQ(u.encoded_host(), "example.com");
        }

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
    testHostname()
    {
        BOOST_TEST(url_view(
            "").encoded_hostname() ==
            "");
        BOOST_TEST(url_view(
            "x:///").encoded_hostname() ==
            "");
        BOOST_TEST(url_view(
            "x://example.com").encoded_hostname() ==
            "example.com");
        BOOST_TEST(url_view(
            "x://example%2dcom").encoded_hostname() ==
            "example%2dcom");
        BOOST_TEST(url_view(
            "x://1.2.3.4").encoded_hostname() ==
            "1.2.3.4");
        BOOST_TEST(url_view(
            "x://[::]").encoded_hostname() ==
            "::");
        BOOST_TEST(url_view(
            "x://[v2.0]").encoded_hostname() ==
            "v2.0");

        //---

        BOOST_TEST(url_view(
            "").hostname() ==
            "");
        BOOST_TEST(url_view(
            "x:///").hostname() ==
            "");
        BOOST_TEST(url_view(
            "x://example.com").hostname() ==
            "example.com");
        BOOST_TEST(url_view(
            "x://example%2dcom").hostname() ==
            "example-com");
        BOOST_TEST(url_view(
            "x://1.2.3.4").hostname() ==
            "1.2.3.4");
        BOOST_TEST(url_view(
            "x://[::]").hostname() ==
            "::");
        BOOST_TEST(url_view(
            "x://[v2.0]").hostname() ==
            "v2.0");
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
                u.remove_port().string() == s2);
            BOOST_TEST(! u.has_port());
            BOOST_TEST(u.port().empty());
            BOOST_TEST_EQ(u.port_number(), 0);
        };

        auto const setn = [](string_view s1,
            std::uint16_t n, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_port(n).string() == s2);
            BOOST_TEST(u.has_port());
            BOOST_TEST_EQ(u.port_number(), n);
        };

        auto const set = [](string_view s1,
            std::uint16_t n, string_view s2,
                string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_port(s2).string() == s3);
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

    //--------------------------------------------

    void
    testAuthority()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST_EQ(u.remove_authority().string(), s2);
            BOOST_TEST(u.encoded_authority().empty());
            BOOST_TEST(! u.has_authority());
        };

        auto const set = [](string_view s1,
            string_view s2, string_view s3)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST(
                u.set_encoded_authority(s2).string() == s3);
            BOOST_TEST_EQ(u.encoded_authority(), s2);
            BOOST_TEST(u.has_authority());
        };

        BOOST_TEST_THROWS(
            url().set_encoded_authority("x:y"),
            std::exception);

        BOOST_TEST_THROWS(
            url().set_encoded_authority("%2"),
            std::exception);

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

        // self-intersection
        modify(
            "x://@?user:pass@example.com:8080",
            "x://user:pass@example.com:8080?user:pass@example.com:8080",
            [](url_base& u)
            {
                u.set_encoded_authority(
                    u.encoded_query());
            });
    }

    //--------------------------------------------

    void
    testOrigin()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST_EQ(u.remove_origin().string(), s2);
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
            BOOST_TEST_EQ(u.string(), "");
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST_EQ(u.string(), "/");
        }
        {
            url u = parse_relative_ref("/").value();
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST_EQ(u.string(), "/");
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST_EQ(u.string(), "");
        }
        {
            url u = parse_relative_ref("//").value();
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST_EQ(u.string(), "///");
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST_EQ(u.string(), "//");
        }
        {
            url u = parse_relative_ref("//x/y").value();
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(! u.set_path_absolute(false));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST_EQ(u.string(), "//x/y");
        }
        {
            url u = parse_uri("x:y").value();
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST_EQ(u.string(), "x:/y");
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST_EQ(u.string(), "x:y");
        }

        // set_encoded_path
        {
            // empty
            url u = parse_uri("x://y/path/to/file.txt?q#f").value();
            u.set_encoded_path("");
            BOOST_TEST_EQ(u.encoded_path(), "/");
            BOOST_TEST_EQ(u.string(), "x://y/?q#f");
        }
        {
            // path-abempty
            url u = parse_uri("x://y/path/to/file.txt?q#f").value();
            u.set_encoded_path("/x");
            BOOST_TEST_EQ(u.encoded_path(), "/x");
            BOOST_TEST_EQ(u.string(), "x://y/x?q#f");
            u.set_encoded_path("x/");
            BOOST_TEST_EQ(u.string(), "x://y/x/?q#f");
        }
        {
            // path-absolute
            url u = parse_relative_ref("/path/to/file.txt").value();
            u.set_encoded_path("/home/file.txt");
            BOOST_TEST_EQ(u.encoded_path(), "/home/file.txt");
            BOOST_TEST_EQ(u.string(), "/home/file.txt");
            u.set_encoded_path("//home/file.txt");
            equal(u, { "", "home", "file.txt" });
            BOOST_TEST_EQ(u.encoded_path(), "/.//home/file.txt");
            BOOST_TEST_THROWS(u.set_encoded_path("/home/%ile.txt"),
                std::invalid_argument);
        }
        {
            // path-rootless
            url u = parse_uri("x:mailto").value();
            u.set_encoded_path("file.txt");
            BOOST_TEST_EQ(u.encoded_path(), "file.txt");
            BOOST_TEST_EQ(u.string(), "x:file.txt");
            u.set_encoded_path(":file.txt");
            BOOST_TEST_EQ(u.encoded_path(), ":file.txt");
            BOOST_TEST_EQ(u.string(), "x::file.txt");
            // to path-absolute
            u.set_encoded_path("/file.txt");
            BOOST_TEST_EQ(u.encoded_path(), "/file.txt");
            BOOST_TEST_EQ(u.string(), "x:/file.txt");
        }
        {
            // path-noscheme
            url u = parse_relative_ref("mailto").value();
            u.set_encoded_path("file.txt");
            BOOST_TEST_EQ(u.encoded_path(), "file.txt");
            BOOST_TEST_EQ(u.string(), "file.txt");
            u.set_encoded_path(":file.txt");
            BOOST_TEST_EQ(u.encoded_path(), "./:file.txt");
            u.set_encoded_path("http:index.htm");
            BOOST_TEST_EQ(u.encoded_path(), "./http:index.htm");
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
                    u.string() == match);
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
                BOOST_TEST_EQ(u.string(), match);
                auto s = pct_encode_to_string(arg, pchars);
                u.set_path(pct_encoded_view(s));
                BOOST_TEST_EQ(u.string(), match);
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

        // self-intersection
        modify(
            "?/a/b/c",
            "/a/b/c?/a/b/c",
            [](url_base& u)
            {
                u.set_encoded_path(u.encoded_query());
            });
        modify(
            "?/a/b/c",
            "/a/b/c?/a/b/c",
            [](url_base& u)
            {
                u.set_path(u.encoded_query());
            });
        modify(
            "?/a/b/c",
            "/a/b/c?/a/b/c",
            [](url_base& u)
            {
                u.set_path(u.query());
            });
    }

    //--------------------------------------------

    void
    testQuery()
    {
        // has_query
        {
            {
                url u;
                BOOST_TEST(! u.has_query());
            }
            {
                url u("?");
                BOOST_TEST(u.has_query());
            }
            {
                url u("?x");
                BOOST_TEST(u.has_query());
            }
        }

        // remove_query
        {
            {
                url u;
                u.remove_query();
                BOOST_TEST(! u.has_query());
            }
            {
                url u("?");
                u.remove_query();
                BOOST_TEST(! u.has_query());
            }
            {
                url u("?x");
                u.remove_query();
                BOOST_TEST(! u.has_query());
            }
        }

        // set_encoded_query
        {
            {
                url u;
                u.set_encoded_query("");
                BOOST_TEST(u.has_query());
                BOOST_TEST_EQ(u.string(), "?");
                BOOST_TEST_EQ(u.encoded_query(), "");
            }
            {
                url u;
                u.set_encoded_query("x");
                BOOST_TEST(u.has_query());
                BOOST_TEST_EQ(u.string(), "?x");
                BOOST_TEST_EQ(u.encoded_query(), "x");
            }
            {
                url u;
                u.set_encoded_query("%41");
                BOOST_TEST(u.has_query());
                BOOST_TEST_EQ(u.string(), "?%41");
                BOOST_TEST_EQ(u.encoded_query(), "%41");
                BOOST_TEST_EQ(u.query(), "A");
            }
            {
                url u;
                BOOST_TEST_THROWS(
                    u.set_encoded_query("%%"),
                    std::invalid_argument);
                BOOST_TEST_THROWS(
                    u.set_encoded_query("%fg"),
                    std::invalid_argument);
            }
        }

        // set_query
        {
            auto good = [](
                string_view q, string_view us)
            {
                url u;
                u.set_query(q);
                BOOST_TEST(u.has_query());
                BOOST_TEST_EQ(u.string(), us);
                BOOST_TEST_EQ(u.query(), q);

                u.remove_query();
                auto s = pct_encode_to_string(
                    q, detail::query_chars);
                pct_decode_opts opt;
                opt.plus_to_space = true;
                auto es = pct_encoded_view(s, opt);
                u.set_query(es);
                BOOST_TEST(u.has_query());
                BOOST_TEST_EQ(u.string(), us);
                BOOST_TEST_EQ(u.query(), q);
            };
            good("", "?");
            good("x", "?x");
            good("%41", "?%2541");
            good("%%fg", "?%25%25fg");
            good("{}", "?%7b%7d");

            // issue #245
            {
                url u;
                u.set_query("");
                u.set_query("");
                BOOST_TEST_EQ(u.string(), "?");
            }
        }

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
            BOOST_TEST_EQ(u.encoded_query(), "query");
            BOOST_TEST_EQ(u.params().size(), 1u);
            BOOST_TEST_EQ(u.remove_query().has_query(), false);
            BOOST_TEST_EQ(u.encoded_query(), "");
            BOOST_TEST_EQ(u.query(), "");
            BOOST_TEST_EQ(u.params().size(), 0u);
            BOOST_TEST_EQ(u.encoded_params().size(), 0u);
        }

        // set_encoded_query
        {
            url u;
            BOOST_TEST(! u.has_query());
            u.set_encoded_query("k1=v1&k2=v2");
            BOOST_TEST(u.has_query());
            BOOST_TEST_EQ(u.params().size(), 2u);
            BOOST_TEST_EQ((*u.params().begin()).key, "k1");
            BOOST_TEST_EQ((*u.params().begin()).value, "v1");
            BOOST_TEST_EQ((*std::next(u.params().begin())).key, "k2");
            BOOST_TEST_EQ((*std::next(u.params().begin())).value, "v2");

            u.set_encoded_query("");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query().empty());
            BOOST_TEST_EQ(u.params().size(), 0u);

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
            BOOST_TEST_EQ(u.params().size(), 2u);
            BOOST_TEST_EQ((*u.params().begin()).key, "!@#$%^");
            BOOST_TEST_EQ((*u.params().begin()).value, "");
            BOOST_TEST_EQ((*std::next(u.params().begin())).key, "*()_ ");
            BOOST_TEST_EQ((*std::next(u.params().begin())).value,
                "-;:'{}[]|\\?/>.<,");
        }

        // self-intersection
        modify(
            "#abracadabra",
            "?abracadabra#abracadabra",
            [](url_base& u)
            {
                u.set_encoded_query(
                    u.encoded_fragment());
            });
        modify(
            "#abracadabra",
            "?abracadabra#abracadabra",
            [](url_base& u)
            {
                u.set_query(
                    u.encoded_fragment());
            });
        modify(
            "#abracadabra",
            "?abracadabra#abracadabra",
            [](url_base& u)
            {
                u.set_query(
                    u.fragment());
            });
    }

    void
    testFragment()
    {
        // has_fragment
        {
            {
                url u;
                BOOST_TEST(! u.has_fragment());
            }
            {
                url u("#");
                BOOST_TEST(u.has_fragment());
            }
            {
                url u("#x");
                BOOST_TEST(u.has_fragment());
            }
        }

        // remove_fragment
        {
            {
                url u;
                u.remove_fragment();
                BOOST_TEST(! u.has_fragment());
            }
            {
                url u("#");
                u.remove_fragment();
                BOOST_TEST(! u.has_fragment());
            }
            {
                url u("#x");
                u.remove_fragment();
                BOOST_TEST(! u.has_fragment());
            }
        }

        // set_encoded_fragment
        {
            {
                url u;
                u.set_encoded_fragment("");
                BOOST_TEST(u.has_fragment());
                BOOST_TEST_EQ(u.string(), "#");
                BOOST_TEST_EQ(u.encoded_fragment(), "");
            }
            {
                url u;
                u.set_encoded_fragment("x");
                BOOST_TEST(u.has_fragment());
                BOOST_TEST_EQ(u.string(), "#x");
                BOOST_TEST_EQ(u.encoded_fragment(), "x");
            }
            {
                url u;
                u.set_encoded_fragment("%41");
                BOOST_TEST(u.has_fragment());
                BOOST_TEST_EQ(u.string(), "#%41");
                BOOST_TEST_EQ(u.encoded_fragment(), "%41");
                BOOST_TEST_EQ(u.fragment(), "A");
            }
            {
                url u;
                BOOST_TEST_THROWS(
                    u.set_encoded_fragment("%%"),
                    std::exception);
                BOOST_TEST_THROWS(
                    u.set_encoded_fragment("%fg"),
                    std::exception);
            }
        }

        // set_fragment
        {
            auto good = [](
                string_view f, string_view h, string_view ef)
            {
                url u;
                u.set_fragment(f);
                BOOST_TEST(u.has_fragment());
                BOOST_TEST_EQ(u.string(), h);
                BOOST_TEST_EQ(u.encoded_fragment(), ef);
                BOOST_TEST_EQ(u.fragment(), f);

                auto s = pct_encode_to_string(
                    f, detail::fragment_chars);
                u.set_fragment(pct_encoded_view(s));
                BOOST_TEST(u.has_fragment());
                BOOST_TEST_EQ(u.string(), h);
                BOOST_TEST_EQ(u.encoded_fragment(), ef);
                BOOST_TEST_EQ(u.fragment(), f);
            };

            good("", "#", "");
            good("x", "#x", "x");
            good("%41", "#%2541", "%2541");
            good("%%fg", "#%25%25fg", "%25%25fg");
            good("{}", "#%7b%7d", "%7b%7d");
        }

        // self-intersection
        modify(
            "?abracadabra",
            "?abracadabra#abracadabra",
            [](url_base& u)
            {
                u.set_encoded_fragment(
                    u.encoded_query());
            });
        modify(
            "?abracadabra",
            "?abracadabra#abracadabra",
            [](url_base& u)
            {
                u.set_fragment(
                    u.encoded_query());
            });
        modify(
            "?abracadabra",
            "?abracadabra#abracadabra",
            [](url_base& u)
            {
                u.set_fragment(
                    u.query());
            });
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
        BOOST_TEST_EQ(u.string(), s1);
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
                BOOST_TEST_EQ(u.string(), s1);
            }
            {
                url u(u0);
                u.encoded_segments() = init;
                equal(u.segments(), init);
                equal(u.encoded_segments(), init);
                BOOST_TEST_EQ(u.string(), s1);
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
            result<void> rv = resolve(ub, ur, u);
            if(! BOOST_TEST( rv.has_value() ))
                return;
            BOOST_TEST_EQ(u.string(), m);
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
            BOOST_TEST_EQ(ss.str(), "http://x***");
        }
    }

    //--------------------------------------------

    void
    testNormalize()
    {
        // normalize
        {
            auto check = [](string_view before,
                            string_view after)
            {
                url u1 = parse_uri(before).value();
                url_view u2 = parse_uri(after).value();
                BOOST_TEST_EQ(u1.compare(u2), 0);
                BOOST_TEST_EQ(u1, u2);
                u1.normalize();
                BOOST_TEST_EQ(u1.string(), after);
                std::hash<url_view> h;
                BOOST_TEST_EQ(h(u1), h(u2));
                h = std::hash<url_view>(10);
                BOOST_TEST_EQ(h(u1), h(u2));
            };

            check("HtTp://cPpAlLiAnCe.oRG/",
                  "http://cppalliance.org/");
            check("http://%2a%2b%2C%2f%3A.org/",
                  "http://%2A%2B%2C%2F%3A.org/");
            check("http://%63%70%70%61%6c%6Ci%61n%63e.org/",
                  "http://cppalliance.org/");
            check("http://%43%70%50%61%6c%6Ci%61n%43e.org/",
                  "http://cppalliance.org/");
            check("http://cppalliance.org/a/b/c/./../../g",
                  "http://cppalliance.org/a/g");
            check("http://cppalliance.org/aa/bb/cc/./../../gg",
                  "http://cppalliance.org/aa/gg");
            check("http://cppalliance.org/a/b/../../g",
                  "http://cppalliance.org/g");
            check("http://cppalliance.org/a/b/../../../g",
                  "http://cppalliance.org/g");
            check("http://cppalliance.org/..",
                  "http://cppalliance.org/");
            check("http://cppalliance.org?%61=b",
                  "http://cppalliance.org?a=b");
        }

        // remove_dot_segments
        {
            auto check = [](string_view p,
                            string_view e) {
                url u1 = parse_relative_ref(p).value();
                u1.normalize_path();
                BOOST_TEST_EQ(u1.encoded_path(), e);
                url u2 = parse_relative_ref(e).value();
                BOOST_TEST_EQ(u1.compare(u2), 0);
                BOOST_TEST_EQ(u1, u2);
                std::hash<url_view> h;
                BOOST_TEST_EQ(h(u1), h(u2));
                h = std::hash<url_view>(10);
                BOOST_TEST_EQ(h(u1), h(u2));
            };

            check("/a/b/c/./../../g", "/a/g");
            check("/aa/bb/cc/./../../gg", "/aa/gg");
            check("../a/b/c/./../../g", "../a/g");
            check("./a/b/c/./../../g", "a/g");
            check(".././a/b/c/./../../g", "../a/g");
            check("%2E%2E/./a/b/c/./../../g", "../a/g");
            check("/a/b/../../g", "/g");
            check("/a/b/../../../g", "/g");
            check("mid/content=5/../6", "mid/6");
            check("mid/content=5/../6/.", "mid/6/");
            check("mid/content=5/../6/..", "mid/");
            check("/..", "/");
            check(".", "");
            check("..", "..");
            check("", "");
        }

        // inequality
        {
            auto check = [](string_view e1,
                            string_view e2,
                            int cmp) {
                url_view u1 = parse_uri(e1).value();
                url_view u2 = parse_uri(e2).value();
                BOOST_TEST_EQ(u1.compare(u2), cmp);
                BOOST_TEST_EQ(u2.compare(u1), -cmp);
                BOOST_TEST_NE(u1, u2);
                BOOST_TEST_EQ((u1 < u2), (cmp < 0));
                BOOST_TEST_EQ((u1 <= u2), (cmp <= 0));
                BOOST_TEST_EQ((u1 > u2), (cmp > 0));
                BOOST_TEST_EQ((u1 >= u2), (cmp >= 0));
                std::hash<url_view> h;
                BOOST_TEST_NE(h(u1), h(u2));
                h = std::hash<url_view>(10);
                BOOST_TEST_NE(h(u1), h(u2));
            };

            check("http://cppalliance.org", "https://cppalliance.org", -1);
            check("https://cppalliance.org", "httpz://cppalliance.org", -1);
            check("http://boost.org", "http://cppalliance.org", -1);
            check("http://boost.orgg", "http://boost.org", +1);
            check("http://cppalliance.org/%2E%2E/./b/b/c/./../../g", "http://cppalliance.org/../a/g", +1);
            check("http://alice@cppalliance.org", "http://bob@cppalliance.org", -1);
            check("http://alice:passwd@cppalliance.org", "http://alice:pass@cppalliance.org", 1);
            check("http://alice:pass1@cppalliance.org", "http://alice:pass2@cppalliance.org", -1);
            check("http://cppalliance.org", "http://cppalliance.org:81", -1);
            check("http://cppalliance.org:80", "http://cppalliance.org:81", -1);
            check("http://cppalliance.org?l=v", "http://cppalliance.org?k=v", 1);
            check("http://cppalliance.org?%6C=v", "http://cppalliance.org?k=v", 1);
            check("http://cppalliance.org#frag", "http://cppalliance.org#glob", -1);
            check("http://cppalliance.org#fra", "http://cppalliance.org#frag", -1);
            check("http://cppalliance.org#frag", "http://cppalliance.org#fra", 1);
        }

        // path inequality
        {
            auto check = [](string_view e1,
                            string_view e2,
                            int cmp) {
                url_view u1 = parse_relative_ref(e1).value();
                url_view u2 = parse_relative_ref(e2).value();
                BOOST_TEST_EQ(u1.compare(u2), cmp);
                BOOST_TEST_EQ(u2.compare(u1), -cmp);
                std::hash<url_view> h;
                BOOST_TEST_NE(int(h(u1) == h(u2)), cmp);
                h = std::hash<url_view>(10);
                BOOST_TEST_NE(int(h(u1) == h(u2)), cmp);
            };

            check("a/g", "/../g", 1);
            check("./a/b/c/./../../g", "/a/b/../../../g", 1);
            check("%2E/a/b/c/./../../g", "/a/b/../../../g", 1);
            check("/../g", "a/g", -1);
            check("/a/b/../../../g", "./a/b/c/./../../g", -1);
            check("../g", "a/g", -1);
            check("a/b/../../../g", "./a/b/c/./../../g", -1);
            check("a/b/../../../%67", "./a/b/c/./../../g", -1);
            check("/aa/g", "/aa/gg", -1);
            check("../a/b", "..%2Fa/b", 1);
            check("../a/b", "%2E%2E%2Fa/b", 1);
            check("../a/b", "%2E%2E/a/b", 0);
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
        testHostname();
        testPort();
        testAuthority();
        testOrigin();
        testPath();
        testQuery();
        testFragment();
        testSegments();
        testResolution();
        testOstream();
        testNormalize();
    }
};

TEST_SUITE(url_test, "boost.url.url");

} // urls
} // boost

