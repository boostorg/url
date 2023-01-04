//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
//

//[example_router

/*
    This example defines a router for URL paths.
    Each path is associated with a callback
    function.
*/

#include <router.hpp>
#include <src.hpp>
#include <iostream>
#include <functional>

namespace urls = boost::urls;
using string_view = urls::string_view;

int
main(int argc, char **argv)
{
    namespace urls = boost::urls;

    // Check command line arguments.
    if (argc != 2)
    {
        string_view exec = argv[0];
        auto file_pos = exec.find_last_of("/\\");
        if (file_pos != string_view::npos)
            exec = exec.substr(file_pos + 1);
        std::cerr
            << "Usage: " << exec
            << " <target>\n"
               "Example: " << exec << " /user/32\n";
        return EXIT_FAILURE;
    }

    urls::router<std::function<void()>> r;

    auto m = r.match(argv[1]);
    if (m)
    {
        std::cout << "Matched route\n";
        return EXIT_SUCCESS;
    }

    std::cerr << "No route found for " << argv[1] << "\n";
    return EXIT_FAILURE;
}

//]
