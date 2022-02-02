//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CppAlliance/url
//

//[snippet_headers_1
#include <boost/url.hpp>
//]

#if 0
//[snippet_headers_2
#include <boost/url/src.hpp>
//]
#endif

#include <iostream>

//[snippet_headers_3
#include <boost/url.hpp>
using namespace boost::urls;
//]

void
using_url_views() {
    //[snippet_parsing_1
    string_view url_str = "https://user:pass@www.example.com:443/path/to/my%2dfile.txt?id=42&name=John%20Doe#page%20anchor";
    //]

    {
        //[snippet_parsing_2
        result<url_view> r = parse_uri( url_str );
        //]
    }

    {
        //[snippet_parsing_3
        url_view u = parse_uri( url_str ).value();
        //]
    }

    {
        //[snippet_parsing_4
        try
        {
            url_view u = parse_uri(url_str).value();
        }
        catch (std::invalid_argument const&)
        {
            // this happens
        }
        //]
    }

    {
        //[snippet_parsing_5
        result<url_view> r = parse_uri( url_str );
        if (r.has_value())
        {
            url_view u = r.value();
        }
        else
        {
            // this happens
        }
        //]
    }

    url_view u = parse_uri( url_str ).value();

    //[snippet_accessing_1
    std::cout <<
        "scheme    : " << u.scheme()            << '\n' <<
        "authority : " << u.encoded_authority() << '\n' <<
        "path      : " << u.encoded_path()      << '\n' <<
        "query     : " << u.encoded_query()     << '\n' <<
        "fragment  : " << u.encoded_fragment()  << '\n';
    //]

    //[snippet_decoding_1
    std::cout <<
        "query    : " << u.query()            << '\n' <<
        "fragment : " << u.fragment()         << '\n';
    //]

    {
        //[snippet_allocators_1
        static_pool< 1024 > sp;
        std::cout <<
            "query    : " << u.query(sp.allocator())    << '\n' <<
            "fragment : " << u.fragment(sp.allocator()) << '\n';
        //]
    }

    {
        //[snippet_compound_elements_1
        segments_encoded_view segs = u.encoded_segments();
        for( auto v : segs )
        {
            std::cout << v << "\n";
        }
        //]
    }

    {
        //[snippet_encoded_compound_elements_1
        segments_view segs = u.segments();

        for( auto v : segs )
        {
            std::cout << v << "\n";
        }
        //]
    }

    {
        //[snippet_encoded_compound_elements_2
        static_pool< 1024 > pool;

        segments_view segs = u.segments( pool.allocator() );

        for( auto v : segs )
        {
            std::cout << v << "\n";
        }
        //]
    }

    {
        //[snippet_encoded_compound_elements_3
        params_encoded_view params = u.encoded_params();

        for( auto v : params )
        {
            std::cout <<
                "key = " << v.key <<
                ", value = " << v.value << "\n";
        }
        //]
    }

    {
        //[snippet_encoded_compound_elements_4
        static_pool< 1024 > pool;

        params_view params = u.params( pool.allocator() );

        for( auto v : params )
        {
            std::cout <<
                "key = " << v.key <<
                ", value = " << v.value << "\n";
        }
        //]
    }
}

void
using_urls() {
    string_view url_str = "https://user:pass@www.example.com:443/path/to/my%2dfile.txt?id=42&name=John%20Doe#page%20anchor";

    //[snippet_modification_1
    url u = parse_uri( url_str ).value();
    //]

    //[snippet_modification_2
    u.set_scheme( "https" );
    //]

    //[snippet_modification_3
    u.set_scheme( scheme::https ); // equivalent to u.set_scheme( "https" );
    //]

    //[snippet_modification_4
    try
    {
        u.set_scheme( "100" ); // illegal, must start with a letter
    }
    catch( std::invalid_argument const& )
    {
        // this happens
    }
    //]

    //[snippet_modification_5
    u.set_host( parse_ipv4_address( "192.168.0.1" ).value() )
        .set_port( 8080 )
        .remove_userinfo();
    //]

    //[snippet_modification_6
    params p = u.params();
    p.emplace_at(p.find("name"), "name", "Vinnie Falco");
    std::cout << u << '\n';
    //]
}

void
parsing_urls() {
    //[snippet_parsing_url_1
    result< url_view > r = parse_uri( "https://www.example.com/path/to/file.txt" );

    if( r.has_value() )                         // parsing was successful
    {
        url_view u = r.value();                 // extract the url_view

        std::cout << u;                         // format the URL to cout
    }
    else
    {
        std::cout << r.error().message();       // parsing failure; print error
    }
    //]

    //[snippet_parsing_url_2
    // This will hold our copy
    std::shared_ptr<url_view const> sp;
    {
        // result::value() will throw an exception if an error occurs
        url_view u = parse_relative_ref( "/path/to/file.txt" ).value();

        // create a copy with ownership and string lifetime extension
        sp = u.collect();

        // At this point the string literal goes out of scope
    }

    // but `*sp` remains valid since it has its own copy
    std::cout << *sp;
    //]
}

int
main(int, char**)
{
    using_url_views();
    using_urls();
    parsing_urls();
    return EXIT_SUCCESS;
}
