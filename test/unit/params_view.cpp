//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/params_view.hpp>

#include <boost/url/url.hpp>
#include <boost/url/url_view.hpp>
#include <boost/core/ignore_unused.hpp>
#include "test_suite.hpp"

namespace boost {
namespace urls {

/*  Legend

    '#' 0x23
    '%' 0x25
    '&' 0x26
    '=' 0x3d
*/
struct params_view_test
{
    static
    bool
    is_equal(
        param_view const& p0,
        param_view const& p1)
    {
        return
            p0.key == p1.key &&
            p0.has_value == p1.has_value &&
            (! p0.has_value ||
                p0.value == p1.value);
    }

    static
    void
    check(
        params_view const& p,
        std::initializer_list<
            param_view> init)
    {
        if(! BOOST_TEST_EQ(
            p.size(), init.size()))
            return;
        auto it0 = p.begin();
        auto it1 = init.begin();
        auto const end = init.end();
        while(it1 != end)
        {
            BOOST_TEST(is_equal(
                *it0, *it1));
            auto tmp = it0++;
            BOOST_TEST_EQ(++tmp, it0);
            ++it1;
        }
        // reverse
        if(init.size() > 0)
        {
            it0 = p.end();
            it1 = init.end();
            do
            {
                auto tmp = it0--;
                BOOST_TEST_EQ(--tmp, it0);
                --it1;
                BOOST_TEST(is_equal(
                    *it0, *it1));
            }
            while(it1 != init.begin());
        }
    }

    static
    void
    modify(
        string_view s0,
        string_view s1,
        void(*f)(params_view&))
     {
        url u("http://user:pass@www.example.com/path/to/file.txt?k=v#f");
        if(! s0.data())
        {
            u.remove_query();
            BOOST_TEST_EQ(u.encoded_query(), "");
        }
        else
        {
            u.set_encoded_query(s0);
            BOOST_TEST_EQ(u.encoded_query(), s0);
        }
        auto p = u.params();
        f(p);
        if(! s1.data())
        {
            BOOST_TEST(! u.has_query());
            BOOST_TEST_EQ(u.encoded_query(), "");
            BOOST_TEST_EQ(u.query(), "");
        }
        else
        {
            BOOST_TEST(u.has_query());
            BOOST_TEST_EQ(u.encoded_query(), s1);
        }
    }

    //--------------------------------------------

    static
    void
    assign(
        params_view& p,
        std::initializer_list<param_view> init)
    {
        p.assign(
            init.begin(), init.end());
    };

    static
    auto
    append(
        params_view& p,
        std::initializer_list<param_view> init) ->
            params_view::iterator
    {
        return p.append(
            init.begin(), init.end());
    };

    static
    auto
    insert(
        params_view& p,
        params_view::iterator before,
        std::initializer_list<param_view> init) ->
            params_view::iterator
    {
        return p.insert(before,
            init.begin(), init.end());
    };

    static
    auto
    replace(
        params_view& p,
        params_view::iterator from,
        params_view::iterator to,
        std::initializer_list<param_view> init) ->
            params_view::iterator
    {
        return p.replace(from, to,
            init.begin(), init.end());
    };

    //--------------------------------------------

    void
    testSpecial()
    {
        BOOST_STATIC_ASSERT(
            ! std::is_default_constructible<
                params_view>::value);

        // params_view(params_view)
        {
            url u;
            params_view p0 = u.params();
            BOOST_TEST_EQ(&p0.url(), &u);
            params_view p1(p0);
            BOOST_TEST_EQ(
                &p0.url(), &p1.url());
            check(p0, {});
            check(p1, {});
        }

        // operator=(params_view)
        {
            url u0, u1;
            params_view p0 = u0.params();
            params_view p1 = u1.params();
            p1 = p0;
            BOOST_TEST_EQ(
                &p0.url(), &p1.url());
            check(p0, {});
            check(p1, {});
        }
    }

