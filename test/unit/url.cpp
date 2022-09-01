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
#include <boost/url/encode.hpp>

#include "test_suite.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

/*  Legend

    '#' 0x23    ':' 0x3a
    '%' 0x25    '@' 0x40
    '&' 0x26    '[' 0x5b
    '=' 0x3d    ']' 0x5d
*/

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

struct url_test
{
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

    void
    testSpecial()
    {
        // max_size()
        {
            BOOST_TEST_GT(url::max_size(), 0u);

            url u;
            BOOST_TEST_GT(u.max_size(), 0u);
        }

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
                system_error);
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
            };
            check(
                "",
                "",
                "");
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
                    system_error);
                BOOST_TEST_THROWS(
                    u.set_encoded_query("%fg"),
                    system_error);
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
            };
            good("", "?");
            good("x", "?x");
            good("%41", "?%2541");
            good("%%fg", "?%25%25fg");
            good("{}", "?%7B%7D");

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
            BOOST_TEST_EQ(u.params().size(), 1);
        }

        // set_query
        {
            url u;
            BOOST_TEST(! u.has_query());
            u.set_query("!@#$%^&*()_+=-;:'{}[]|\\?/>.<,");
            BOOST_TEST(u.has_query());
            BOOST_TEST(u.encoded_query() ==
                "!@%23$%25%5E&*()_+=-;:'%7B%7D%5B%5D%7C%5C?/%3E.%3C,");
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
            };

            good("", "#", "");
            good("x", "#x", "x");
            good("%41", "#%2541", "%2541");
            good("%%fg", "#%25%25fg", "%25%25fg");
            good("{}", "#%7B%7D", "%7B%7D");
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
