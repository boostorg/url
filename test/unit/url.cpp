//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

// Test that header file is self-contained.
#include <boost/url/url.hpp>

#include <boost/url/encode.hpp>
#include <boost/url/parse.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/rfc/detail/charsets.hpp>
#include <boost/url/detail/remove_dot_segments.hpp>

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
        auto s = u.buffer();
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
            BOOST_TEST_EQ(u2.buffer(), u.buffer());
        }
        {
            url u = parse_uri_reference("x://y/z?q#f").value();
            url u2 = parse_relative_ref("./").value();
            u2 = u;
            BOOST_TEST_EQ(u2.buffer(), u.buffer());
        }

        // move
        {
            url u = parse_uri_reference("x://y/z?q#f").value();
            url u2(std::move(u));
            BOOST_TEST(u.empty());
            BOOST_TEST_EQ(u2.buffer(), "x://y/z?q#f");
        }
        {
            url u = parse_uri_reference("x://y/z?q#f").value();
            url u2 = parse_relative_ref("./").value();
            u2 = std::move(u);
            BOOST_TEST(u.empty());
            BOOST_TEST_EQ(u2.buffer(), "x://y/z?q#f");
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
            u.reserve(0);
            BOOST_TEST_GE(u.capacity(), 0);
            BOOST_TEST_EQ(u.c_str()[0], '\0');
        }
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
    testOrigin()
    {
        auto const remove = [](
            string_view s1, string_view s2)
        {
            url u = parse_uri_reference(s1).value();
            BOOST_TEST_EQ(u.remove_origin().buffer(), s2);
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
            BOOST_TEST_EQ(u.buffer(), "");
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST_EQ(u.buffer(), "/");
        }
        {
            url u = parse_relative_ref("/").value();
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST_EQ(u.buffer(), "/");
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST_EQ(u.buffer(), "");
        }
        {
            url u = parse_relative_ref("//").value();
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST_EQ(u.buffer(), "///");
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST_EQ(u.buffer(), "//");
        }
        {
            url u = parse_relative_ref("//x/y").value();
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST(! u.set_path_absolute(false));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST_EQ(u.buffer(), "//x/y");
        }
        {
            url u = parse_uri("x:y").value();
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST(u.set_path_absolute(true));
            BOOST_TEST(u.is_path_absolute());
            BOOST_TEST_EQ(u.buffer(), "x:/y");
            BOOST_TEST(u.set_path_absolute(false));
            BOOST_TEST(! u.is_path_absolute());
            BOOST_TEST_EQ(u.buffer(), "x:y");
        }

        // set_encoded_path
        {
            // empty
            url u = parse_uri("x://y/path/to/file.txt?q#f").value();
            u.set_encoded_path("");
            BOOST_TEST_EQ(u.encoded_path(), "/");
            BOOST_TEST_EQ(u.buffer(), "x://y/?q#f");
        }
        {
            // path-abempty
            url u = parse_uri("x://y/path/to/file.txt?q#f").value();
            u.set_encoded_path("/x");
            BOOST_TEST_EQ(u.encoded_path(), "/x");
            BOOST_TEST_EQ(u.buffer(), "x://y/x?q#f");
            u.set_encoded_path("x/");
            BOOST_TEST_EQ(u.buffer(), "x://y/x/?q#f");
        }
        {
            // path-absolute
            url u = parse_relative_ref("/path/to/file.txt").value();
            u.set_encoded_path("/home/file.txt");
            BOOST_TEST_EQ(u.encoded_path(), "/home/file.txt");
            BOOST_TEST_EQ(u.buffer(), "/home/file.txt");
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
            BOOST_TEST_EQ(u.buffer(), "x:file.txt");
            u.set_encoded_path(":file.txt");
            BOOST_TEST_EQ(u.encoded_path(), ":file.txt");
            BOOST_TEST_EQ(u.buffer(), "x::file.txt");
            // to path-absolute
            u.set_encoded_path("/file.txt");
            BOOST_TEST_EQ(u.encoded_path(), "/file.txt");
            BOOST_TEST_EQ(u.buffer(), "x:/file.txt");
        }
        {
            // path-noscheme
            url u = parse_relative_ref("mailto").value();
            u.set_encoded_path("file.txt");
            BOOST_TEST_EQ(u.encoded_path(), "file.txt");
            BOOST_TEST_EQ(u.buffer(), "file.txt");
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
                    u.buffer() == match);
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
                BOOST_TEST_EQ(u.buffer(), match);
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

        // crash
        {
            url u;
            u.set_path("");
            BOOST_TEST(u.empty());
        }
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
                BOOST_TEST_EQ(u.buffer(), "#");
                BOOST_TEST_EQ(u.encoded_fragment(), "");
            }
            {
                url u;
                u.set_encoded_fragment("x");
                BOOST_TEST(u.has_fragment());
                BOOST_TEST_EQ(u.buffer(), "#x");
                BOOST_TEST_EQ(u.encoded_fragment(), "x");
            }
            {
                url u;
                u.set_encoded_fragment("%41");
                BOOST_TEST(u.has_fragment());
                BOOST_TEST_EQ(u.buffer(), "#%41");
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
                BOOST_TEST_EQ(u.buffer(), h);
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
        BOOST_TEST_EQ(u.buffer(), s1);
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
                //equal(u.encoded_segments(), init);
                BOOST_TEST_EQ(u.buffer(), s1);
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
            BOOST_TEST_EQ(u.buffer(), m);

            // in place resolution
            url base( ub );
            rv = base.resolve( ur );
            if(! BOOST_TEST( rv.has_value() ))
                return;
            BOOST_TEST_EQ(base.buffer(), m);
        };

        check("g:h"          , "g:h");
        check("g"            , "http://a/b/c/g");
        check("./g"          , "http://a/b/c/g");
        check("g/"           , "http://a/b/c/g/");
        check("/g"           , "http://a/g");
        check("//g"          , "http://g");
        check("//g/a/../a"   , "http://g/a");
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
        check("%2E%2E"       , "http://a/b/");
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

        {
            url u("path/to/file.txt");
            result<void> r = u.resolve(url_view("g/../h"));
            BOOST_TEST(r.has_error());
            BOOST_TEST(r.error() == error::not_a_base);
        }
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
                url u1 = parse_uri_reference(before).value();
                url_view u2 = parse_uri_reference(after).value();
                BOOST_TEST_EQ(u1.compare(u2), 0);
                BOOST_TEST_EQ(u1, u2);
                u1.normalize();
                BOOST_TEST_EQ(u1.buffer(), after);
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
                  "http://cppalliance.org/../g");
            check("http://cppalliance.org/..",
                  "http://cppalliance.org/..");
            check("http://cppalliance.org?%61=b",
                  "http://cppalliance.org?a=b");
            // issue 396
            check("/.//my:sharona",
                  "/.//my:sharona");
            check("/././/my:sharona",
                  "/.//my:sharona");
            check(".//my:sharona",
                  ".//my:sharona");
            check("././/my:sharona",
                  ".//my:sharona");
            // issue 395
            check("./my:sharona",
                  "./my:sharona");
            check("././my:sharona",
                  "./my:sharona");
            // issue 391
            check("my%3Asharona",
                  "./my:sharona");
        }

        // normalize path
        {
            auto check = [](string_view p,
                            string_view e) {
                // normalize
                url u1 = parse_relative_ref(p).value();
                u1.normalize_path();
                BOOST_TEST_EQ(u1.encoded_path(), e);
                url u2 = parse_relative_ref(e).value();
                BOOST_TEST_EQ(u1.compare(u2), 0);
                BOOST_TEST_EQ(u1, u2);

                // remove_dot_segments
                std::string str;
                str.resize(p.size());
                std::size_t n =
                    urls::detail::remove_dot_segments(
                    &str[0], &str[0] + str.size(), p);
                str.resize(n);
                BOOST_TEST_EQ(str, e);

                // hash
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
            check("%2e%2E/./a/b/c/./../../g", "../a/g");
            check("/a/b/../../g", "/g");
            check("/a/b/../../../g", "/../g");
            check("mid/content=5/../6", "mid/6");
            check("mid/content=5/../6/.", "mid/6/");
            check("mid/content=5/../6/..", "mid/");
            check("a/../a"       , "a");
            check("a/.."       , "");
            check("/..", "/..");
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

    void
    testSwap()
    {
        url u1( "http://a.com");
        url u2( "http://b.com");
        swap(u1, u2);
        BOOST_TEST_EQ(u1.buffer(), "http://b.com");
        BOOST_TEST_EQ(u2.buffer(), "http://a.com");

        swap(u1, u1);
        BOOST_TEST_EQ(u1.buffer(), "http://b.com");
    }

    //--------------------------------------------

    void
    run()
    {
        testSpecial();
        testCapacity();
        testOrigin();
        testPath();
        testFragment();
        testSegments();
        testResolution();
        testOstream();
        testNormalize();
        testSwap();
    }
};

TEST_SUITE(url_test, "boost.url.url");

} // urls
} // boost
