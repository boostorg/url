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
#include <boost/container/small_vector.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/optional.hpp>
#include <boost/throw_exception.hpp>
#include <functional>
#include <iostream>

/*
 * This example includes a router that exemplifies
 * how we can use the decoded segment iterators to
 * find routes and files.
 */

namespace urls = boost::urls;
namespace fs = boost::filesystem;

enum class method
{
    get,
    post,
    head,
    put,
    del,
    connect,
    options,
    trace,
    patch,
};

enum class status : unsigned short
{
    unknown                             = 0,

    continue_                           = 100,
    switching_protocols                 = 101,
    processing                          = 102,

    ok                                  = 200,
    created                             = 201,
    accepted                            = 202,
    non_authoritative_information       = 203,
    no_content                          = 204,
    reset_content                       = 205,
    partial_content                     = 206,
    multi_status                        = 207,
    already_reported                    = 208,
    im_used                             = 226,

    multiple_choices                    = 300,
    moved_permanently                   = 301,
    found                               = 302,
    see_other                           = 303,
    not_modified                        = 304,
    use_proxy                           = 305,
    temporary_redirect                  = 307,
    permanent_redirect                  = 308,

    bad_request                         = 400,
    unauthorized                        = 401,
    payment_required                    = 402,
    forbidden                           = 403,
    not_found                           = 404,
    method_not_allowed                  = 405,
    not_acceptable                      = 406,
    proxy_authentication_required       = 407,
    request_timeout                     = 408,
    conflict                            = 409,
    gone                                = 410,
    length_required                     = 411,
    precondition_failed                 = 412,
    payload_too_large                   = 413,
    uri_too_long                        = 414,
    unsupported_media_type              = 415,
    range_not_satisfiable               = 416,
    expectation_failed                  = 417,
    misdirected_request                 = 421,
    unprocessable_entity                = 422,
    locked                              = 423,
    failed_dependency                   = 424,
    upgrade_required                    = 426,
    precondition_required               = 428,
    too_many_requests                   = 429,
    request_header_fields_too_large     = 431,
    connection_closed_without_response  = 444,
    unavailable_for_legal_reasons       = 451,
    client_closed_request               = 499,

    internal_server_error               = 500,
    not_implemented                     = 501,
    bad_gateway                         = 502,
    service_unavailable                 = 503,
    gateway_timeout                     = 504,
    http_version_not_supported          = 505,
    variant_also_negotiates             = 506,
    insufficient_storage                = 507,
    loop_detected                       = 508,
    not_extended                        = 510,
    network_authentication_required     = 511,
    network_connect_timeout_error       = 599,
};

// This example is C++11 but std::less<void> is C++14
// boost::container also requires `is_transparent`
template <class T = void>
struct cxx11_less
{
    BOOST_CXX14_CONSTEXPR
    bool
    operator()(const T& x, const T& y) const
    {
        return x < y;
    }
};

template <>
struct cxx11_less<void>
{
    using is_transparent = std::true_type;

    template <class T1, class T2>
    BOOST_CXX14_CONSTEXPR
        bool
        operator()(const T1& x, const T2& y) const
    {
        return x < y;
    }
};

struct router
{
public:
    // In practice, the handler would usually be
    // a std::move_only_function receiving the
    // request and response objects. In this
    // example, we will just execute the handler.
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

        segment_node* node =
            find_or_create_node(prefix);

        node->doc_root =
            std::make_pair(prefix, doc_root);