    void
    testModifiers()
    {
        using std::next;

        //----------------------------------------

        //
        // operator=(initializer_list)
        //

        modify(
            {},
            {},
            [](params_view& p)
            {
                p = {};
            });

        modify(
            {},
            "y",
            [](params_view& p)
            {
                p = {{ "y" }};
            });

        modify(
            {},
            "y=",
            [](params_view& p)
            {
                p = {{ "y", "" }};
            });

        modify(
            {},
            "y=g",
            [](params_view& p)
            {
                p = {{ "y", "g" }};
            });

        modify(
            {},
            "y=g&",
            [](params_view& p)
            {
                p = {{ "y", "g" }, {}};
            });

        modify(
            {},
            "y=g&z",
            [](params_view& p)
            {
                p = {{ "y", "g" }, { "z" }};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_view& p)
            {
                p = {};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y",
            [](params_view& p)
            {
                p = {{ "y" }};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=",
            [](params_view& p)
            {
                p = {{ "y", "" }};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g",
            [](params_view& p)
            {
                p = {{ "y", "g" }};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&",
            [](params_view& p)
            {
                p = {{ "y", "g" }, {}};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z",
            [](params_view& p)
            {
                p = {{ "y", "g" }, { "z" }};
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "%23=%25&%26==&%3D=%26",
            [](params_view& p)
            {
                // encodings
                p = {{ "#", "%" }, { "&", "=" }, { "=", "&" }};
            });

        //----------------------------------------

        //
        // clear
        //

        modify(
            "",
            {},
            [](params_view& p)
            {
                p.clear();
                BOOST_TEST(! p.url().has_query());
            });

        modify(
            "key",
            {},
            [](params_view& p)
            {
                p.clear();
                BOOST_TEST(! p.url().has_query());
            });

        modify(
            "key=",
            {},
            [](params_view& p)
            {
                p.clear();
                BOOST_TEST(! p.url().has_query());
            });

        modify(
            "key=value",
            {},
            [](params_view& p)
            {
                p.clear();
                BOOST_TEST(! p.url().has_query());
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_view& p)
            {
                p.clear();
                BOOST_TEST(! p.url().has_query());
            });

        //----------------------------------------

        //
        // assign(initializer_list)
        //

        modify(
            {},
            {},
            [](params_view& p)
            {
                p.assign({});
            });

        modify(
            {},
            "y",
            [](params_view& p)
            {
                p.assign(
                    {{ "y" }});
            });

        modify(
            {},
            "y=",
            [](params_view& p)
            {
                p.assign(
                    {{ "y", "" }});
            });

        modify(
            {},
            "y=g",
            [](params_view& p)
            {
                p.assign(
                    {{ "y", "g" }});
            });

        modify(
            {},
            "y=g&",
            [](params_view& p)
            {
                p.assign(
                    {{ "y", "g" }, {} });
            });

        modify(
            {},
            "y=g&z",
            [](params_view& p)
            {
                p.assign(
                    {{ "y", "g" }, { "z" } });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_view& p)
            {
                p.assign({});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y",
            [](params_view& p)
            {
                p.assign(
                    {{ "y" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=",
            [](params_view& p)
            {
                p.assign(
                    {{ "y", "" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g",
            [](params_view& p)
            {
                p.assign(
                    {{ "y", "g" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&",
            [](params_view& p)
            {
                p.assign(
                    {{ "y", "g" }, {} });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z",
            [](params_view& p)
            {
                p.assign(
                    {{ "y", "g" }, { "z" } });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "%23=%25&%26==&%3D=%26",
            [](params_view& p)
            {
                // encodings
                p.assign(
                    {{ "#", "%" }, { "&", "=" }, { "=", "&" }});
            });

        //
        // assign(FwdIt, FwdIt)
        //

        modify(
            {},
            {},
            [](params_view& p)
            {
                p.assign({});
            });

        modify(
            {},
            "y",
            [](params_view& p)
            {
                assign(p,
                    {{ "y" }});
            });

        modify(
            {},
            "y=",
            [](params_view& p)
            {
                assign(p,
                    {{ "y", "" }});
            });

        modify(
            {},
            "y=g",
            [](params_view& p)
            {
                assign(p,
                    {{ "y", "g" }});
            });

        modify(
            {},
            "y=g&",
            [](params_view& p)
            {
                assign(p,
                    {{ "y", "g" }, {} });
            });

        modify(
            {},
            "y=g&z",
            [](params_view& p)
            {
                assign(p,
                    {{ "y", "g" }, { "z" } });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_view& p)
            {
                assign(p, {});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y",
            [](params_view& p)
            {
                assign(p,
                    {{ "y" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=",
            [](params_view& p)
            {
                assign(p,
                    {{ "y", "" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g",
            [](params_view& p)
            {
                assign(p,
                    {{ "y", "g" }});
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&",
            [](params_view& p)
            {
                assign(p,
                    {{ "y", "g" }, {} });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z",
            [](params_view& p)
            {
                assign(p,
                    {{ "y", "g" }, { "z" } });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "%23=%25&%26==&%3D=%26",
            [](params_view& p)
            {
                // encodings
                assign(p,
                    {{ "#", "%" }, { "&", "=" }, { "=", "&" }});
            });

        //----------------------------------------

        //
        // append(param_view)
        //

        modify(
            {},
            "y",
            [](params_view& p)
            {
                auto it = p.append(
                    { "y" });
                BOOST_TEST(is_equal(
                    *it, { "y" }));
            });

        modify(
            {},
            "y=",
            [](params_view& p)
            {
                auto it = p.append(
                    { "y", "" });
                BOOST_TEST(is_equal(
                    *it, { "y", "" }));
            });

        modify(
            {},
            "y=g",
            [](params_view& p)
            {
                auto it = p.append(
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "",
            "&y",
            [](params_view& p)
            {
                auto it = p.append(
                    { "y" });
                BOOST_TEST(is_equal(
                    *it, { "y" }));
            });

        modify(
            "",
            "&y=",
            [](params_view& p)
            {
                auto it = p.append(
                    { "y", "" });
                BOOST_TEST(is_equal(
                    *it, { "y", "" }));
            });

        modify(
            "",
            "&y=g",
            [](params_view& p)
            {
                auto it = p.append(
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "",
            "&key=value",
            [](params_view& p)
            {
                // should not go through
                // initializer_list overload
                auto it = p.append({
                    string_view("key"),
                    string_view("value")});
                BOOST_TEST(is_equal(
                    *it, { "key", "value" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&%23=%25",
            [](params_view& p)
            {
                // encodings
                p.append({ "#", "%" });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&%26==",
            [](params_view& p)
            {
                // encodings
                p.append({ "&", "=" });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&%3D=%26",
            [](params_view& p)
            {
                // encodings
                p.append({ "=", "&" });
            });

        //
        // append(FwdIt, FwdIt)
        //

        modify(
            {},
            "y=g&z=q",
            [](params_view& p)
            {
                auto it = append(p,
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "",
            "&y=g&z=q",
            [](params_view& p)
            {
                auto it = append(p,
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "t",
            "t&y=g&z=q",
            [](params_view& p)
            {
                auto it = append(p,
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&%23=%25&%26==&%3D=%26",
            [](params_view& p)
            {
                // encodings
                append(p,
                    {{ "#", "%" }, { "&", "=" }, { "=", "&" }});
            });

        //
        // append_list(initializer_list)
        //

        modify(
            {},
            "y=g&z=q",
            [](params_view& p)
            {
                auto it = p.append_list(
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "",
            "&y=g&z=q",
            [](params_view& p)
            {
                auto it = p.append_list(
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "t",
            "t&y=g&z=q",
            [](params_view& p)
            {
                auto it = p.append_list(
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&%23=%25&%26==&%3D=%26",
            [](params_view& p)
            {
                // encodings
                p.append_list(
                    {{ "#", "%" }, { "&", "=" }, { "=", "&" }});
            });

        //----------------------------------------

        //
        // insert(iterator, param_view)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&k0=0&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 0),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&k0=0&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 0),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 1),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 2),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&y=g&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 3),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&y=g&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 4),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&y=g",
            [](params_view& p)
            {
                auto it = p.insert(
                    std::next(p.begin(), 5),
                    { "y", "g" });
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&key=value&k2=&k3&k4=4444",
            [](params_view& p)
            {
                // should not go through
                // initializer_list overload
                auto it = p.insert(
                    std::next(p.begin(), 2),
                    {string_view("key"),
                        string_view("value")});
                BOOST_TEST(is_equal(
                    *it, { "key", "value" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&%23=%25&k2=&k3&k4=4444",
            [](params_view& p)
            {
                // encodings
                p.insert(std::next(p.begin(), 2),
                    { "#", "%" });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&%26==&k2=&k3&k4=4444",
            [](params_view& p)
            {
                // encodings
                p.insert(std::next(p.begin(), 2),
                    { "&", "=" });
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&%3D=%26&k2=&k3&k4=4444",
            [](params_view& p)
            {
                // encodings
                p.insert(std::next(p.begin(), 2),
                    { "=", "&" });
            });

        //
        // insert(iterator, FwdIt, FwdIt)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z=q&k0=0&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 0),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&z=q&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 1),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&z=q&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 2),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&y=g&z=q&k3&k4=4444",
            [](params_view& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 3),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&y=g&z=q&k4=4444",
            [](params_view& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 4),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&y=g&z=q",
            [](params_view& p)
            {
                auto it = insert(p,
                    std::next(p.begin(), 5),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&%23=%25&%26==&%3D=%26&k2=&k3&k4=4444",
            [](params_view& p)
            {
                // encodings
                insert(p, std::next(p.begin(), 2),
                    {{ "#", "%" }, { "&", "=" }, { "=", "&" }});
            });

        //
        // insert_list(iterator, initializer_list)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z=q&k0=0&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 0),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&z=q&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 1),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&z=q&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 2),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&y=g&z=q&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 3),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&y=g&z=q&k4=4444",
            [](params_view& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 4),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444&y=g&z=q",
            [](params_view& p)
            {
                auto it = p.insert_list(
                    std::next(p.begin(), 5),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(is_equal(
                    *it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&%23=%25&%26==&%3D=%26&k2=&k3&k4=4444",
            [](params_view& p)
            {
                // encodings
                p.insert_list(std::next(p.begin(), 2),
                    {{ "#", "%" }, { "&", "=" }, { "=", "&" }});
            });

        //----------------------------------------
        
        //
        // erase(iterator)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 0));
                BOOST_TEST(is_equal(
                    *it, { "k1", "1" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 1));
                BOOST_TEST(is_equal(
                    *it, { "k2", "" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 2));
                BOOST_TEST(is_equal(
                    *it, { "k3" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k4=4444",
            [](params_view& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 3));
                BOOST_TEST(is_equal(
                    *it, { "k4", "4444" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3",
            [](params_view& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 4));
                BOOST_TEST_EQ(it, p.end());
            });

        //
        // erase(iterator, iterator)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 0),
                    std::next(p.begin(), 2));
                BOOST_TEST(is_equal(
                    *it, { "k2", "" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 1),
                    std::next(p.begin(), 3));
                BOOST_TEST(is_equal(
                    *it, { "k3" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k4=4444",
            [](params_view& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 2),
                    std::next(p.begin(), 4));
                BOOST_TEST(is_equal(
                    *it, { "k4", "4444" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=",
            [](params_view& p)
            {
                auto it = p.erase(
                    std::next(p.begin(), 3),
                    std::next(p.begin(), 5));
                BOOST_TEST_EQ(it, p.end());
            });

        //
        // erase(string_view, ignore_case_param)
        //

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            [](params_view& p)
            {
                auto n = p.erase("K0");
                BOOST_TEST_EQ(n, 0);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k1=1&K2=3&k3=4&K2=5&k4=6",
            [](params_view& p)
            {
                auto n = p.erase("k0");
                BOOST_TEST_EQ(n, 2);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k1=1&K2=3&k3=4&K2=5&k4=6",
            [](params_view& p)
            {
                auto n = p.erase("K0", ignore_case);
                BOOST_TEST_EQ(n, 2);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k0=2&K2=3&k3=4&K2=5&k4=6",
            [](params_view& p)
            {
                auto n = p.erase("k1");
                BOOST_TEST_EQ(n, 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k0=2&K2=3&k3=4&K2=5&k4=6",
            [](params_view& p)
            {
                auto n = p.erase("K1", ignore_case);
                BOOST_TEST_EQ(n, 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            [](params_view& p)
            {
                auto n = p.erase("k2");
                BOOST_TEST_EQ(n, 0);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&k3=4&k4=6",
            [](params_view& p)
            {
                auto n = p.erase("K2");
                BOOST_TEST_EQ(n, 2);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&k3=4&k4=6",
            [](params_view& p)
            {
                auto n = p.erase("k2", ignore_case);
                BOOST_TEST_EQ(n, 2);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&K2=5&k4=6",
            [](params_view& p)
            {
                auto n = p.erase("k3");
                BOOST_TEST_EQ(n, 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5",
            [](params_view& p)
            {
                auto n = p.erase("k4");
                BOOST_TEST_EQ(n, 1);
            });

        //----------------------------------------

        //
        // replace(iterator, param_view)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.replace(
                    p.find("k0"), { "y", "g" });
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.replace(
                    p.find("k1"), { "y", "g" });
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.replace(
                    p.find("k2"), { "y", "g" });
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&y=g&k4=4444",
            [](params_view& p)
            {
                auto it = p.replace(
                    p.find("k3"), { "y", "g" });
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&y=g",
            [](params_view& p)
            {
                auto it = p.replace(
                    p.find("k4"), { "y", "g" });
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&%23=%25&k2=&k3&k4=4444",
            [](params_view& p)
            {
                // encodings
                p.replace(p.find("k1"),
                    { "#", "%" });
            });

        //
        // replace(iterator, iterator, initializer_list)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z=q&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.replace(
                    p.find("k0"), p.find("k3"),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&z=q&k4=4444",
            [](params_view& p)
            {
                auto it = p.replace(
                    p.find("k1"), p.find("k4"),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&z=q",
            [](params_view& p)
            {
                auto it = p.replace(
                    p.find("k2"), p.end(),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_view& p)
            {
                // clear
                auto it = p.replace(
                    p.find("k0"), p.end(), {});
                BOOST_TEST_EQ(it, p.begin());
                BOOST_TEST_EQ(it, p.end());
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&%23=%25&%26==&%3D=%26&k3&k4=4444",
            [](params_view& p)
            {
                // encodings
                p.replace(
                    p.find("k1"), p.find("k3"),
                    {{ "#", "%" }, { "&", "=" }, { "=", "&" }});
            });

        //
        // replace(iterator, iterator, FwdIt, FwdIt)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "y=g&z=q&k3&k4=4444",
            [](params_view& p)
            {
                auto it = replace(p,
                    p.find("k0"), p.find("k3"),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&y=g&z=q&k4=4444",
            [](params_view& p)
            {
                auto it = replace(p,
                    p.find("k1"), p.find("k4"),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&y=g&z=q",
            [](params_view& p)
            {
                auto it = replace(p,
                    p.find("k2"), p.end(),
                    {{ "y", "g" }, { "z", "q" }});
                BOOST_TEST(
                    is_equal(*it, { "y", "g" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            {},
            [](params_view& p)
            {
                // clear
                auto it = replace(p,
                    p.find("k0"), p.end(), {});
                BOOST_TEST_EQ(it, p.begin());
                BOOST_TEST_EQ(it, p.end());
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&%23=%25&%26==&%3D=%26&k3&k4=4444",
            [](params_view& p)
            {
                // encodings
                replace(p,
                    p.find("k1"), p.find("k3"),
                    {{ "#", "%" }, { "&", "=" }, { "=", "&" }});
            });

        //----------------------------------------

        //
        // unset(iterator)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.unset(next(p.begin(), 0));
                BOOST_TEST(is_equal(*it, { "k0" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.unset(next(p.begin(), 1));
                BOOST_TEST(is_equal(*it, { "k1" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.unset(next(p.begin(), 2));
                BOOST_TEST(is_equal(*it, { "k2" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.unset(next(p.begin(), 3));
                BOOST_TEST(is_equal(*it, { "k3" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4",
            [](params_view& p)
            {
                auto it = p.unset(next(p.begin(), 4));
                BOOST_TEST(is_equal(*it, { "k4" }));
            });

        //
        // set(iterator, string_view)
        //

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=42&k1=1&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.set(next(p.begin(), 0), "42");
                BOOST_TEST(is_equal(*it, { "k0", "42" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=42&k2=&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.set(next(p.begin(), 1), "42");
                BOOST_TEST(is_equal(*it, { "k1", "42" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=42&k3&k4=4444",
            [](params_view& p)
            {
                auto it = p.set(next(p.begin(), 2), "42");
                BOOST_TEST(is_equal(*it, { "k2", "42" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3=42&k4=4444",
            [](params_view& p)
            {
                auto it = p.set(next(p.begin(), 3), "42");
                BOOST_TEST(is_equal(*it, { "k3", "42" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=&k3&k4=42",
            [](params_view& p)
            {
                auto it = p.set(next(p.begin(), 4), "42");
                BOOST_TEST(is_equal(*it, { "k4", "42" }));
            });

        modify(
            "k0=0&k1=1&k2=&k3&k4=4444",
            "k0=0&k1=1&k2=%23%25%26=&k3&k4=4444",
            [](params_view& p)
            {
                // encodings
                p.set(next(p.begin(), 2), "#%&=");
            });

        //
        // set(string_view, string_view, ignore_case_param)
        //

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=x&k1=1&K2=3&k3=4&K2=5&k4=6",
            [](params_view& p)
            {
                auto it = p.set("k0", "x");
                BOOST_TEST(is_equal(*it, { "k0", "x" }));
                BOOST_TEST_EQ(p.count("k0"), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=x&k0=2&K2=3&k3=4&K2=5&k4=6",
            [](params_view& p)
            {
                auto it = p.set("k1", "x");
                BOOST_TEST(is_equal(*it, { "k1", "x" }));
                BOOST_TEST_EQ(p.count("k1"), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6&k2=x",
            [](params_view& p)
            {
                auto it = p.set("k2", "x");
                BOOST_TEST(is_equal(*it, { "k2", "x" }));
                BOOST_TEST_EQ(p.count("k2"), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=x&k3=4&k4=6",
            [](params_view& p)
            {
                auto it = p.set("k2", "x", ignore_case);
                BOOST_TEST(is_equal(*it, { "K2", "x" }));
                BOOST_TEST_EQ(p.count("k2", ignore_case), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=x&K2=5&k4=6",
            [](params_view& p)
            {
                auto it = p.set("k3", "x");
                BOOST_TEST(is_equal(*it, { "k3", "x" }));
                BOOST_TEST_EQ(p.count("k3"), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=x",
            [](params_view& p)
            {
                auto it = p.set("k4", "x");
                BOOST_TEST(is_equal(*it, { "k4", "x" }));
                BOOST_TEST_EQ(p.count("k4"), 1);
            });

        modify(
            "k0=0&k1=1&k0=2&K2=3&k3=4&K2=5&k4=6",
            "k0=0&k1=1&k0=2&K2=%23%25%26=&k3=4&k4=6",
            [](params_view& p)
            {
                // encodings
                p.set("k2", "#%&=", ignore_case);
            });
    }

    void
    testIterator()
    {
        using T = params_view::iterator;

        // iterator()
        {
            T t0;
            T t1;
            BOOST_TEST_EQ(t0, t1);
        }

        // operator==()
        {
            url u;
            BOOST_TEST_EQ(
                u.params().begin(),
                u.params().begin());
        }

        // operator!=()
        {
            url u("?");
            BOOST_TEST_NE(
                u.params().begin(),
                u.params().end());
        }

        // value_type outlives reference
        {
            params_view::value_type v;
            {
                url u("/?a=1&bb=22&ccc=333&dddd=4444#f");
                params_view ps = u.params();
                params_view::reference r = *ps.begin();
                v = params_view::value_type(r);
            }
            BOOST_TEST_EQ(v.key, "a");
            BOOST_TEST_EQ(v.value, "1");
            BOOST_TEST_EQ(v.has_value, true);
        }
    }

    void
    run()
    {
        testSpecial();
        testModifiers();
        testIterator();
    }
};

TEST_SUITE(
    params_view_test,
    "boost.url.params_view");

} // urls
} // boost
