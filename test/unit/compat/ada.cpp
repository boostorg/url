//
// Copyright (c) 2023 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

// Test that header file is self-contained.
#include <boost/url/url.hpp>
#include <boost/url/parse.hpp>

#include "test_suite.hpp"

#ifdef BOOST_TEST_CSTR_EQ
#undef BOOST_TEST_CSTR_EQ
#define BOOST_TEST_CSTR_EQ(expr1,expr2) \
    BOOST_TEST_EQ( boost::urls::detail::to_sv(expr1), boost::urls::detail::to_sv(expr2) )
#endif

namespace boost {
namespace urls {

struct ada_test
{
    static
    void
    setHostShouldReturnFalseSometimes()
    {
        result<url> r = parse_uri("mailto:a@b.com");
        BOOST_TEST_CSTR_EQ(r->scheme(), "mailto");
        BOOST_TEST_CSTR_EQ(r->path(), "a@b.com");
        // This is an invalid operation in ada, which is reasonable since
        // the path is relative, so it can't have a host.
        // Boost.URL will encode the host, add any required prefixes, and
        // make the path absolute.
        BOOST_TEST_NO_THROW(r->set_encoded_host("something"));
        BOOST_TEST_CSTR_EQ(r->scheme(), "mailto");
        BOOST_TEST_CSTR_EQ(r->host(), "something");
        BOOST_TEST_CSTR_EQ(r->path(), "/a@b.com");
        BOOST_TEST_CSTR_EQ(r->buffer(), "mailto://something/a@b.com");
    }

    static
    void
    setHostShouldReturnTrueSometimes()
    {
        result<url> r = parse_uri("https://www.google.com");
        BOOST_TEST_NO_THROW(r->set_encoded_host("something"));
    }

    static
    void
    setHostnameShouldReturnFalseSometimes()
    {
        result<url> r = parse_uri("mailto:a@b.com");
        // This is an invalid operation in ada, while Boost.URL will
        // encode the host and adjust the URL.
        BOOST_TEST_NO_THROW(r->set_host_address("something"));
        BOOST_TEST_CSTR_EQ(r->scheme(), "mailto");
        BOOST_TEST_CSTR_EQ(r->host(), "something");
        BOOST_TEST_CSTR_EQ(r->path(), "/a@b.com");
        BOOST_TEST_CSTR_EQ(r->buffer(), "mailto://something/a@b.com");
    }

    static
    void
    setHostnameShouldReturnTrueSometimes()
    {
        result<url> r = parse_uri("https://www.google.com");
        BOOST_TEST_NO_THROW(r->set_host_address("something"));
    }

    static
    void
    readme1()
    {
        result<url> u = parse_uri("https://www.google.com");
        BOOST_TEST(u);
    }

    static
    void
    readme2()
    {
        result<url> u = parse_uri("https://www.google.com");
        if (!BOOST_TEST(u))
            return;
        u->set_encoded_user("username");
        u->set_encoded_password("password");
        // Ada would add a "/" path at the end of the authority
        // Boost.URL keeps the original path
        BOOST_TEST_CSTR_EQ(u->buffer(), "https://username:password@www.google.com");
    }

    static
    void
    readme3()
    {
        result<url> u = parse_uri("https://www.google.com");
        if (!BOOST_TEST(u))
            return;
        u->set_scheme("wss");
        BOOST_TEST_CSTR_EQ(u->scheme(), "wss");
        // Ada would add a "/" path at the end of the authority
        // Boost.URL keeps the original path
        BOOST_TEST_CSTR_EQ(u->buffer(), "wss://www.google.com");
    }

    static
    void
    readme4()
    {
        result<url> u = parse_uri("https://www.google.com");
        if (!BOOST_TEST(u))
            return;
        u->set_encoded_host("github.com");
        BOOST_TEST_CSTR_EQ(u->encoded_host(), "github.com");
    }

    static
    void
    readme5()
    {
        result<url> u = parse_uri("https://www.google.com");
        if (!BOOST_TEST(u))
            return;
        u->set_port("8080");
        BOOST_TEST_CSTR_EQ(u->port(), "8080");
    }

    static
    void
    readme6()
    {
        result<url> u = parse_uri("https://www.google.com");
        if (!BOOST_TEST(u))
            return;
        u->set_encoded_path("/my-super-long-path");
        BOOST_TEST_CSTR_EQ(u->encoded_path(), "/my-super-long-path");
    }

    static
    void
    readme7()
    {
        result<url> u = parse_uri("https://www.google.com");
        if (!BOOST_TEST(u))
            return;
        u->set_encoded_query("target=self");
        // Ada would return the "?" prefix with the query
        // Boost.URL only returns the query
        BOOST_TEST_CSTR_EQ(u->encoded_query(), "target=self");
    }

    static
    void
    readme8()
    {
        result<url> u = parse_uri("https://www.google.com");
        if (!BOOST_TEST(u))
            return;
        u->set_encoded_fragment("is-this-the-real-life");
        // Ada would return the "#" prefix with the fragment
        // Boost.URL only returns the fragment
        BOOST_TEST_CSTR_EQ(u->encoded_fragment(), "is-this-the-real-life");
    }

    static
    void
    nodejs1()
    {
        auto base = parse_uri("http://other.com/");
        if (!BOOST_TEST(base))
            return;
        url u;
        auto r = resolve(*base, url_view("http://GOOgoo.com"), u);
        BOOST_TEST(r);
    }

    static
    void
    nodejs2()
    {
        // Ada supports URLs with whitespaces so that these whitespaces
        // are not considered part of the path
        // Boost.URL doesn't support whitespaces in the URL
        result<url> u = parse_uri("data:space    ?test");
        BOOST_TEST_NOT(u);
        u = parse_uri("data:space%20%20%20%20?test");
        if (!BOOST_TEST(u))
            return;
        BOOST_TEST_CSTR_EQ(u->encoded_query(), "test");
        u->set_encoded_query("");
        BOOST_TEST_CSTR_EQ(u->encoded_query(), "");
        BOOST_TEST_NOT(u->encoded_path() == "space");
        BOOST_TEST_CSTR_EQ(u->encoded_path(), "space%20%20%20%20");
        BOOST_TEST_CSTR_EQ(u->path(), "space    ");
        // Ada would remove the query is it is set to ""
        // Boost.URL keeps the empty query
        BOOST_TEST_NOT(u->buffer() == "data:space%20%20%20%20");
        BOOST_TEST_CSTR_EQ(u->buffer(), "data:space%20%20%20%20?");
        u->remove_query();
        BOOST_TEST_CSTR_EQ(u->buffer(), "data:space%20%20%20%20");
    }

    static
    void
    nodejs3()
    {
        // Ada supports URLs with whitespaces so that these whitespaces
        // are not considered part of the path
        // Boost.URL doesn't support whitespaces in the URL
        result<url> u = parse_uri("data:space    ?test#test");
        BOOST_TEST_NOT(u);
        u = parse_uri("data:space%20%20%20%20?test#test");
        if (!BOOST_TEST(u))
            return;
        BOOST_TEST_CSTR_EQ(u->encoded_query(), "test");
        BOOST_TEST_CSTR_EQ(u->encoded_fragment(), "test");
        u->set_encoded_query("");
        BOOST_TEST_CSTR_EQ(u->encoded_query(), "");
        BOOST_TEST_CSTR_EQ(u->path(), "space    ");
        BOOST_TEST_CSTR_EQ(u->buffer(), "data:space%20%20%20%20?#test");
    }

    static
    void
    nodejs4()
    {
        result<url> u = parse_uri("file:///var/log/system.log");
        if (!BOOST_TEST(u))
            return;
        // This test uses set_href in ada.
        // boost.url does not support this operation.
        u = parse_uri("http://0300.168.0xF0");
        if (!BOOST_TEST(u))
            return;
        // Ada returns ":" with the scheme. Boost.URL doesn't.
        BOOST_TEST_NOT(u->scheme() == "http:");
        BOOST_TEST_CSTR_EQ(u->scheme(), "http");
        // Ada converts "0300.168.0xF0" to "192.168.0.240"
        // Boost.URL doesn't, although "0300.168.0xF0" is a valid reg-name
        BOOST_TEST_NOT(u->buffer() == "http://192.168.0.240/");
        BOOST_TEST_CSTR_EQ(u->buffer(), "http://0300.168.0xF0");
        BOOST_TEST_CSTR_EQ(u->encoded_host_address(), "0300.168.0xF0");
    }

    static
    void
    adaBasicTests()
    {
        setHostShouldReturnFalseSometimes();
        setHostShouldReturnTrueSometimes();
        setHostnameShouldReturnFalseSometimes();
        setHostnameShouldReturnTrueSometimes();
        readme1();
        readme2();
        readme3();
        readme4();
        readme5();
        readme6();
        readme7();
        readme8();
        nodejs1();
        nodejs2();
        nodejs3();
        nodejs4();
    }