        return *this;
    }

    // Adds the handler for these routes
    router&
    add(std::initializer_list<method> methods,
        urls::string_view path,
        handler_type &&handler)
    {
        segment_node* node = find_or_create_node(path);

        // Associate the handler with this node
        handlers_.emplace_back(handler);
        for (method m: methods)
        {
            auto idx = static_cast<std::size_t>(m);
            node->handlers[idx] = handlers_.size() - 1;
        }
        return *this;
    }

    template <class Handler>
    router&
    add(std::initializer_list<method> methods,
        urls::string_view path,
        Handler &&handler)
    {
        return add(
            methods,
            path,
            to_canonical(std::forward<Handler>(handler)));
    }

    router&
    add(method m,
        urls::string_view path,
        handler_type &&handler)
    {
        return add({m}, path, std::move(handler));
    }

    template <class Handler>
    router&
    add(method m,
        urls::string_view path,
        Handler &&handler)
    {
        return add(
            m,
            path,
            to_canonical(std::forward<Handler>(handler)));
    }

    router&
    add(urls::string_view path,
        handler_type &&handler)
    {
        return add(method::get, path, std::move(handler));
    }

    template <class Handler>
    router&
    add(urls::string_view path,
        Handler &&handler)
    {
        return add(
            path,
            to_canonical(std::forward<Handler>(handler)));
    }

    // Adds the default handler for the status codes
    router&
    add(std::initializer_list<status> codes,
        handler_type &&handler)
    {
        handlers_.emplace_back(handler);
        for (status c: codes)
        {
            std::size_t i = status_offset(c);
            status_handlers_[i] = handlers_.size() - 1;
        }
        return *this;
    }

    template <class Handler>
    router&
    add(std::initializer_list<status> codes,
        Handler &&handler)
    {
        return add(
            codes,
            to_canonical(std::forward<Handler>(handler)));
    }

    router&
    add(status code,
        handler_type &&handler)
    {
        add({code}, std::move(handler));
        return *this;
    }

    template <class Handler>
    router&
    add(status code,
        Handler &&handler)
    {
        return add(
            code,
            to_canonical(std::forward<Handler>(handler)));
    }

    bool
    route(method m, urls::string_view target)
    {
        // Parse path
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

        // Lookup segment node
        segment_node *node = &root();
        segment_node *last_root = nullptr;
        auto it = segs.begin();
        auto end = segs.end();
        while (node && it != end)
        {
            urls::const_string seg = *it;
            node = get_segment_node(node, seg);
            if (node &&
                node->doc_root.has_value())
                last_root = node;
            ++it;
        }

        // Target doesn't exist
        auto method_idx = static_cast<std::size_t>(m);
        if (!node || node->handlers[method_idx] == npos)
        {
            if (!last_root)
                return route(status::not_found, target);
            else
                return route(
                    last_root->doc_root.value().first,
                    last_root->doc_root.value().second,
                    target);
        }

        // Target exists
        std::size_t h_idx = node->handlers[method_idx];
        if (h_idx == npos)
            return route(status::not_found, target);

        // In practice, we would pass the full
        // request to the handler here
        handlers_[h_idx](target);
        return true;
    }

    bool
    route(urls::string_view target)
    {
        return route(method::get, target);
    }

