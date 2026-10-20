//
// Copyright (c) 2022 alandefreitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//

//[example_qrcode

/*
    This example shows how to construct and modify
    URLs to consume a third party API to
    generate QR Codes.
    https://goqr.me/api/doc/create-qr-code/
*/

#include <boost/url/url.hpp>
#include <boost/core/detail/string_view.hpp>
#include <boost/url/parse.hpp>
#include <iostream>

namespace urls = boost::urls;
namespace core = boost::core;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << argv[0] << "\n";
        std::cout << "Usage: qrcode <data> <width> <height> <output encoding> <error correction> <margin>\n"
                     "options:\n"
                     "    <data>:             The data to encode (required)\n"
                     "    <width>:            Image width (default: 100)\n"
                     "    <height>:           Image height (default: width)\n"
                     "    <output encoding>:  UTF-8, ISO-8859-1 (default: UTF-8)\n"
                     "    <error correction>: percentage of error correction (default: 7)\n"
                     "    <margin>:           border width in modules (default: 4)\n"
                     "examples:\n"
                     "qrcode \"Hello world\"\n";
        return EXIT_FAILURE;
    }

    urls::url u =
        urls::parse_uri(
            "https://api.qrserver.com/v1/create-qr-code/").value();
    auto ps = u.params();

    // Data
    ps.append({"data", argv[1]});

    // Size
    std::size_t width = argc < 3 ? 100 : std::stoll(argv[2]);
    std::size_t height = argc < 4 ? width : std::stoll(argv[3]);
    ps.append({"size", std::to_string(width) + "x" + std::to_string(height)});

    // Encoding
    if (argc >= 5)
    {
         core::string_view output_encoding =
            core::string_view(argv[4]) == "ISO-8859-1" ?
                argv[4] : "UTF-8";
         ps.append({"charset-target", output_encoding});
    }

    // Error correction
    if (argc >= 6)
    {
        std::size_t err = std::stoll(argv[5]);
        core::string_view ecc;
        if (err < 11)
            ecc = "L";
        else if (err < 20)
            ecc = "M";
        else if (err < 27)
            ecc = "Q";
        else
            ecc = "H";
        ps.append({"ecc", ecc});
    }

    // Margin
    std::size_t margin = argc < 7 ? 4 : std::stoll(argv[6]);
    ps.append({"qzone", std::to_string(margin)});

    std::cout << u << '\n';

    return EXIT_SUCCESS;
}

//]
