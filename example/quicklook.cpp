//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CppAlliance/url
//

#include <boost/url.hpp>
#include <iostream>

using namespace boost::urls;

int
main(int, char**)
{
    result<url_view> r = parse_uri( "https://user:pass@www.example.com:443/path/to/my%2dfile.txt?id=42&name=John%20Doe#page%20anchor" );
    if (r.has_error())
    {
        std::cerr << r.error().what() << '\n';
        return EXIT_FAILURE;
    }

    url_view u = r.value();
    std::cout << u << 'n';

    std::cout <<
        "scheme    : " << u.scheme()            << '\n' <<
        "authority : " << u.encoded_authority() << '\n' <<
        "path      : " << u.encoded_path()      << '\n' <<
        "query     : " << u.encoded_query()     << '\n' <<
        "fragment  : " << u.encoded_fragment()  << '\n';

    std::cout <<
        "query    : " << u.query()            << '\n' <<
        "fragment : " << u.fragment()         << '\n';

    {
        segments_encoded_view segs = u.encoded_segments();
        for( auto v : segs )
        {
            std::cout << v << "\n";
        }
    }

    {
        segments_view segs = u.segments();

        for( auto v : segs )
        {
            std::cout << v << "\n";
        }
    }

    {
        static_pool< 1024 > pool;

        segments_view segs = u.segments( pool.allocator() );

        for( auto v : segs )
        {
            std::cout << v << "\n";
        }
    }

    {
        params_encoded_view params = u.encoded_params();

        for( auto v : params )
        {
            std::cout <<
                "key = " << v.key <<
                ", value = " << v.value << "\n";
        }
    }

    {
        static_pool< 1024 > pool;

        params_view params = u.params( pool.allocator() );

        for( auto v : params )
        {
            std::cout <<
                "key = " << v.key <<
                ", value = " << v.value << "\n";
        }
    }

    {
        url v = parse_uri( "http://user:pass@www.example.com:443/path/to/my%2dfile.txt?id=42&name=John%20Doe#page%20anchor" ).value();
        v.set_scheme( "https" );
        v.set_scheme( scheme::https ); // equivalent to u.set_scheme( "https" );
        try
        {
            v.set_scheme( "100" ); // illegal, must start with a letter
        }
        catch( std::invalid_argument const& )
        {
            // this happens
        }
        v.set_host( parse_ipv4_address( "192.168.0.1" ).value() )
            .set_port( 8080 )
            .remove_userinfo();

        params p = v.params();
        p.emplace_at(p.find("name"), "name", "Vinnie Falco");
        std::cout << v << '\n';
    }

    return EXIT_SUCCESS;
}
