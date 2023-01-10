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

#include "router.hpp"
#include "src.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/config.hpp>

#include <iostream>
#include <functional>

/*
 * Aliases
 */

namespace urls = boost::urls;
namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
using string_view = urls::string_view;
using request_t = http::request<http::string_body>;
using handler = std::function<http::message_generator(request_t const&, urls::matches)>;

/*
 * Auxiliary functions
 */

int
serve(urls::router<handler> r, asio::ip::address a, unsigned short port);

string_view
mime_type(string_view path);

http::response<http::string_body>
string_response(
    string_view msg,
    request_t const& req);

http::message_generator
file_response(
    string_view doc_root,
    string_view path,
    request_t const& req);

std::string
path_cat(
    beast::string_view base,
    beast::string_view path);

int
main(int argc, char **argv)
{
    /*
     * Parse cmd-line params
     */
    if (argc != 4)
    {
        string_view exec = argv[0];
        auto file_pos = exec.find_last_of("/\\");
        if (file_pos != string_view::npos)
            exec = exec.substr(file_pos + 1);
        std::cerr
            << "Usage: " << exec
            << " <address> <port> <doc_root>\n"
               "Example: " << exec << " 0.0.0.0 8080 .\n"
                       "Default values:\n"
                       "- address: 0.0.0.0\n"
                       "- port: 8080\n"
                       "- doc_root: ./\n";
    }
    auto const address = asio::ip::make_address(argc > 1 ? argv[1] : "0.0.0.0");
    auto const port = static_cast<unsigned short>(argc > 2 ? std::atoi(argv[2]) : 8080);
    auto const doc_root = std::string(argc > 3 ? argv[3] : ".");

    /*
     * Create router
     */
    urls::router<handler> r;

    r.insert("/", [&](request_t const& req, urls::matches const&) {
        return string_response("Hello!", req);
    });

    r.insert("/user/{name}", [&](request_t const& req, urls::matches const& m) {
        std::string msg = "Hello, ";
        urls::pct_string_view(m[0]).decode({}, urls::string_token::append_to(msg));
        msg += "!";
        return string_response(msg, req);
    });

    r.insert("/user", [&](request_t const& req, urls::matches const&) {
        std::string msg = "Users: ";
        auto names = {"johndoe", "maria", "alice"};
        for (auto name: names) {
            msg += "<a href=\"/user/";
            msg += name;
            msg += "\">";
            msg += name;
            msg += "</a> ";
        }
        return string_response(msg, req);
    });

    r.insert("/public/{path+}", [&](request_t const& req, urls::matches m) {
        return file_response(doc_root, m["path"], req);
    });

    return serve(std::move(r), address, port);
}

auto
string_response(string_view msg, request_t const& req)
    -> http::response<http::string_body>
{
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = msg;
    res.prepare_payload();
    return res;
}

