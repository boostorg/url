//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#include <boost/url.hpp>

#include "test_suite.hpp"

#include <iostream>
#include <list>
#include <string>

namespace boost {
namespace urls {
namespace grammar {

struct doc_container_test
{
    //[code_container_4_1
    auto segs( url_view const& u ) -> std::list< std::string >
    {
        std::list< std::string > seq;
        for( auto s : u.encoded_segments() )
            seq.push_back( s );
        return seq;
    }
    //]

    void
    run()
    {
        // segs()
        {
            url_view u;
            segs(u);
        }
    }
};

TEST_SUITE(
    doc_container_test,
    "boost.url.doc.container");

} // grammar
} // urls
} // boost