    static
    void
    urlTestData()
    {
        // Based on http://trac.webkit.org/browser/trunk/LayoutTests/fast/url/script-tests/segments.js
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://example\t.\norg");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://user:pass@foo:21/bar;par?b#c");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://user:pass@foo:21/bar;par?b#c"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "user");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "21");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/bar;par");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "b");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "c");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("https://test:@test");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "https://test@test"
            BOOST_TEST_CSTR_EQ(u.scheme(), "https");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "test");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "test");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("https://:@test");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "https://test"
            BOOST_TEST_CSTR_EQ(u.scheme(), "https");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "test");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-special://test:@test/x");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-special://test@test/x"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-special");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "test");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "test");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/x");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-special://:@test/x");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-special://test/x"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-special");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "test");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/x");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:foo.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/foo/foo.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/foo.com");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("\t   :foo.com   \n");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference(" foo.com  ");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("a:\t foo.com");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("http://f:21/ b ? d # e ");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("lolscheme:x x#x x");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://f:/c");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://f/c"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "f");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/c");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://f:0/c");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://f:0/c"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "f");
            // Boost.URL does not compress port values
            BOOST_TEST_EQ(u.port_number(), 0);
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/c");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://f:00000000000000/c");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://f:0/c"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "f");
            // Boost.URL does not compress port values
            BOOST_TEST_EQ(u.port_number(), 0);
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/c");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://f:00000000000000000000080/c");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://f/c"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "f");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/c");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            result<url> ref = parse_uri_reference("http://f:b/c");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("http://f: /c");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://f:\n/c");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            result<url> ref = parse_uri_reference("http://f:fifty-two/c");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            result<url> ref = parse_uri_reference("http://f:999999/c");
            // Boost.URL does not fail on port overflow
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            result<url> ref = parse_uri_reference("non-special://f:999999/c");
            // Boost.URL does not fail on port overflow
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("http://f: 21 / b ? d # e ");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not fail on empty references
            result<url> ref = parse_uri_reference("");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("  \t");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference(":foo.com/");
            // Boost.URL requires segment-nz-nc
            // A URL should have a scheme or the first path segment
            // cannot contain a colon (":")
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference(":foo.com\\");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference(":");
            // Boost.URL requires segment-nz-nc
            // A URL should have a scheme or the first path segment
            // cannot contain a colon (":")
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference(":a");
            // Boost.URL requires segment-nz-nc
            // A URL should have a scheme or the first path segment
            // cannot contain a colon (":")
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference(":/");
            // Boost.URL requires segment-nz-nc
            // A URL should have a scheme or the first path segment
            // cannot contain a colon (":")
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference(":\\");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference(":#");
            // Boost.URL requires segment-nz-nc
            // A URL should have a scheme or the first path segment
            // cannot contain a colon (":")
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/foo/bar#"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/foo/bar#/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("#\\");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#;?");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/foo/bar#;?"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), ";?");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("?");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/foo/bar?"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference(":23");
            // Boost.URL requires segment-nz-nc
            // A URL should have a scheme or the first path segment
            // cannot contain a colon (":")
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/:23");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/:23"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/:23");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("\\x");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("\\\\x\\hello");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("::");
            // Boost.URL requires segment-nz-nc
            // A URL should have a scheme or the first path segment
            // cannot contain a colon (":")
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("::23");
            // Boost.URL requires segment-nz-nc
            // A URL should have a scheme or the first path segment
            // cannot contain a colon (":")
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("foo://");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "foo://"
            BOOST_TEST_CSTR_EQ(u.scheme(), "foo");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://a:b@c:29/d");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://a:b@c:29/d"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "a");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "c");
            BOOST_TEST_CSTR_EQ(u.port(), "29");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/d");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http::@c:29");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/foo/:@c:29"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/:@c:29");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "&" in username
            // Expected username in Ada: "&a"
            result<url> ref = parse_uri_reference("http://&a:foo(b]c@d:2/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not passwords with unencoded "@"
            // Expected password in Ada: "%3A%40c"
            result<url> ref = parse_uri_reference("http://::@c@d:2");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://foo.com:b@d/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://foo.com:b@d/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "foo.com");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "d");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://foo.com/\\@");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http:\\\\foo.com\\");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http:\\\\a\\b:c\\d@foo.com\\");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("foo:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "foo:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("foo:/bar.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "foo:/bar.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/bar.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("foo://///////");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "foo://///////"
            BOOST_TEST_CSTR_EQ(u.scheme(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "///////");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("foo://///////bar.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "foo://///////bar.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "///////bar.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("foo:////://///");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "foo:////://///"
            BOOST_TEST_CSTR_EQ(u.scheme(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//://///");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("c:/foo");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "c:/foo"
            BOOST_TEST_CSTR_EQ(u.scheme(), "c");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("//foo/bar");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://foo/bar"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/bar");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://foo/path;a??e#f#g");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://foo/path;a??e#f#g"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/path;a");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "?e");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "f#g");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://foo/abcd?efgh?ijkl");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://foo/abcd?efgh?ijkl"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/abcd");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "efgh?ijkl");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://foo/abcd#foo?bar");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://foo/abcd#foo?bar"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/abcd");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "foo?bar");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "[" in paths
            result<url> ref = parse_uri_reference("[61:24:74]:98");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "[" in paths
            result<url> ref = parse_uri_reference("http:[61:27]/:foo");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            result<url> ref = parse_uri_reference("http://[1::2]:3:4");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            result<url> ref = parse_uri_reference("http://2001::1");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            result<url> ref = parse_uri_reference("http://2001::1]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            result<url> ref = parse_uri_reference("http://2001::1]:80");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://[2001::1]");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://[2001::1]"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://[::127.0.0.1]");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://[::7f00:1]"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "[::7f00:1]"
            BOOST_TEST_NOT(u.encoded_host() == "[::7f00:1]");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            result<url> ref = parse_uri_reference("http://[::127.0.0.1.]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://[0:0:0:0:0:0:13.1.68.3]");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://[::d01:4403]"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "[::d01:4403]"
            BOOST_TEST_NOT(u.encoded_host() == "[::d01:4403]");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://[2001::1]:80");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://[2001::1]"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ftp:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("https:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("madeupscheme:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "madeupscheme:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "madeupscheme");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("file://example:1/");
            // Boost.URL does validate individual schemes
            // Ada and Node.js fail on "file" URLs with ports
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("file://example:test/");
            // Boost.URL does validate individual schemes
            // Ada and Node.js fail on "file" URLs with ports
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("file://example%/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("file://[example]/");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ftps:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ftps:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ftps");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("gopher:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "gopher:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "gopher");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ws:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("wss:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("data:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "data:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "data");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("javascript:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "javascript:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "javascript");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("mailto:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "mailto:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "mailto");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/foo/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ftp:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("https:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("madeupscheme:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "madeupscheme:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "madeupscheme");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ftps:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ftps:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ftps");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("gopher:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "gopher:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "gopher");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ws:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("wss:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("data:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "data:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "data");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("javascript:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "javascript:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "javascript");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("mailto:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "mailto:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "mailto");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/a/b/c");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/a/b/c"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/a/b/c");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("/a/ /c");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/a%2fc");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/a%2fc"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            // Boost.URL normalizes octets when resolving the URL
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/a%2Fc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/a%2Fc");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/a/%2f/c");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/a/%2f/c"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            // Boost.URL normalizes octets when resolving the URL
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/a/%2F/c");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/a/%2F/c");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("#β");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("data:text/html,test#test");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "data:text/html,test#test"
            BOOST_TEST_CSTR_EQ(u.scheme(), "data");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "text/html,test");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "test");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("tel:1234567890");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "tel:1234567890"
            BOOST_TEST_CSTR_EQ(u.scheme(), "tel");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "1234567890");
        }();
        // Based on https://felixfbecker.github.io/whatwg-url-custom-host-repro/
        []{
            result<url> base = parse_uri("http://example.org/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ssh://example.com/foo/bar.git");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ssh://example.com/foo/bar.git"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ssh");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar.git");
        }();
        // Based on http://trac.webkit.org/browser/trunk/LayoutTests/fast/url/file.html
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("file:c:\\foo\\bar.html");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("  File:c|////foo\\bar.html");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("C|/foo/bar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("/C|\\foo\\bar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("//C|/foo/bar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("//server/file");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://server/file"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "server");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/file");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("\\\\server\\file");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("/\\server/file");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///foo/bar.txt");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///foo/bar.txt"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar.txt");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///home/me");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///home/me"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/home/me");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("//");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("///");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("///test");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///test"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://test");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://test"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "test");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://localhost");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://localhost/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://localhost/test");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///test"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("test");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///tmp/mock/test"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/tmp/mock/test");
        }();
        []{
            result<url> base = parse_uri("file:///tmp/mock/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:test");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///tmp/mock/test"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/tmp/mock/test");
        }();
        // Based on http://trac.webkit.org/browser/trunk/LayoutTests/fast/url/script-tests/path.js
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/././foo");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/./.foo");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/.foo"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/.foo");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/.");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/./");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/bar/..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/bar/../");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/..bar");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo/..bar"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/..bar");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/bar/../ton");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo/ton"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/ton");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/bar/../ton/../../a");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/a"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/a");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/../../..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['foo', '..', '..', '..']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/../../../ton");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/ton"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['foo', '..', '..', '..', 'ton']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/ton");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/%2e");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("http://example.com/foo/%2e%2");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/%2e./%2e%2e/.%2e/%2e.bar");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/%2e.bar"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['foo', '%2e.', '%2e%2e', '.%2e', '%2e.bar']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/%2E.bar");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com////../..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com//"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/bar//../..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo/bar//..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo/bar/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/%20foo");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/%20foo"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/%20foo");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("http://example.com/foo%");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("http://example.com/foo%2");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("http://example.com/foo%2zbar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://example.com/foo%2Â©zbar");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo%41%7a");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo%41%7a"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            // Boost.URL normalizes octets when resolving the URL
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo%41%7A");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/fooAz");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://example.com/foo\t\x91" "%91");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/foo%00%51");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/foo%00%51"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            // Boost.URL normalizes octets when resolving the URL
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo%00%51");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo%00Q");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/(%28:%3A%29)");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/(%28:%3A%29)"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            // Boost.URL normalizes octets when resolving the URL
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/(%28:%3A%29)");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/((::))");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/%3A%3a%3C%3c");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/%3A%3a%3C%3c"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            // Boost.URL normalizes octets when resolving the URL
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/%3A%3A%3C%3C");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/::%3C%3C");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://example.com/foo\tbar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://example.com\\\\foo\\\\bar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/%7Ffp3%3Eju%3Dduvgw%3Dd");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/%7Ffp3%3Eju%3Dduvgw%3Dd"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            // Boost.URL normalizes octets when resolving the URL
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/%7Ffp3%3Eju%3Dduvgw%3Dd");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/%7Ffp3%3Eju=duvgw=d");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.com/@asdf%40");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com/@asdf%40"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            // Boost.URL normalizes octets when resolving the URL
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/@asdf%40");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/@asdf@");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://example.com/你好你好");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://example.com/‥/foo");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://example.com/\ufeff/foo");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://example.com/\u202e/foo/\u202d/bar");
            // BOOST_TEST_NOT(ref);
        }();
        // Based on http://trac.webkit.org/browser/trunk/LayoutTests/fast/url/script-tests/relative.js
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://www.google.com/foo?bar=baz#");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.google.com/foo?bar=baz#"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.google.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "bar=baz");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://www.google.com/foo?bar=baz# »");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("data:test# »");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://www.google.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.google.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.google.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://192.0x00A80001");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://192.168.0.1"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "192.168.0.1"
            BOOST_TEST_NOT(u.encoded_host() == "192.168.0.1");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://www/foo%2Ehtml");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www/foo%2Ehtml"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www");
            // Boost.URL normalizes octets when resolving the URL
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo%2Ehtml");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo.html");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://www/foo/%2E/html");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www/foo/html"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/html");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://user:pass@/");
            // Boost.URL and Node.js do not fail
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://%25DOMAIN:foobar@foodomain.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://%25DOMAIN:foobar@foodomain.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "%25DOMAIN");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foodomain.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http:\\\\www.google.com\\foo");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://foo:80/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://foo:81/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://foo:81/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "81");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("httpa://foo:80/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "httpa://foo:80/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "httpa");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "80");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://foo:-80/");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("https://foo:443/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "https://foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "https");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("https://foo:80/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "https://foo:80/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "https");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "80");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ftp://foo:21/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ftp://foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ftp");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ftp://foo:80/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ftp://foo:80/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ftp");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "80");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("gopher://foo:70/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "gopher://foo:70/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "gopher");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "70");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("gopher://foo:443/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "gopher://foo:443/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "gopher");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "443");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ws://foo:80/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ws://foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ws");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ws://foo:81/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ws://foo:81/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ws");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "81");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ws://foo:443/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ws://foo:443/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ws");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "443");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ws://foo:815/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ws://foo:815/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ws");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "815");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("wss://foo:80/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "wss://foo:80/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "wss");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "80");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("wss://foo:81/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "wss://foo:81/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "wss");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "81");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("wss://foo:443/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "wss://foo/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "wss");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("wss://foo:815/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "wss://foo:815/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "wss");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foo");
            BOOST_TEST_CSTR_EQ(u.port(), "815");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ftp:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("https:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("madeupscheme:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "madeupscheme:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "madeupscheme");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ftps:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ftps:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ftps");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("gopher:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "gopher:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "gopher");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ws:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("wss:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("data:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "data:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "data");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("javascript:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "javascript:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "javascript");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("mailto:/example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "mailto:/example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "mailto");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ftp:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("https:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("madeupscheme:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "madeupscheme:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "madeupscheme");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ftps:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ftps:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ftps");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("gopher:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "gopher:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "gopher");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ws:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("wss:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "example.com"
            BOOST_TEST_NOT(u.encoded_host() == "example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("data:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "data:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "data");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("javascript:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "javascript:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "javascript");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("mailto:example.com/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "mailto:example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "mailto");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "example.com/");
        }();
        // Based on http://trac.webkit.org/browser/trunk/LayoutTests/fast/url/segments-userinfo-vs-host.html
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "www.example.com"
            BOOST_TEST_NOT(u.encoded_host() == "www.example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:/@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "www.example.com"
            BOOST_TEST_NOT(u.encoded_host() == "www.example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:a:b@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "www.example.com"
            BOOST_TEST_NOT(u.encoded_host() == "www.example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:/a:b@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "www.example.com"
            BOOST_TEST_NOT(u.encoded_host() == "www.example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://a:b@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://a:b@www.example.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "a");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://@pple.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://pple.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "pple.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http::b@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "www.example.com"
            BOOST_TEST_NOT(u.encoded_host() == "www.example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:/:b@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "www.example.com"
            BOOST_TEST_NOT(u.encoded_host() == "www.example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://:b@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://:b@www.example.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http:/:@/www.example.com");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://user@/www.example.com");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http:@/www.example.com");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http:/@/www.example.com");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://@/www.example.com");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("https:@/www.example.com");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http:a:b@/www.example.com");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http:/a:b@/www.example.com");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://a:b@/www.example.com");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http::@/www.example.com");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:a:@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "www.example.com"
            BOOST_TEST_NOT(u.encoded_host() == "www.example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:/a:@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL does not convert paths into hostnames
            // Expected hostname in Ada: "www.example.com"
            BOOST_TEST_NOT(u.encoded_host() == "www.example.com");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://a:@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://a@www.example.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "a");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://www.@pple.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.@pple.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "www.");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "pple.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http:@:www.example.com");
            // Boost.URL does not fail on paths that resemble hosts
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http:/@:www.example.com");
            // Boost.URL does not fail on paths that resemble hosts
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://@:www.example.com");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://:@www.example.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        // Others
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/test.txt");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com/test.txt"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test.txt");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference(".");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("test.txt");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com/test.txt"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test.txt");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("./test.txt");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com/test.txt"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test.txt");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("../test.txt");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com/test.txt"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', 'test.txt']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test.txt");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("../aaa/test.txt");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com/aaa/test.txt"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', 'aaa', 'test.txt']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/aaa/test.txt");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("../../test.txt");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example.com/test.txt"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example.com");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', '..', 'test.txt']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test.txt");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("中/test.txt");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://www.example2.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example2.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example2.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("//www.example2.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://www.example2.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "www.example2.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:...");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///..."
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            // Boost.URL normalization does not treat "file:" differently
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/...");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://www.example.com/test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:a");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///a"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            // Boost.URL normalization does not treat "file:" differently
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/a");
        }();
        // Based on http://trac.webkit.org/browser/trunk/LayoutTests/fast/url/host.html
        // Basic canonicalization, uppercase should be converted to lowercase
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://ExAmPlE.CoM");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("http://example example.com");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("http://Goo%20 goo%7C|.com");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://[]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://[:]");
            BOOST_TEST_NOT(base && ref);
        }();
        // U+3000 is mapped to U+0020 (space) which is disallowed
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://GOO\xa0" "\u3000goo.com");
            // BOOST_TEST_NOT(ref);
        }();
        // Other types of space (no-break, zero-width, zero-width-no-break) are name-prepped away to nothing. U+200B, U+2060, and U+FEFF, are ignored
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://GOO\u200b\u2060\ufeffgoo.com");
            // BOOST_TEST_NOT(ref);
        }();
        // Leading and trailing C0 control or space
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("\x00" "\x1b" "\x04" "\x12" " http://example.com/\x1f" " \r ");
            // BOOST_TEST_NOT(ref);
        }();
        // Ideographic full stop (full-width period for Chinese, etc.) should be treated as a dot. U+3002 is mapped to U+002E (dot)
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://www.foo。bar.com");
            // BOOST_TEST_NOT(ref);
        }();
        // Invalid unicode characters should fail... U+FDD0 is disallowed; %ef%b7%90 is U+FDD0
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://\ufdd0zyx.com");
            // BOOST_TEST_NOT(ref);
        }();
        // This is the same as previous but escaped
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("http://%ef%b7%90zyx.com");
            BOOST_TEST(ref);
        }();
        // U+FFFD
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("https://�");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("https://%EF%BF%BD");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("https://x/�?�#�");
            // BOOST_TEST_NOT(ref);
        }();
        // Domain is ASCII, but a label is invalid IDNA
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid IDNA
            result<url> ref = parse_uri_reference("http://a.b.c.xn--pokxncvks");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid IDNA
            result<url> ref = parse_uri_reference("http://10.0.0.xn--pokxncvks");
            BOOST_TEST(ref);
        }();
        // IDNA labels should be matched case-insensitively
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid IDNA
            result<url> ref = parse_uri_reference("http://a.b.c.XN--pokxncvks");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid IDNA
            result<url> ref = parse_uri_reference("http://a.b.c.Xn--pokxncvks");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid IDNA
            result<url> ref = parse_uri_reference("http://10.0.0.XN--pokxncvks");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid IDNA
            result<url> ref = parse_uri_reference("http://10.0.0.xN--pokxncvks");
            BOOST_TEST(ref);
        }();
        // Test name prepping, fullwidth input should be converted to ASCII and NOT IDN-ized. This is 'Go' in fullwidth UTF-8/UTF-16.
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://Ｇｏ.com");
            // BOOST_TEST_NOT(ref);
        }();
        // URL spec forbids the following. https://www.w3.org/Bugs/Public/show_bug.cgi?id=24257
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://％４１.com");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("http://%ef%bc%85%ef%bc%94%ef%bc%91.com");
            BOOST_TEST(ref);
        }();
        // ...%00 in fullwidth should fail (also as escaped UTF-8 input)
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://％００.com");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("http://%ef%bc%85%ef%bc%90%ef%bc%90.com");
            BOOST_TEST(ref);
        }();
        // Basic IDN support, UTF-8 and UTF-16 input should be converted to IDN
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://你好你好");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("https://faß.ExAmPlE/");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("sc://faß.ExAmPlE/");
            // BOOST_TEST_NOT(ref);
        }();
        // Invalid escaped characters should fail and the percents should be escaped. https://www.w3.org/Bugs/Public/show_bug.cgi?id=24191
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("http://%zz%66%a.com");
            BOOST_TEST_NOT(ref);
        }();
        // If we get an invalid character that has been escaped.
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://%25");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://hello%00");
            BOOST_TEST(ref);
        }();
        // Escaped numbers should be treated like IP addresses if they are.
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://%30%78%63%30%2e%30%32%35%30.01");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://192.168.0.1"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "192.168.0.1"
            BOOST_TEST_NOT(u.encoded_host() == "192.168.0.1");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://%30%78%63%30%2e%30%32%35%30.01%2e");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://192.168.0.1"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "192.168.0.1"
            BOOST_TEST_NOT(u.encoded_host() == "192.168.0.1");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://192.168.0.257");
            BOOST_TEST(ref);
        }();
        // Invalid escaping in hosts causes failure
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("http://%3g%78%63%30%2e%30%32%35%30%2E.01");
            BOOST_TEST_NOT(ref);
        }();
        // A space in a host causes failure
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("http://192.168.0.1 hello");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("https://x x:12");
            BOOST_TEST_NOT(ref);
        }();
        // Fullwidth and escaped UTF-8 fullwidth should still be treated as IP
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://０Ｘｃ０．０２５０．０１");
            // BOOST_TEST_NOT(ref);
        }();
        // Domains with empty labels
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://./");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://./"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), ".");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://../");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://../"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "..");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        // Non-special domains with empty labels
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("h://.");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "h://."
            BOOST_TEST_CSTR_EQ(u.scheme(), "h");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), ".");
        }();
        // Broken IPv6
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://[www.google.com]/");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://[google.com]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://[::1.2.3.4x]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://[::1.2.3.]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://[::1.2.]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://[::.1.2]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://[::1.]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://[::.1]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://[::%31]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://%5B::1]");
            BOOST_TEST_NOT(base && ref);
        }();
        // Misc Unicode
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://foo:💩@example.com/bar");
            // BOOST_TEST_NOT(ref);
        }();
        // resolving a fragment against any scheme succeeds
        []{
            result<url> base = parse_uri("test:test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "test:test#"
            BOOST_TEST_CSTR_EQ(u.scheme(), "test");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "test");
        }();
        []{
            result<url> base = parse_uri("mailto:x@x.com");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#x");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "mailto:x@x.com#x"
            BOOST_TEST_CSTR_EQ(u.scheme(), "mailto");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "x@x.com");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "x");
        }();
        []{
            result<url> base = parse_uri("data:,");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#x");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "data:,#x"
            BOOST_TEST_CSTR_EQ(u.scheme(), "data");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), ",");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "x");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#x");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "about:blank#x"
            BOOST_TEST_CSTR_EQ(u.scheme(), "about");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "blank");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "x");
        }();
        []{
            result<url> base = parse_uri("test:test?test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "test:test?test#"
            BOOST_TEST_CSTR_EQ(u.scheme(), "test");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "test");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "test");
        }();
        // multiple @ in authority state
        []{
            result<url> base = parse_uri("http://doesnotmatter/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support multiple "@" in the authority
            // Ada and Node.js would consider the last "@" as the separator
            result<url> ref = parse_uri_reference("https://@test@test@example:800/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://doesnotmatter/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support multiple "@" in the authority
            // Ada and Node.js would consider the last "@" as the separator
            result<url> ref = parse_uri_reference("https://@@@example");
            BOOST_TEST_NOT(ref);
        }();
        // non-az-09 characters
        []{
            result<url> base = parse_uri("http://doesnotmatter/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid authority char "`"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://`{}:`{}@h/`{}?`{}");
            BOOST_TEST_NOT(ref);
        }();
        // byte is ' and url is special
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://host/?'");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://host/?%27"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "host");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "'");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("notspecial://host/?'");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "notspecial://host/?'"
            BOOST_TEST_CSTR_EQ(u.scheme(), "notspecial");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "host");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "'");
        }();
        // Credentials in base
        []{
            result<url> base = parse_uri("http://user@example.org/smth");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/some/path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://user@example.org/some/path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "user");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/some/path");
        }();
        []{
            result<url> base = parse_uri("http://user:pass@example.org:21/smth");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not fail on empty references
            result<url> ref = parse_uri_reference("");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://user:pass@example.org:21/smth");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/some/path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://user:pass@example.org:21/some/path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "user");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.port(), "21");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/some/path");
        }();
        // a set of tests designed by zcorpan for relative URLs with unknown schemes
        []{
            result<url> base = parse_uri("sc:/pa/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:/pa/i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/pa/i");
        }();
        []{
            result<url> base = parse_uri("sc://ho/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc://ho/i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "ho");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/i");
        }();
        []{
            result<url> base = parse_uri("sc:///pa/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:///pa/i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/pa/i");
        }();
        []{
            result<url> base = parse_uri("sc:/pa/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("../i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:/i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/i");
        }();
        []{
            result<url> base = parse_uri("sc://ho/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("../i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc://ho/i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "ho");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', 'i']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/i");
        }();
        []{
            result<url> base = parse_uri("sc:///pa/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("../i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:///i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/i");
        }();
        []{
            result<url> base = parse_uri("sc:/pa/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:/i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/i");
        }();
        []{
            result<url> base = parse_uri("sc://ho/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc://ho/i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "ho");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/i");
        }();
        []{
            result<url> base = parse_uri("sc:///pa/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:///i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/i");
        }();
        []{
            result<url> base = parse_uri("sc:/pa/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("?i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:/pa/pa?i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/pa/pa");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "i");
        }();
        []{
            result<url> base = parse_uri("sc://ho/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("?i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc://ho/pa?i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "ho");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/pa");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "i");
        }();
        []{
            result<url> base = parse_uri("sc:///pa/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("?i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:///pa/pa?i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/pa/pa");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "i");
        }();
        []{
            result<url> base = parse_uri("sc:sd");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:sd#i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "sd");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "i");
        }();
        []{
            result<url> base = parse_uri("sc:sd/sd");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:sd/sd#i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "sd/sd");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "i");
        }();
        []{
            result<url> base = parse_uri("sc:/pa/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:/pa/pa#i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/pa/pa");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "i");
        }();
        []{
            result<url> base = parse_uri("sc://ho/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc://ho/pa#i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "ho");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/pa");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "i");
        }();
        []{
            result<url> base = parse_uri("sc:///pa/pa");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#i");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:///pa/pa#i"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/pa/pa");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "i");
        }();
        // make sure that relative URL logic works on known typically non-relative schemes too
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("about:/../");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "about:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "about");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', '']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("data:/../");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "data:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "data");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', '']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("javascript:/../");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "javascript:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "javascript");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', '']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("mailto:/../");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "mailto:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "mailto");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', '']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        // unknown schemes and their hosts
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("sc://ñ.test/");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("sc://%/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("sc://@/");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support multiple "@" in the authority
            // Ada and Node.js would consider the last "@" as the separator
            result<url> ref = parse_uri_reference("sc://te@s:t@/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("sc://:/");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("sc://:12/");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            // Boost.URL does not support unicode
            // result<url> base = parse_uri("sc://ñ");
            // BOOST_TEST_NOT(base);
        }();
        // unknown schemes and backslashes
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("sc:\\../");
            BOOST_TEST_NOT(ref);
        }();
        // unknown scheme with path looking like a password
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("sc::a@example.net");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc::a@example.net"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), ":a@example.net");
        }();
        // unknown scheme with bogus percent-encoding
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("wow:%NBD");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("wow:%1G");
            BOOST_TEST_NOT(ref);
        }();
        // unknown scheme with non-URL characters
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("wow:\uffff");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://example.com/\ud800\U000107fe\udfff\ufdd0\ufdcf\ufdefﷰ\ufffe\uffff?\ud800\U000107fe\udfff\ufdd0\ufdcf\ufdefﷰ\ufffe\uffff");
            // BOOST_TEST_NOT(ref);
        }();
        // Forbidden host code points
        []{
            result<url> base = parse_uri("about:blank");
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("sc://a\x00" "b/");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("sc://a b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char "<"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("sc://a<b");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char ">"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("sc://a>b");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("sc://a[b/");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("sc://a\\b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("sc://a]b/");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char "^"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("sc://a^b");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char "|"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("sc://a|b/");
            BOOST_TEST_NOT(ref);
        }();
        // Forbidden host codepoints: tabs and newlines are removed during preprocessing
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("foo://ho\tst/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("foo://ho\nst/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("foo://ho\rst/");
            BOOST_TEST_NOT(ref);
        }();
        // Forbidden domain code-points
        []{
            result<url> base = parse_uri("about:blank");
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("http://a\x00" "b/");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x01
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x01" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x02
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x02" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x03
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x03" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x04
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x04" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x05
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x05" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x06
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x06" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://a\x07" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://a\x08" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://a\x0b" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://a\x0c" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x0e
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x0e" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x0f
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x0f" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x10
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x10" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x11
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x11" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x12
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x12" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x13
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x13" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x14
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x14" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x15
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x15" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x16
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x16" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x17
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x17" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x18
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x18" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x19
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x19" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x1a
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x1a" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x1b
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x1b" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x1c
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x1c" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x1d
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x1d" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x1e
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x1e" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x1f
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x1f" "b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("http://a b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("http://a%b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char "<"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a<b");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char ">"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a>b");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://a[b/");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://a]b/");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char "^"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a^b");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char "|"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a|b/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char \x7f
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://a\x7f" "b/");
            BOOST_TEST_NOT(ref);
        }();
        // Forbidden domain codepoints: tabs and newlines are removed during preprocessing
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://ho\tst/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://ho\nst/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://ho\rst/");
            BOOST_TEST_NOT(ref);
        }();
        // Encoded forbidden domain codepoints in special URLs
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%00st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%01st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%02st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%03st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%04st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%05st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%06st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%07st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%08st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%09st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%0Ast/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%0Bst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%0Cst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%0Dst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%0Est/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%0Fst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%10st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%11st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%12st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%13st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%14st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%15st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%16st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%17st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%18st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%19st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%1Ast/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%1Bst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%1Cst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%1Dst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%1Est/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%1Fst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%20st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%23st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%25st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%2Fst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%3Ast/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%3Cst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%3Est/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%3Fst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%40st/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%5Bst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%5Cst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%5Dst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%7Cst/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("http://ho%7Fst/");
            BOOST_TEST(ref);
        }();
        // Allowed host/domain code points
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid authority char """
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://!\"$&\'()*+,-.;=_`{}~/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("sc://\x01" "\x02" "\x03" "\x04" "\x05" "\x06" "\x07" "\x08" "\x0b" "\x0c" "\x0e" "\x0f" "\x10" "\x11" "\x12" "\x13" "\x14" "\x15" "\x16" "\x17" "\x18" "\x19" "\x1a" "\x1b" "\x1c" "\x1d" "\x1e" "\x1f" "\x7f" "!\"$%&\'()*+,-.;=_`{}~/");
            BOOST_TEST_NOT(ref);
        }();
        // Hosts and percent-encoding
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("ftp://example.com%80/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("ftp://example.com%A0/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("https://example.com%80/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("https://example.com%A0/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("ftp://%e2%98%83");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("https://%e2%98%83");
            BOOST_TEST(ref);
        }();
        // tests from jsdom/whatwg-url designed for code coverage
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://127.0.0.1:10100/relative_import.html");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://127.0.0.1:10100/relative_import.html"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.port(), "10100");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/relative_import.html");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://facebook.com/?foo=%7B%22abc%22");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://facebook.com/?foo=%7B%22abc%22"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "facebook.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "foo=%7B%22abc%22");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("https://localhost:3000/jqueryui@1.2.3");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "https://localhost:3000/jqueryui@1.2.3"
            BOOST_TEST_CSTR_EQ(u.scheme(), "https");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "localhost");
            BOOST_TEST_CSTR_EQ(u.port(), "3000");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/jqueryui@1.2.3");
        }();
        // tab/LF/CR
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("h\tt\nt\rp://h\to\ns\rt:9\t0\n0\r0/p\ta\nt\rh?q\tu\ne\rry#f\tr\na\rg");
            BOOST_TEST_NOT(ref);
        }();
        // Stringification of URL.searchParams
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("?a=b&c=d");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/foo/bar?a=b&c=d"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "a=b&c=d");
        }();
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("??a=b&c=d");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/foo/bar??a=b&c=d"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "?a=b&c=d");
        }();
        // Scheme only
        []{
            result<url> base = parse_uri("http://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http:");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/foo/bar"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar");
        }();
        []{
            result<url> base = parse_uri("https://example.org/foo/bar");
            result<url> ref = parse_uri_reference("http:");
            // Boost.URL does validate individual schemes
            // Ada and Node.js fail on "http:" with no authority
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("https://example.org/foo/bar");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("sc:");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
        }();
        // Percent encoding of fragments
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("http://foo.bar/baz?qux#foo\x08" "bar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid fragment char """
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://foo.bar/baz?qux#foo\"bar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid fragment char "<"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://foo.bar/baz?qux#foo<bar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid fragment char ">"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://foo.bar/baz?qux#foo>bar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid fragment char "`"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://foo.bar/baz?qux#foo`bar");
            BOOST_TEST_NOT(ref);
        }();
        // IPv4 parsing (via https://github.com/nodejs/node/pull/10317)
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://1.2.3.4/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://1.2.3.4/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://1.2.3.4./");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://1.2.3.4/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://192.168.257");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://192.168.1.1"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "192.168.1.1"
            BOOST_TEST_NOT(u.encoded_host() == "192.168.1.1");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://192.168.257.");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://192.168.1.1"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "192.168.1.1"
            BOOST_TEST_NOT(u.encoded_host() == "192.168.1.1");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://192.168.257.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://192.168.257.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "192.168.257.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://256");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://0.0.1.0"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "0.0.1.0"
            BOOST_TEST_NOT(u.encoded_host() == "0.0.1.0");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://256.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://256.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "256.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://999999999");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://59.154.201.255"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "59.154.201.255"
            BOOST_TEST_NOT(u.encoded_host() == "59.154.201.255");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://999999999.");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://59.154.201.255"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "59.154.201.255"
            BOOST_TEST_NOT(u.encoded_host() == "59.154.201.255");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://999999999.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://999999999.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "999999999.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            result<url> ref = parse_uri_reference("http://10000000000");
            // Boost.URL does not fail on port overflow
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://10000000000.com");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://10000000000.com"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "10000000000.com");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://4294967295");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://255.255.255.255"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "255.255.255.255"
            BOOST_TEST_NOT(u.encoded_host() == "255.255.255.255");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL does support invalid IDNA
            result<url> ref = parse_uri_reference("http://4294967296");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://0xffffffff");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://255.255.255.255"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "255.255.255.255"
            BOOST_TEST_NOT(u.encoded_host() == "255.255.255.255");
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://0xffffffff1");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://256.256.256.256");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("https://0x.0x.0");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "https://0.0.0.0"
            BOOST_TEST_CSTR_EQ(u.scheme(), "https");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "0.0.0.0"
            BOOST_TEST_NOT(u.encoded_host() == "0.0.0.0");
        }();
        // More IPv4 parsing (via https://github.com/jsdom/whatwg-url/issues/92)
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("https://0x100000000/test");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("https://256.0.0.1/test");
            BOOST_TEST(ref);
        }();
        // file URLs containing percent-encoded Windows drive letters (shouldn't work)
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///C%3A/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///C%3A/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            // Boost.URL normalizes octets when resolving the URL
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C%3A/");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C:/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///C%7C/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///C%7C/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C%7C/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("file://%43%3A");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("file://%43%7C");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char "|"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("file://%43|");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("file://C%7C");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("file://%43%7C/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("https://%43%7C/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support invalid authority char "|"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("asdf://%43|/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does support invalid encoded chars
            result<url> ref = parse_uri_reference("asdf://%43%7C/");
            BOOST_TEST(ref);
        }();
        // file URLs relative to other file URLs (via https://github.com/jsdom/whatwg-url/pull/60)
        []{
            result<url> base = parse_uri("file:///C:/Users/Domenic/Dropbox/GitHub/tmpvar/jsdom/test/level2/html/files/anchor.html");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("pix/submit.gif");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///C:/Users/Domenic/Dropbox/GitHub/tmpvar/jsdom/test/level2/html/files/pix/submit.gif"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C:/Users/Domenic/Dropbox/GitHub/tmpvar/jsdom/test/level2/html/files/pix/submit.gif");
        }();
        []{
            result<url> base = parse_uri("file:///C:/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:///C:/");
        }();
        []{
            result<url> base = parse_uri("file:///");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        // More file URL tests by zcorpan and annevk
        []{
            result<url> base = parse_uri("file:///C:/a/b");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:///C:/");
        }();
        []{
            result<url> base = parse_uri("file://h/C:/a/b");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file://h/C:/");
        }();
        []{
            result<url> base = parse_uri("file://h/a/b");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://h/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "h");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("file:///C:/a/b");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("//d:");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:///d:");
        }();
        []{
            result<url> base = parse_uri("file:///C:/a/b");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("//d:/..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:///d:/");
        }();
        []{
            result<url> base = parse_uri("file:///ab:/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("file:///1:/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("..");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("file:///test?test#test");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not fail on empty references
            result<url> ref = parse_uri_reference("");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("file:///test?test#test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///test?test"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "test");
        }();
        []{
            result<url> base = parse_uri("file:///test?test#test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("?x");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///test?x"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "x");
        }();
        []{
            result<url> base = parse_uri("file:///test?test#test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:?x");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///test?x"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "x");
        }();
        []{
            result<url> base = parse_uri("file:///test?test#test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#x");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///test?test#x"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "test");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "x");
        }();
        []{
            result<url> base = parse_uri("file:///test?test#test");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:#x");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///test?test#x"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "test");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "x");
        }();
        // File URLs and many (back)slashes
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("file:\\\\//");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("file:\\\\\\\\");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("file:\\\\\\\\?fox");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("file:\\\\\\\\#guppy");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://spider///");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://spider///"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "spider");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "///");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("file:\\\\localhost//");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///localhost//cat");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///localhost//cat"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/localhost//cat");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("file://\\/localhost//cat");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://localhost//a//../..//");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://///"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "///");
        }();
        []{
            result<url> base = parse_uri("file:///elephant");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/////mouse");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://///mouse"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "///mouse");
        }();
        []{
            result<url> base = parse_uri("file://lion/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("\\//pig");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://lion/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("\\/localhost//pig");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://lion/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("//localhost//pig");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:////pig");
        }();
        []{
            result<url> base = parse_uri("file://lion/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/..//localhost//pig");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file://lion//localhost//pig");
        }();
        []{
            result<url> base = parse_uri("file://ape/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:///");
        }();
        // File URLs with non-empty hosts
        []{
            result<url> base = parse_uri("file://tea/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/rooibos");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://tea/rooibos"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "tea");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/rooibos");
        }();
        []{
            result<url> base = parse_uri("file://tea/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/?chai");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://tea/?chai"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "tea");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "chai");
        }();
        // Windows drive letter handling with the 'file:' base URL
        []{
            result<url> base = parse_uri("file://host/dir/file");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("C|");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://host/D:/dir1/dir2/file");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("C|");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://host/dir/file");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("C|#");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://host/dir/file");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("C|?");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://host/dir/file");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("C|/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://host/dir/file");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("C|\n/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://host/dir/file");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("C|\\");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://host/dir/file");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("C");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://host/dir/C"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "host");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/dir/C");
        }();
        []{
            result<url> base = parse_uri("file://host/dir/file");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("C|a");
            BOOST_TEST_NOT(ref);
        }();
        // Windows drive letter quirk in the file slash state
        []{
            result<url> base = parse_uri("file:///c:/baz/qux");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/c:/foo/bar");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///c:/foo/bar"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/c:/foo/bar");
        }();
        []{
            result<url> base = parse_uri("file:///c:/baz/qux");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("/c|/foo/bar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file:///c:/baz/qux");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("file:\\c:\\foo\\bar");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://host/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/c:/foo/bar");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://host/c:/foo/bar"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "host");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/c:/foo/bar");
        }();
        // Do not drop the host in the presence of a drive letter
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://example.net/C:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://example.net/C:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.net");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C:/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://1.2.3.4/C:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://1.2.3.4/C:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C:/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://[1::8]/C:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://[1::8]/C:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C:/");
        }();
        // Copy the host from the base URL in the following cases
        []{
            result<url> base = parse_uri("file://host/");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("C|/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("file://host/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/C:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://host/C:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "host");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C:/");
        }();
        []{
            result<url> base = parse_uri("file://host/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:C:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://host/C:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "host");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C:/");
        }();
        []{
            result<url> base = parse_uri("file://host/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:/C:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://host/C:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "host");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C:/");
        }();
        // Copy the empty host from the input in the following cases
        []{
            result<url> base = parse_uri("file://host/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("//C:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:///C:/");
        }();
        []{
            result<url> base = parse_uri("file://host/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://C:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:///C:/");
        }();
        []{
            result<url> base = parse_uri("file://host/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("///C:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///C:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C:/");
        }();
        []{
            result<url> base = parse_uri("file://host/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///C:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///C:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/C:/");
        }();
        // Windows drive letter quirk (no host)
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support "|" in paths
            result<url> ref = parse_uri_reference("file:/C|/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid authority char "|"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("file://C|/");
            BOOST_TEST_NOT(ref);
        }();
        // file URLs without base URL by Rimas Misevičius
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:?q=v");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:///?q=v");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:#frag");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:///#frag");
        }();
        // file: drive letter cases from https://crbug.com/1078698
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///Y:");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///Y:"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/Y:");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///Y:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///Y:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/Y:/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///./Y");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///Y"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/Y");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///./Y:");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///Y:"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/Y:");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("\\\\\\.\\Y:");
            BOOST_TEST_NOT(ref);
        }();
        // file: drive letter cases from https://crbug.com/1078698 but lowercased
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///y:");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///y:"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/y:");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///y:/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///y:/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/y:/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///./y");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///y"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/y");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///./y:");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///y:"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/y:");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("\\\\\\.\\y:");
            BOOST_TEST_NOT(ref);
        }();
        // Additional file URL tests for (https://github.com/whatwg/url/issues/405)
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://localhost//a//../..//foo");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://///foo"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "///foo");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file://localhost////foo");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://////foo"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "////foo");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:////foo");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:////foo"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//foo");
        }();
        []{
            result<url> base = parse_uri("file:///");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///one/two");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///one/two"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/one/two");
        }();
        []{
            result<url> base = parse_uri("file:///");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:////one/two");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:////one/two"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//one/two");
        }();
        []{
            result<url> base = parse_uri("file:///");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("//one/two");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file://one/two"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "one");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/two");
        }();
        []{
            result<url> base = parse_uri("file:///");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("///one/two");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///one/two"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/one/two");
        }();
        []{
            result<url> base = parse_uri("file:///");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("////one/two");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:////one/two"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//one/two");
        }();
        []{
            result<url> base = parse_uri("file:////");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:///.//");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:////"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//");
        }();
        // File URL tests for https://github.com/whatwg/url/issues/549
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:.//p");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:////p");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("file:/.//p");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no exceptional logic for "file:"
            BOOST_TEST_NOT(u.buffer() == "file:////p");
        }();
        // IPv6 tests
        []{
            result<url> base = parse_uri("http://example.net/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://[1:0::]");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://[1::]"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "[1::]"
            BOOST_TEST_NOT(u.encoded_host() == "[1::]");
        }();
        []{
            result<url> base = parse_uri("http://example.net/");
            result<url> ref = parse_uri_reference("http://[0:1:2:3:4:5:6:7:8]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("https://[0::0::0]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("https://[0:.0]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("https://[0:0:]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("https://[0:1:2:3:4:5:6:7.0.0.0.1]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("https://[0:1.00.0.0.0]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("https://[0:1.290.0.0.0]");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("https://[0:1.23.23]");
            BOOST_TEST_NOT(base && ref);
        }();
        // Empty host
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://?");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://#");
            // Boost.URL does not fail on empty hostname
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        // Port overflow (2^32 + 81)
        []{
            result<url> base = parse_uri("http://example.org/");
            result<url> ref = parse_uri_reference("http://f:4294967377/c");
            // Boost.URL does not fail on port overflow
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        // Port overflow (2^64 + 81)
        []{
            result<url> base = parse_uri("http://example.org/");
            result<url> ref = parse_uri_reference("http://f:18446744073709551697/c");
            // Boost.URL does not fail on port overflow
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        // Port overflow (2^128 + 81)
        []{
            result<url> base = parse_uri("http://example.org/");
            result<url> ref = parse_uri_reference("http://f:340282366920938463463374607431768211537/c");
            // Boost.URL does not fail on port overflow
            BOOST_TEST(base);
            BOOST_TEST(ref);
        }();
        // Non-special-URL path tests
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("sc://ñ");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("sc://ñ?x");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("sc://ñ#x");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            // Boost.URL does not support unicode
            // result<url> base = parse_uri("sc://ñ");
            // BOOST_TEST_NOT(base);
        }();
        []{
            // Boost.URL does not support unicode
            // result<url> base = parse_uri("sc://ñ");
            // BOOST_TEST_NOT(base);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("sc://?");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc://?"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("sc://#");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc://#"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
        }();
        []{
            result<url> base = parse_uri("sc://x/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("///");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:///"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("sc://x/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("////");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:////"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//");
        }();
        []{
            result<url> base = parse_uri("sc://x/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("////x/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "sc:////x/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "sc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//x/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("tftp://foobar.com/someconfig;mode=netascii");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "tftp://foobar.com/someconfig;mode=netascii"
            BOOST_TEST_CSTR_EQ(u.scheme(), "tftp");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foobar.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/someconfig;mode=netascii");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("telnet://user:pass@foobar.com:23/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "telnet://user:pass@foobar.com:23/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "telnet");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "user");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "foobar.com");
            BOOST_TEST_CSTR_EQ(u.port(), "23");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ut2004://10.10.10.10:7777/Index.ut2");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ut2004://10.10.10.10:7777/Index.ut2"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ut2004");
            BOOST_TEST_CSTR_EQ(u.port(), "7777");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/Index.ut2");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("redis://foo:bar@somehost:6379/0?baz=bam&qux=baz");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "redis://foo:bar@somehost:6379/0?baz=bam&qux=baz"
            BOOST_TEST_CSTR_EQ(u.scheme(), "redis");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "somehost");
            BOOST_TEST_CSTR_EQ(u.port(), "6379");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/0");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "baz=bam&qux=baz");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("rsync://foo@host:911/sup");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "rsync://foo@host:911/sup"
            BOOST_TEST_CSTR_EQ(u.scheme(), "rsync");
            BOOST_TEST_CSTR_EQ(u.encoded_user(), "foo");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "host");
            BOOST_TEST_CSTR_EQ(u.port(), "911");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/sup");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("git://github.com/foo/bar.git");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "git://github.com/foo/bar.git"
            BOOST_TEST_CSTR_EQ(u.scheme(), "git");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "github.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar.git");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("irc://myserver.com:6999/channel?passwd");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "irc://myserver.com:6999/channel?passwd"
            BOOST_TEST_CSTR_EQ(u.scheme(), "irc");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "myserver.com");
            BOOST_TEST_CSTR_EQ(u.port(), "6999");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/channel");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "passwd");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("dns://fw.example.org:9999/foo.bar.org?type=TXT");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "dns://fw.example.org:9999/foo.bar.org?type=TXT"
            BOOST_TEST_CSTR_EQ(u.scheme(), "dns");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "fw.example.org");
            BOOST_TEST_CSTR_EQ(u.port(), "9999");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo.bar.org");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "type=TXT");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("ldap://localhost:389/ou=People,o=JNDITutorial");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "ldap://localhost:389/ou=People,o=JNDITutorial"
            BOOST_TEST_CSTR_EQ(u.scheme(), "ldap");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "localhost");
            BOOST_TEST_CSTR_EQ(u.port(), "389");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/ou=People,o=JNDITutorial");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("git+https://github.com/foo/bar");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "git+https://github.com/foo/bar"
            BOOST_TEST_CSTR_EQ(u.scheme(), "git+https");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "github.com");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/foo/bar");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("urn:ietf:rfc:2648");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "urn:ietf:rfc:2648"
            BOOST_TEST_CSTR_EQ(u.scheme(), "urn");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "ietf:rfc:2648");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("tag:joe@example.org,2001:foo/bar");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "tag:joe@example.org,2001:foo/bar"
            BOOST_TEST_CSTR_EQ(u.scheme(), "tag");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "joe@example.org,2001:foo/bar");
        }();
        // Serialize /. in path
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-spec:/.//");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            // Boost.URL will not let "//" path become the authority separator
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "//");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-spec:/..//");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', '', '']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "//");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-spec:/a/..//");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-spec:/.//path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            // Boost.URL will not let "//" path become the authority separator
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "//path");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-spec:/..//path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', '', 'path']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "//path");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-spec:/a/..//path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//path");
        }();
        []{
            result<url> base = parse_uri("non-spec:/p");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/.//path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            // Boost.URL will not let "//" path become the authority separator
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "//path");
        }();
        []{
            result<url> base = parse_uri("non-spec:/p");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("/..//path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', '', 'path']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "//path");
        }();
        []{
            result<url> base = parse_uri("non-spec:/p");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("..//path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', '', 'path']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "//path");
        }();
        []{
            result<url> base = parse_uri("non-spec:/p");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("a/..//path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "//path");
        }();
        []{
            result<url> base = parse_uri("non-spec:/..//p");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not fail on empty references
            result<url> ref = parse_uri_reference("");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("non-spec:/..//p");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/.//path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            // Boost.URL normalization does not remove ".." segments above root
            // Resolved path has segments ['..', '', 'path']
            // Removing ".." segments would make Errata 4547 innocuous
            // see https://www.rfc-editor.org/errata/eid4547
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "//path");
        }();
        // Do not serialize /. in path
        []{
            result<url> base = parse_uri("non-spec:/.//p");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("../path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-spec:/path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-spec");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/path");
        }();
        // percent encoded hosts in non-special-URLs
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("non-special://%E2%80%A0/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-special://H%4fSt/path");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-special://H%4fSt/path"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-special");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "host");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/path");
        }();
        // IPv6 in non-special-URLs
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-special://[1:2:0:0:5:0:0:0]/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-special://[1:2:0:0:5::]/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-special");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "[1:2:0:0:5::]"
            BOOST_TEST_NOT(u.encoded_host() == "[1:2:0:0:5::]");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-special://[1:2:0:0:0:0:0:3]/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-special://[1:2::3]/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-special");
            // Boost.URL does not modify IP addresses
            // Expected hostname in Ada: "[1:2::3]"
            BOOST_TEST_NOT(u.encoded_host() == "[1:2::3]");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("non-special://[1:2::3]:80/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "non-special://[1:2::3]:80/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "non-special");
            BOOST_TEST_CSTR_EQ(u.port(), "80");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("non-special://[:80/");
            BOOST_TEST_NOT(base && ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("blob:https://example.com:443/");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "blob:https://example.com:443/"
            BOOST_TEST_CSTR_EQ(u.scheme(), "blob");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "https://example.com:443/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("blob:d3958f5c-0777-0845-9dcf-2cb28783acaf");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "blob:d3958f5c-0777-0845-9dcf-2cb28783acaf"
            BOOST_TEST_CSTR_EQ(u.scheme(), "blob");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "d3958f5c-0777-0845-9dcf-2cb28783acaf");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("blob:");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "blob:"
            BOOST_TEST_CSTR_EQ(u.scheme(), "blob");
        }();
        // Invalid IPv4 radix digits
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://0x7f.0.0.0x7g");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://0x7f.0.0.0x7g"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "0x7f.0.0.0x7g");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://0X7F.0.0.0X7G");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://0x7f.0.0.0x7g"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "0x7f.0.0.0x7g");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        // Invalid IPv4 portion of IPv6 address
        []{
            result<url> base = parse_uri("about:blank");
            result<url> ref = parse_uri_reference("http://[::127.0.0.0.1]");
            BOOST_TEST_NOT(base && ref);
        }();
        // Uncompressed IPv6 addresses with 0
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://[0:1:0:1:0:1:0:1]");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://[0:1:0:1:0:1:0:1]"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://[1:0:1:0:1:0:1:0]");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://[1:0:1:0:1:0:1:0]"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            // Boost.URL does not create absolute paths
            // BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
        }();
        // Percent-encoded query and fragment
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid query char """
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://example.org/test?\"");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.org/test?#");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/test?#"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid query char "<"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://example.org/test?<");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid query char ">"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("http://example.org/test?>");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://example.org/test?⌣");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("http://example.org/test?%23%23");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "http://example.org/test?%23%23"
            BOOST_TEST_CSTR_EQ(u.scheme(), "http");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test");
            BOOST_TEST_CSTR_EQ(u.encoded_query(), "%23%23");
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("http://example.org/test?%GH");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("http://example.org/test?a#%EF");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("http://example.org/test?a#%GH");
            BOOST_TEST_NOT(ref);
        }();
        // URLs that require a non-about:blank base. (Also serve as invalid base tests.)
        // Bases that don't fail to parse but fail to be bases
        // Other base URL tests, that must succeed
        []{
            result<url> base = parse_uri("a:/");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("test-a-colon-slash.html");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "a:/test-a-colon-slash.html"
            BOOST_TEST_CSTR_EQ(u.scheme(), "a");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test-a-colon-slash.html");
        }();
        []{
            result<url> base = parse_uri("a://");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("test-a-colon-slash-slash.html");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "a:///test-a-colon-slash-slash.html"
            BOOST_TEST_CSTR_EQ(u.scheme(), "a");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test-a-colon-slash-slash.html");
        }();
        []{
            result<url> base = parse_uri("a:/b");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("test-a-colon-slash-b.html");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "a:/test-a-colon-slash-b.html"
            BOOST_TEST_CSTR_EQ(u.scheme(), "a");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test-a-colon-slash-b.html");
        }();
        []{
            result<url> base = parse_uri("a://b");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("test-a-colon-slash-slash-b.html");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "a://b/test-a-colon-slash-slash-b.html"
            BOOST_TEST_CSTR_EQ(u.scheme(), "a");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "b");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/test-a-colon-slash-slash-b.html");
        }();
        // Null code point in fragment
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("http://example.org/test?a#b\x00" "c");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("non-spec://example.org/test?a#b\x00" "c");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("non-spec:/test?a#b\x00" "c");
            // BOOST_TEST_NOT(ref);
        }();
        // First scheme char - not allowed: https://github.com/whatwg/url/issues/464
        []{
            result<url> base = parse_uri("file:///some/dir/bar.html");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("10.0.0.7:8080/foo.html");
            // Boost.URL requires segment-nz-nc
            // A URL should have a scheme or the first path segment
            // cannot contain a colon (":")
            BOOST_TEST_NOT(ref);
        }();
        // Subsequent scheme chars - not allowed
        []{
            result<url> base = parse_uri("file:///some/dir/bar.html");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("a!@$*=/foo.html");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "file:///some/dir/a!@$*=/foo.html"
            BOOST_TEST_CSTR_EQ(u.scheme(), "file");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/some/dir/a!@$*=/foo.html");
        }();
        // First and subsequent scheme chars - allowed
        []{
            result<url> base = parse_uri("http://example.com/dir/file");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("a1234567890-+.:foo/bar");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "a1234567890-+.:foo/bar"
            BOOST_TEST_CSTR_EQ(u.scheme(), "a1234567890-+.");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "foo/bar");
        }();
        // IDNA ignored code points in file URLs hosts
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("file://a\xad" "b/p");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("file://a%C2%ADb/p");
            BOOST_TEST(ref);
        }();
        // IDNA hostnames which get mapped to 'localhost'
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("file://loC𝐀𝐋𝐇𝐨𝐬𝐭/usr/bin");
            // BOOST_TEST_NOT(ref);
        }();
        // Empty host after the domain to ASCII
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("file://\xad" "/p");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("file://%C2%AD/p");
            BOOST_TEST(ref);
        }();
        // https://bugzilla.mozilla.org/show_bug.cgi?id=1647058
        []{
            result<url> base = parse_uri("https://example.org/##link");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("#link");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "https://example.org/#link"
            BOOST_TEST_CSTR_EQ(u.scheme(), "https");
            BOOST_TEST_CSTR_EQ(u.encoded_host(), "example.org");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "/");
            BOOST_TEST_CSTR_EQ(u.encoded_fragment(), "link");
        }();
        // UTF-8 percent-encode of C0 control percent-encode set and supersets
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("non-special:cannot-be-a-base-url-\x00" "\x01" "\x1f" "\x1e" "~\x7f" "\x80" "");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid path char "{"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("https://www.example.com/path{\x7f" "path.html?query'\x7f" "=query#fragment<\x7f" "fragment");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("http://example.org");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid query char "\x7f"
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("https://user:pass[\x7f" "@foo/bar");
            BOOST_TEST_NOT(ref);
        }();
        // Tests for the distinct percent-encode sets
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("foo:// !\"$%&\'()*+,-.;<=>@[\\]^_`{|}~@host/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("wss:// !\"$%&\'()*+,-.;<=>@[]^_`{|}~@host/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("foo://joe: !\"$%&\'()*+,-.:;<=>@[\\]^_`{|}~@host/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support whitespace in input
            result<url> ref = parse_uri_reference("wss://joe: !\"$%&\'()*+,-.:;<=>@[]^_`{|}~@host/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support incomplete octets
            result<url> ref = parse_uri_reference("foo://!\"$%&\'()*+,-.;=_`{}~/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid authority char """
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("wss://!\"$&\'()*+,-.;=_`{}~/");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("foo://host/ !\"$%&\'()*+,-./:;<=>@[\\]^_`{|}~");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("wss://host/ !\"$%&\'()*+,-./:;<=>@[\\]^_`{|}~");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("foo://host/dir/? !\"$%&\'()*+,-./:;<=>?@[\\]^_`{|}~");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("wss://host/dir/? !\"$%&\'()*+,-./:;<=>?@[\\]^_`{|}~");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("foo://host/dir/# !\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support escape chars in input
            result<url> ref = parse_uri_reference("wss://host/dir/# !\"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~");
            BOOST_TEST_NOT(ref);
        }();
        // Ensure that input schemes are not ignored when resolving non-special URLs
        []{
            result<url> base = parse_uri("abc://host/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("abc:rootless");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no special logic for schemes
            BOOST_TEST_NOT(u.buffer() == "abc:rootless");
        }();
        []{
            result<url> base = parse_uri("abc:/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("abc:rootless");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no special logic for schemes
            BOOST_TEST_NOT(u.buffer() == "abc:rootless");
        }();
        []{
            result<url> base = parse_uri("abc:path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("abc:rootless");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // href: "abc:rootless"
            BOOST_TEST_CSTR_EQ(u.scheme(), "abc");
            BOOST_TEST_CSTR_EQ(u.encoded_path(), "rootless");
        }();
        []{
            result<url> base = parse_uri("abc://host/path");
            if (!BOOST_TEST(base))
                return;
            result<url> ref = parse_uri_reference("abc:/rooted");
            if (!BOOST_TEST(ref))
                return;
            url u;
            result<void> r = resolve(*base, *ref, u);
            BOOST_TEST(r);
            u.normalize_authority();
            u.normalize_path();
            // Boost.URL has no special logic for schemes
            BOOST_TEST_NOT(u.buffer() == "abc:/rooted");
        }();
        // Empty query and fragment with blank should throw an error
        []{
            result<url> r = parse_uri_reference("#");
            url u = *r;
        }();
        []{
            result<url> r = parse_uri_reference("?");
            url u = *r;
        }();
        // Last component looks like a number, but not valid IPv4
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://1.2.3.4.5");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://1.2.3.4.5.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://0..0x300/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://0..0x300./");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://256.256.256.256.256");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("http://other.com/");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://256.256.256.256.256.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://1.2.3.08");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://1.2.3.08.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://1.2.3.09");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://09.2.3.4");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://09.2.3.4.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://01.2.3.4.5");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://01.2.3.4.5.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://0x100.2.3.4");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://0x100.2.3.4.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://0x1.2.3.4.5");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://0x1.2.3.4.5.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://foo.1.2.3.4");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://foo.1.2.3.4.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://foo.2.3.4");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://foo.2.3.4.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://foo.09");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://foo.09.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://foo.0x4");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL can parse an overflown IPv4address as a valid reg-name
            // All chars in the invalid IPv4address are unreserved
            result<url> ref = parse_uri_reference("http://foo.0x4.");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not fail when a component looks like a number
            // All unreserved chars are accepted
            result<url> ref = parse_uri_reference("http://foo.09..");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not fail when a component looks like a number
            // All unreserved chars are accepted
            result<url> ref = parse_uri_reference("http://0999999999999999999/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not fail when a component looks like a number
            // All unreserved chars are accepted
            result<url> ref = parse_uri_reference("http://foo.0x");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not fail when a component looks like a number
            // All unreserved chars are accepted
            result<url> ref = parse_uri_reference("http://foo.0XFfFfFfFfFfFfFfFfFfAcE123");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("http://💩.123/");
            // BOOST_TEST_NOT(ref);
        }();
        // U+0000 and U+FFFF in various places
        []{
            result<url> base = parse_uri("about:blank");
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("https://\x00" "y");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("https://x/\x00" "y");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("https://x/?\x00" "y");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("https://x/?#\x00" "y");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("https://\uffffy");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("https://x/\uffffy");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("https://x/?\uffffy");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("https://x/?#\uffffy");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("non-special:\x00" "y");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("non-special:x/\x00" "y");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("non-special:x/?\x00" "y");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Invalid input: contains "\0"
            // result<url> ref = parse_uri_reference("non-special:x/?#\x00" "y");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("non-special:\uffffy");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("non-special:x/\uffffy");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("non-special:x/?\uffffy");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("non-special:x/?#\uffffy");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not fail on empty references
            result<url> ref = parse_uri_reference("");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does not support invalid path char """
            // Node.js and Ada would attempt to encode these chars
            result<url> ref = parse_uri_reference("https://example.com/\"quoted\"");
            BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            if (!BOOST_TEST(base))
                return;
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("https://a%C2%ADb/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does not support unicode input
            // result<url> ref = parse_uri_reference("https://\xad" "/");
            // BOOST_TEST_NOT(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid encoded unicode
            result<url> ref = parse_uri_reference("https://%C2%AD/");
            BOOST_TEST(ref);
        }();
        []{
            result<url> base = parse_uri("about:blank");
            // Boost.URL does support invalid IDNA
            result<url> ref = parse_uri_reference("https://xn--/");
            BOOST_TEST(ref);
        }();
    }

    void
    run()
    {
        // An adaptation of ada unit tests to ensure boost.url
        // can handle all the same URLs or at least the differences
        // are explicit
        // https://github.com/ada-url/ada
        adaBasicTests();
        urlTestData();
    }
};

TEST_SUITE(ada_test, "boost.url.compat.ada");

} // urls
} // boost
