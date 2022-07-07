//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CppAlliance/url
//

#include <boost/url/url.hpp>
#include <boost/url/const_string.hpp>
#include <boost/url/static_pool.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>

int
main(int argc, char** argv)
{
    namespace urls = boost::urls;
    namespace fs = boost::filesystem;

    // Check command line arguments.
    fs::path doc_root;
    if(argc == 2)
    {
        doc_root = argv[1];
    }
    else
    {
        fs::path exec = argv[0];
        exec = exec.filename();
        std::cerr <<
            "Usage: " << exec.c_str() << " <doc_root>\n"
            "Using default values\n";
        doc_root = fs::current_path();
    }

    // Parse a URL
    urls::url u(
        "https://www.boost.org/doc/libs/1_79_0/libs/json/doc/html/index%2Ehtml");
    u.normalize_path();

    // Construct path from segments
    fs::path p = doc_root;
    urls::static_pool<1024> pool;
    auto stack_allocator = pool.allocator();
    for (urls::const_string seg: u.segments(stack_allocator))
    {
        p /= seg;
    }

    std::cout << "Path: " << p << "\n";

    return EXIT_SUCCESS;
}