auto
file_response(string_view doc_root, string_view path, request_t const& req)
    -> http::message_generator
{
    beast::error_code ec;
    http::file_body::value_type body;
    std::string jpath = path_cat(doc_root, path);
    body.open(jpath.c_str(), beast::file_mode::scan, ec);
    auto const size = body.size();
    if(ec == beast::errc::no_such_file_or_directory)
    {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(path) + "' was not found in " + jpath;
        res.prepare_payload();
        BOOST_URL_RETURN(res);
    }
    http::response<http::file_body> res{
        std::piecewise_construct,
        std::make_tuple(std::move(body)),
        std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    BOOST_URL_RETURN(res);
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string
path_cat(
    string_view base,
    string_view path)
{
    if (base.empty())
        return std::string(path);
    std::string result(base);
#ifdef BOOST_MSVC
    char constexpr path_separator = '\\';
#else
    char constexpr path_separator = '/';
#endif
    if( result.back() == path_separator &&
        path.starts_with(path_separator))
        result.resize(result.size() - 1);
    else if (result.back() != path_separator &&
             !path.starts_with(path_separator))
    {
        result.push_back(path_separator);
    }
    result.append(path.data(), path.size());
#ifdef BOOST_MSVC
    for(auto& c : result)
        if(c == '/')
            c = path_separator;
#endif
    return result;
}

int
serve(urls::router<handler> r, asio::ip::address address, unsigned short port)
{
    /*
     * Serve the routes with a simple synchronous
     * server. This is an implementation detail
     * in the context of this example.
     */
    std::cout << "Listening on http://" << address << ":" << port << "\n";
    asio::io_context ioc(1);
    asio::ip::tcp::acceptor acceptor(ioc, {address, port});
    for(;;)
    {
        asio::ip::tcp::socket socket{ioc};
        acceptor.accept(socket);
        beast::error_code ec;
        beast::flat_buffer buffer;
        for(;;)
        {
            // Read a request
            http::request<http::string_body> req;
            http::read(socket, buffer, req, ec);
            if(ec == http::error::end_of_stream)
                break;
            if(ec)
            {
                std::cerr << "read: " << ec.message() << "\n";
                break;
            }

            // Handle request
            bool const method_ok =
                req.method() == http::verb::get ||
                req.method() == http::verb::head;
            auto rpath = urls::parse_path(req.target());
            if (method_ok && rpath)
            {
                urls::matches m;
                if (auto h = r.find(*rpath, m))
                {
                    // good request
                    http::message_generator res = (*h)(req, m);
                    beast::write(socket, std::move(res), ec);
                }
                else
                {
                    // invalid route
                    http::response<http::string_body> res{http::status::not_found, req.version()};
                    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                    res.set(http::field::content_type, "text/html");
                    res.keep_alive(req.keep_alive());
                    res.body() = "The resource '" + std::string(rpath->buffer()) + "' was not found.";
                    res.prepare_payload();
                    http::write(socket, res, ec);
                }
            }
            else
            {
                // bad request
                http::response<http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                if (!method_ok)
                {
                    res.body() = "Unknown HTTP-method: ";
                    res.body() += req.method_string();
                }
                else
                    res.body() = std::string("Illegal request-target");
                res.prepare_payload();
                http::write(socket, res, ec);
            }
            if(ec)
            {
                std::cerr << "write: " << ec.message() << "\n";
                break;
            }
            if(!req.keep_alive())
            {
                break;
            }
        }
        socket.shutdown(asio::ip::tcp::socket::shutdown_send, ec);
    }
    return EXIT_SUCCESS;
}

string_view
mime_type(string_view path)
{
    using beast::iequals;
    auto const ext = [&path]
    {
        auto const pos = path.rfind(".");
        if(pos == beast::string_view::npos)
            return beast::string_view{};
        return path.substr(pos);
    }();
    if(iequals(ext, ".htm"))  return "text/html";
    if(iequals(ext, ".html")) return "text/html";
    if(iequals(ext, ".php"))  return "text/html";
    if(iequals(ext, ".css"))  return "text/css";
    if(iequals(ext, ".txt"))  return "text/plain";
    if(iequals(ext, ".js"))   return "application/javascript";
    if(iequals(ext, ".json")) return "application/json";
    if(iequals(ext, ".xml"))  return "application/xml";
    if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if(iequals(ext, ".flv"))  return "video/x-flv";
    if(iequals(ext, ".png"))  return "image/png";
    if(iequals(ext, ".jpe"))  return "image/jpeg";
    if(iequals(ext, ".jpeg")) return "image/jpeg";
    if(iequals(ext, ".jpg"))  return "image/jpeg";
    if(iequals(ext, ".gif"))  return "image/gif";
    if(iequals(ext, ".bmp"))  return "image/bmp";
    if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if(iequals(ext, ".tiff")) return "image/tiff";
    if(iequals(ext, ".tif"))  return "image/tiff";
    if(iequals(ext, ".svg"))  return "image/svg+xml";
    if(iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

//]