private:
    static
    handler_type
    to_canonical(std::function<void()> const& f)
    {
        return [f](urls::string_view)
        {
            f();
        };
    }

    static
    handler_type
    to_canonical(handler_type const& f)
    {
        return f;
    }

    bool
    route(status s, urls::string_view target)
    {
        std::size_t idx =
            status_offset(s);
        std::size_t h_idx =
            status_handlers_[idx];
        if (h_idx != npos) {
            // In practice, we would pass the
            // request to the handler here
            handlers_[h_idx](target);
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
            route(status::not_found, target);
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
            return route(status::not_found, target);

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

    static constexpr
    std::size_t n_supported_status = 64;

    static
    std::size_t
    status_offset(status code)
    {
        static constexpr
        std::size_t
        n_category_status[] = {
            1,
            3,
            9,
            9,
            30,
            12
        };

        static constexpr
        std::size_t
        n_category_offset[] = {
            0,
            1,
            4,
            13,
            22,
            52
        };

        auto v = static_cast<std::size_t>(code);
        std::size_t cat = v / 100;
        std::size_t idx = v % 100;
        if (cat > 5)
            return 0;
        if (idx >= n_category_status[cat])
            return 0;
        return n_category_offset[cat] + idx;
    }

    static constexpr std::size_t npos = std::size_t(-1);

    // The nodes with route segments
    struct segment_node
    {
        // Child segments
        // The segment is represented with const_string
        // so we reuse its copy on write.
        // To avoid allocations, the child handler
        // is represented as an index in the pool
        // of segments.
        // We assume segments will typically have
        // at most 10 elements, assuming the
        // routes might correspond to methods.
        boost::container::small_flat_map<
            urls::const_string, std::size_t, 10, cxx11_less<void>> children;

        // Indices of handlers this segment would
        // use for each method.
        std::size_t handlers[9] = {
            npos, npos, npos, npos, npos,
            npos, npos, npos, npos
        };

        // Nodes that represent doc roots
        // are special. When the child route
        // doesn't match exactly, we
        // fall back to the last node
        // with have seen that represents
        // a doc root and handle that file
        boost::optional<
            std::pair<
                std::string, fs::path>> doc_root;
    };

    // Has valid handler
    bool
    has_valid_handler(
        segment_node *parent)
    {
        BOOST_ASSERT(parent);
        for (std::size_t h: parent->handlers)
            if (h != npos)
                return true;
        return false;
    }

    // Find the child node with a given name
    segment_node *
    get_segment_node(
        segment_node *parent,
        urls::string_view child)
    {
        BOOST_ASSERT(parent);
        auto it = parent->children.find(child);
        if (it == parent->children.end())
            return nullptr;
        std::size_t idx = it->second;
        return &nodes_[idx];
    }

    // Make the child node with a given name
    segment_node *
    make_segment_node(
        segment_node *parent,
        urls::const_string child)
    {
        BOOST_ASSERT(parent);
        std::size_t parent_idx = parent - &nodes_.front();
        nodes_.emplace_back();
        parent = &nodes_[parent_idx];
        parent->children.emplace(child, nodes_.size() - 1);
        return &nodes_.back();
    }

    segment_node*
    find_or_create_node(urls::string_view path)
    {
        // Parse path
        urls::result<urls::segments_encoded_view>
            segs_v = urls::parse_path(path);
        if (!segs_v)
            BOOST_THROW_EXCEPTION(
                std::invalid_argument(
                    "Invalid path"));

        // Decoded path
        urls::static_pool<1024> p;
        urls::segments_view segs = segs_v->decoded(
            p.allocator());

        // Iterate existing segment nodes
        segment_node *node = &root();
        auto it = segs.begin();
        auto end = segs.end();
        while (it != end)
        {
            urls::const_string seg = *it;
            segment_node *next =
                get_segment_node(node, seg);
            if (next)
                node = next;
            else
                break;
            ++it;
        }

        // Create any extra required segment nodes
        while (it != end)
        {
            urls::const_string seg = *it;
            node = make_segment_node(node, seg);
            ++it;
        }

        return node;
    }

    // The root segment
    segment_node& root() {
        return nodes_[0];
    }

    // All other segment nodes
    std::vector<segment_node> nodes_{1};

    // Handlers we should use when node is not found
    std::size_t status_handlers_[n_supported_status];

    // The actual handlers
    std::vector<handler_type> handlers_;
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

    // Add some regular routes
    r.add("/", []() {
        std::cout << "Hi!\n";
    }).add(method::get, "user/john", []() {
        std::cout << "This is John!\n";
    }).add(method::del, "user/john", []() {
        std::cout << "Deleting John!\n";
    }).add({method::get, method::post}, "getting_started", []() {
        std::cout << "Instructions\n";
    }).add(status::not_found, [](urls::string_view target) {
        std::cout << target << ": The page has not been found\n";
    });

    // Route the target sent by the user
    r.route(target);

    return EXIT_SUCCESS;
}