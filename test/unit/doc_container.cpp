//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

#include <boost/url.hpp>

#include "test_suite.hpp"

#include <iostream>
#include <list>
#include <string>

namespace boost {
namespace urls {

struct doc_container_test
{
    //[code_container_4_1
    auto segs( url_view const& u ) -> std::list< std::string >
    {
        std::list< std::string > seq;
        for( auto s : u.encoded_segments() )
            seq.push_back( s.decode() );
        return seq;
    }
    //]

    void
    path_segments()
    {
        auto check = [](
            string_view path,
            std::initializer_list<string_view> segs,
            bool path_abs)
        {
            auto r1 = parse_path(path);
            BOOST_TEST(r1.has_value());
            auto ss = r1.value();
            BOOST_TEST_EQ(segs.size(), ss.size());
            BOOST_TEST_EQ(path_abs, r1->is_absolute());
            auto it0 = segs.begin();
            auto it1 = ss.begin();
            while (it0 != segs.end())
            {
                BOOST_TEST_EQ(*it0, *it1);
                ++it0;
                ++it1;
            }
        };

        check("", { }, false);
        check("/", { }, true);
        check("./", { "" }, false);
        check("./usr", { "usr" }, false);
        check("/index.htm", { "index.htm" }, true);
        check("/images/cat-pic.gif", { "images", "cat-pic.gif" }, true);
        check("images/cat-pic.gif", { "images", "cat-pic.gif" }, false);
        check("/fast//query", { "fast", "", "query" }, true);
        check("fast//", { "fast", "", "" }, false);
        check("/./", { "" }, true);
        check(".//", { "", "" }, false);
    }

    void
    run()
    {
        // segs()
        {
            url_view u;
            segs(u);
        }

        path_segments();
    }
};

TEST_SUITE(
    doc_container_test,
    "boost.url.doc.container");

} // urls
} // boost
