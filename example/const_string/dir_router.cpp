//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CppAlliance/url
//

#include <boost/url/const_string.hpp>
#include <boost/url/error.hpp>
#include <boost/url/segments_encoded.hpp>
#include <boost/url/segments_encoded_view.hpp>
#include <boost/url/static_pool.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url/static_url.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/throw_exception.hpp>
#include <functional>
#include <iostream>

namespace urls = boost::urls;
namespace fs = boost::filesystem;

/*
 * This example includes a simple file router
 * that exemplifies we can append decoded segment
 * iterators to paths.
 */

struct router
{
public:
    using handler_type = std::function<void(urls::string_view)>;

    // Serves the files at specified prefix
    router&
    add(urls::string_view prefix,
        fs::path doc_root)
    {
        if (!fs::is_directory(doc_root))
            BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "Invalid directory"));

        insert_route(prefix, doc_root);
        return *this;
    }

    // Adds the default handler for errors
    router&
    error_handler(handler_type &&handler)
    {
        error_handler_ = handler;
        return *this;
    }

    bool
    route(urls::string_view target)
    {
        auto* r = get_route_for(target);
        if (r)
            return
                route(r->prefix, r->doc_root, target);
        return route_error(target);
    }

private:
    bool
    route_error(urls::string_view target)
    {
        if (error_handler_) {
            // In practice, we would pass the
            // request to the handler here
            error_handler_(target);
            return true;
        }
        return false;
    }

    bool
    route(
        urls::string_view prefix,
        fs::path const& doc_root,
        urls::string_view target)
    {

        // Normalize target
        auto uv = urls::parse_uri_reference(target);
        if (!uv)
            route_error(target);
        urls::static_url<1024> u = uv.value();
        u.normalize_path();

        // Target segments
        urls::static_pool<1024> p;
        urls::segments target_segs =
            u.segments(p.allocator());

        // Prefix segments
        urls::segments_encoded_view pv =
            urls::parse_path(prefix).value();
        urls::segments_view prefix_segs =
            pv.decoded(p.allocator());

        // Remove prefix segments from target
        auto it0 = target_segs.begin();
        auto end0 = target_segs.end();
        auto it1 = prefix_segs.begin();
        auto end1 = prefix_segs.end();
        while (it0 != end0 && it1 != end1)
        {
            urls::const_string seg0 = *it0;
            urls::const_string seg1 = *it1;
            ++it0;
            ++it1;
            if (seg0 != seg1)
                break;
        }

        // Ensure we consumed the prefix
        if (it1 != end1)
            return route_error(target);

        // Append suffix to doc_root
        fs::path doc = doc_root;
        while (it0 != end0)
        {
            doc /= *it0;
            ++it0;
        }

        // Handle directory target
        if (fs::is_directory(doc))
        {
            fs::directory_iterator it(doc);
            fs::directory_iterator end;
            while (it != end)
            {
                std::cout << it->path() << "\n";
                ++it;
            }
            return true;
        }

        // Handle regular file target
        fs::ifstream in(doc);
        auto it = std::istreambuf_iterator<char>(in);
        auto end = std::istreambuf_iterator<char>();
        std::size_t i = 0;
        for (; i < 1000 && it != end; ++it, ++i)
            std::cout << *it;

        if (i == 1000)
            std::cout << " ...\n";

        return true;
    }

    static constexpr std::size_t npos = std::size_t(-1);

    struct route
    {
        std::string prefix;
        fs::path doc_root;
    };

    // Find the child node with a given name
    struct route*
    get_route_for(
        urls::string_view target)
    {
        // Parse target path
        urls::result<urls::segments_encoded_view>
            segs_v = urls::parse_path(target);
        if (!segs_v)
            BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "Invalid path"));

        // Lazy decoded target segments
        urls::static_pool<1024> p;
        urls::segments_view segs = segs_v->decoded(
            p.allocator());

        // Lookup prefix that matches the target
        for (auto& d: routes_)
        {
            // Check if prefix matches
            auto prefix_segs =
                urls::parse_path(d.prefix).value()
                    .decoded(p.allocator());
            auto it0 = segs.begin();
            auto end0 = segs.end();
            auto it1 = prefix_segs.begin();
            auto end1 = prefix_segs.end();
            while (it0 != end0 &&
                   it1 != end1 &&
                   *it0 == *it1)
            {
                ++it0;
                ++it1;
            }
            if (it1 == end1)
            {
                return &d;
            }
        }
        return nullptr;
    }

    // Make the child node with a given name
    void
    insert_route(
        urls::string_view prefix,
        fs::path doc_root)
    {
        routes_.emplace_back();
        routes_.back().prefix = prefix;
        routes_.back().doc_root = doc_root;
    }

    // All other segment nodes
    std::vector<struct route> routes_;

    // The actual handlers
    handler_type error_handler_;
};


int
main(int argc, char **argv)
{
    namespace urls = boost::urls;
    namespace fs   = boost::filesystem;

    // Check command line arguments.
    if (argc == 1)
    {
        fs::path exec = argv[0];
        exec = exec.filename();
        std::cerr
            << "Usage: " << exec.c_str()
            << " <target> <doc_root> <doc_root2> ...\n"
               "target: path to make a request\n"
               "doc_root: dir(s) to look for files\n";
        return EXIT_FAILURE;
    }
    std::string target = argv[1];
    // Add doc_roots
    fs::path dir;
    if (argc >= 3)
        dir = argv[2];
    if (argc < 3 || !fs::is_directory(dir))
        dir = fs::current_path();

    // Create the router
    router r;

    // Add file routes
    r.add(dir.filename().c_str(), dir);
    for (int i = 3; i < argc; ++i)
    {
        dir = argv[3];
        if (fs::is_directory(dir))
            r.add(dir.filename().c_str(), dir);
    }

    // Add error route
    r.error_handler([](urls::string_view target) {
        std::cout << target << ": The page has not been found\n";
    });

    // Route the target sent by the user
    r.route(target);

    return EXIT_SUCCESS;
}