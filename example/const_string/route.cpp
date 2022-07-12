//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CppAlliance/url
//

#include <boost/url/error.hpp>
#include <boost/url/segments_encoded.hpp>
#include <boost/url/segments_encoded_view.hpp>
#include <boost/url/static_pool.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/static_url.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <algorithm>

namespace urls = boost::urls;
namespace fs = boost::filesystem;
using string_view = urls::string_view;

namespace boost {
namespace filesystem {
namespace path_traits {
template<>
struct is_pathable< urls::pct_decoded_range >
{
    static const bool value = true;
};
}
}
}

class route
{
public:
    route(string_view prefix, fs::path root)
        : prefix_(urls::parse_uri_reference(prefix).value())
        , root_(std::move(root))
    {}

    route(urls::url prefix, fs::path root)
        : prefix_(std::move(prefix))
        , root_(std::move(root))
    {}

    bool match(
        urls::url_view target,
        fs::path& result )
    {
        // Segments
        urls::segments_view target_segs = target.segments();
        urls::segments prefix_segs = prefix_.segments();

        // Match the prefix segments
        if (target_segs.size() < prefix_segs.size())
            return false;
        auto it0 = target_segs.begin();
        auto end0 = target_segs.end();
        auto it1 = prefix_segs.begin();
        auto end1 = prefix_segs.end();
        while (
            it0 != end0 &&
            it1 != end1 &&
            it0.decoded_range() == it1.decoded_range())
        {
            ++it0;
            ++it1;
        }
        if (it1 != end1)
        {
            result = {};
            return false;
        }

        // Append following segments to root
        result = root_;
        while (it0 != end0)
        {
            result.append(it0.decoded_range());
            ++it0;
        }
        return true;
    }

private:
    urls::url prefix_;
    fs::path root_;
};

int
main(int argc, char **argv)
{
    namespace urls = boost::urls;
    namespace fs = boost::filesystem;

    // Check command line arguments.
    if (argc != 4)
    {
        fs::path exec = argv[0];
        exec = exec.filename();
        std::cerr
            << "Usage: " << exec.c_str()
            << " <target> <prefix> <root>\n"
               "target: path to make a request\n"
               "prefix: url prefix\n"
               "doc_root: dir to look for files\n";
        return EXIT_FAILURE;
    }
    urls::result<urls::url_view> target_r =
        urls::parse_uri_reference(argv[1]);
    if (!target_r)
    {
        std::cerr
            << "Error: " << argv[1]
            << " is an invalid target\n";
        return EXIT_FAILURE;
    }
    urls::url target(*target_r);
    target.normalize_path();

    std::string prefix = argv[2];
    fs::path root = argv[2];

    if (!fs::is_directory(root))
    {
        std::cerr
            << "Error: " << root
            << " is not a directory\n";
        return EXIT_FAILURE;
    }

    // Create route
    route r(prefix, root);

    // Check if target matches a file
    fs::path result;
    if (r.match(target, result))
    {
        std::cout
            << target << " would match the file "
            << result << std::endl;
    }
    else
    {
        std::cout
            << "No " << target << " in prefix "
            << prefix << std::endl;
    }

    return EXIT_SUCCESS;
}
