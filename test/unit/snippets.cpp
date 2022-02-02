//
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#include "test_suite.hpp"

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
using_url_views()
{
    //[snippet_parsing_1
    string_view s = "https://user:pass@www.example.com:443/path/to/my%2dfile.txt?id=42&name=John%20Doe#page%20anchor";
    //]

    {
        //[snippet_parsing_2
        result<url_view> r = parse_uri( s );
        //]
    }

    {
        //[snippet_parsing_3
        url_view u = parse_uri( s ).value();
        //]
    }

    {
        //[snippet_parsing_4
        try
        {
            url_view u = parse_uri(s).value();
        }
        catch (std::invalid_argument const&)
        {
            // handle error
        }
        //]
    }

    {
        //[snippet_parsing_5
        result<url_view> r = parse_uri( s );
        if (r.has_value())
        {
            url_view u = r.value();
        }
        else
        {
            // handle error
        }
        //]
    }

    url_view u = parse_uri( s ).value();

    //[snippet_accessing_1
    std::cout <<
        "scheme    : " << u.scheme()            << "\n"
        "authority : " << u.encoded_authority() << "\n"
        "path      : " << u.encoded_path()      << "\n"
        "query     : " << u.encoded_query()     << "\n"
        "fragment  : " << u.encoded_fragment()  << "\n";
    //]

    {
        //[snippet_accessing_2
        url_view u1 = parse_uri( "http://www.example.com" ).value();
        std::cout << "fragment 1 : " << u1.encoded_fragment() << "\n\n";

        url_view u2 = parse_uri( "http://www.example.com/#" ).value();
        std::cout << "fragment 2 : " << u2.encoded_fragment() << "\n\n";
        //]
    }

    {
        //[snippet_accessing_3
        url_view u1 = parse_uri( "http://www.example.com" ).value();
        std::cout << "has fragment 1 : " << u1.has_fragment() << "\n";
        std::cout << "fragment 1 : " << u1.encoded_fragment() << "\n\n";

        url_view u2 = parse_uri( "http://www.example.com/#" ).value();
        std::cout << "has fragment 2 : " << u2.has_fragment() << "\n";
        std::cout << "fragment 2 : " << u2.encoded_fragment() << "\n\n";
        //]
    }

    //[snippet_decoding_1
    std::cout <<
        "query    : " << u.query()    << "\n"
        "fragment : " << u.fragment() << "\n";
    //]

    {
        //[snippet_allocators_1
        static_pool< 1024 > sp;
        std::cout <<
            "query    : " << u.query(sp.allocator())    << "\n"
            "fragment : " << u.fragment(sp.allocator()) << "\n";
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
using_urls()
{
    string_view s = "https://user:pass@www.example.com:443/path/to/my%2dfile.txt?id=42&name=John%20Doe#page%20anchor";

    //[snippet_modification_1
    url u = parse_uri( s ).value();
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
        // handle error
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
    std::cout << u << "\n";
    //]
}

void
parsing_urls()
{
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
        std::string s = "/path/to/file.txt";

        // result::value() will throw an exception if an error occurs
        url_view u = parse_relative_ref( s ).value();

        // create a copy with ownership and string lifetime extension
        sp = u.collect();

        // At this point the string goes out of scope
    }

    // but `*sp` remains valid since it has its own copy
    std::cout << *sp << "\n";
    //]

    {
        //[snippet_parsing_url_3
        // This will hold our mutable copy
        url v;
        {
            std::string s = "/path/to/file.txt";

            // result::value() will throw an exception if an error occurs
            v = parse_relative_ref(s).value();

            // At this point the string goes out of scope
        }

        // but `v` remains valid since it has its own copy
        std::cout << v << "\n";

        // and it's mutable
        v.set_encoded_fragment("anchor");

        std::cout << v << "\n";
        //]
    }
}

void
parsing_scheme()
{
    {
        //[snippet_parsing_scheme_1
        string_view s = "mailto:name@email.com";
        url_view u = parse_uri( s ).value();
        std::cout << u.scheme() << "\n";
        //]
    }
    {
        string_view s = "mailto:name@email.com";
        //[snippet_parsing_scheme_2
        url_view u = parse_uri( s ).value();
        if (u.has_scheme())
        {
            std::cout << u.scheme() << "\n";
        }
        //]
    }
    {
        //[snippet_parsing_scheme_3
        string_view s = "file://host/path/to/file";
        url_view u = parse_uri( s ).value();
        if (u.scheme_id() == scheme::file)
        {
            // handle file
        }
        //]
    }
}

void
parsing_authority()
{
    {
        //[snippet_parsing_authority_1
        string_view s = "https:///path/to_resource";
        url_view u = parse_uri( s ).value();
        std::cout << u << "\n"
            "scheme:        " << u.scheme()            << "\n"
            "has authority: " << u.has_authority()     << "\n"
            "authority:     " << u.encoded_authority() << "\n"
            "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_2
        string_view s = "https://www.boost.org";
        url_view u = parse_uri( s ).value();
        std::cout << "scheme:        " << u.scheme()            << "\n"
                     "has authority: " << u.has_authority()     << "\n"
                     "authority:     " << u.encoded_authority() << "\n"
                     "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_3
        string_view s = "https://www.boost.org/users/download/";
        url_view u = parse_uri( s ).value();
        std::cout << u << "\n"
            "scheme:        " << u.scheme()   << "\n"
            "has authority: " << u.has_authority()     << "\n"
            "authority:     " << u.encoded_authority() << "\n"
            "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_4
        string_view s = "https://www.boost.org/";
        url_view u = parse_uri( s ).value();
        std::cout << "scheme:        " << u.scheme()            << "\n"
                     "has authority: " << u.has_authority()     << "\n"
                     "authority:     " << u.encoded_authority() << "\n"
                     "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_5
        string_view s = "mailto:John.Doe@example.com";
        url_view u = parse_uri( s ).value();
        std::cout << "scheme:        " << u.scheme()            << "\n"
                     "has authority: " << u.has_authority()     << "\n"
                     "authority:     " << u.encoded_authority() << "\n"
                     "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_6
        string_view s = "mailto://John.Doe@example.com";
        url_view u = parse_uri( s ).value();
        std::cout << u << "\n"
            "scheme:        " << u.scheme()   << "\n"
            "has authority: " << u.has_authority()     << "\n"
            "authority:     " << u.encoded_authority() << "\n"
            "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_7
        string_view s = "https://john.doe@www.example.com:123/forum/questions/";
        url_view u = parse_uri( s ).value();
        std::cout << "scheme:        " << u.scheme()   << "\n"
            "has authority: " << u.has_authority()     << "\n"
            "authority:     " << u.encoded_authority() << "\n"
            "host:          " << u.encoded_host()      << "\n"
            "userinfo:      " << u.encoded_userinfo()  << "\n"
            "port:          " << u.port()              << "\n"
            "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_8
        string_view s = "https://john.doe@www.example.com:123/forum/questions/";
        url_view u = parse_uri( s ).value();
        std::cout << u << "\n"
            "encoded host:  " << u.encoded_host()          << "\n"
            "host:          " << u.host()                  << "\n"
            "host and port: " << u.encoded_host_and_port() << "\n"
            "port:          " << u.port()                  << "\n"
            "port number:   " << u.port_number()           << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_9
        string_view s = "https://john.doe@192.168.2.1:123/forum/questions/";
        url_view u = parse_uri( s ).value();
        std::cout << u << "\n"
            "encoded host:  " << u.encoded_host()          << "\n"
            "host:          " << u.host()                  << "\n"
            "host and port: " << u.encoded_host_and_port() << "\n"
            "port:          " << u.port()                  << "\n"
            "port number:   " << u.port_number()           << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_10
        string_view s = "https://www.boost.org/users/download/";
        url_view u = parse_uri( s ).value();
        switch (u.host_type())
        {
        case host_type::name:
            // resolve name
        case host_type::ipv4:
        case host_type::ipv6:
        case host_type::ipvfuture:
            // connect to ip
            break;
        case host_type::none:
            // handle empty host URL
            break;
        }
        //]
    }
    {
        //[snippet_parsing_authority_11
        string_view s = "https://john.doe:123456@www.somehost.com/forum/questions/";
        url_view u = parse_uri( s ).value();
        std::cout << u << "\n\n"
            // userinfo
            "has_userinfo:     " << u.has_userinfo()     << "\n"
            "encoded_userinfo: " << u.encoded_userinfo() << "\n"
            "userinfo:         " << u.userinfo()         << "\n\n"
            // user
            "encoded_user:     " << u.encoded_user()     << "\n"
            "user:             " << u.user()             << "\n\n"
            // password
            "has_password:     " << u.has_password()     << "\n"
            "encoded_password: " << u.encoded_password() << "\n"
            "password:         " << u.password()         << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_12
        string_view s = "www.example.com:80";
        authority_view a = parse_authority( s ).value();
        std::cout << a << "\n\n"
            // host and port
            "encoded_host_and_port: " << a.encoded_host_and_port()  << "\n"
            "encoded_host:          " << a.encoded_host() << "\n"
            "host:                  " << a.host()         << "\n"
            "port:                  " << a.port()         << "\n"
            "port number:           " << a.port_number()  << "\n\n"
            // userinfo
            "has_userinfo:          " << a.has_userinfo()     << "\n"
            "encoded_userinfo:      " << a.encoded_userinfo() << "\n"
            "userinfo:              " << a.userinfo()         << "\n\n"
            // user
            "encoded_user:          " << a.encoded_user()     << "\n"
            "user:                  " << a.user()             << "\n\n"
            // password
            "has_password:          " << a.has_password()     << "\n"
            "encoded_password:      " << a.encoded_password() << "\n"
            "password:              " << a.password()         << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_13
        string_view s = "user:pass@www.example.com:443";
        authority_view a = parse_authority( s ).value();
        std::cout << a << "\n\n"
            // host and port
            "encoded_host_and_port: " << a.encoded_host_and_port()  << "\n"
            "encoded_host:          " << a.encoded_host() << "\n"
            "host:                  " << a.host()         << "\n"
            "port:                  " << a.port()         << "\n"
            "port number:           " << a.port_number()  << "\n\n"
            // userinfo
            "has_userinfo:          " << a.has_userinfo()     << "\n"
            "encoded_userinfo:      " << a.encoded_userinfo() << "\n"
            "userinfo:              " << a.userinfo()         << "\n\n"
            // user
            "encoded_user:          " << a.encoded_user()     << "\n"
            "user:                  " << a.user()             << "\n\n"
            // password
            "has_password:          " << a.has_password()     << "\n"
            "encoded_password:      " << a.encoded_password() << "\n"
            "password:              " << a.password()         << "\n";
        //]
    }
}

void
parsing_path()
{
    {
        //[snippet_parsing_path_1
        string_view s = "https://www.boost.org/doc/libs/";
        url_view    u = parse_uri(s).value();
        std::cout << u << "\n"
            << "path:             " << u.encoded_path()     << "\n"
            << "encoded segments: " << u.encoded_segments() << "\n"
            << "segments:         " << u.segments()         << "\n";
        //]

        //[snippet_parsing_path_1_b
        std::cout << u.encoded_segments().size() << " segments\n";
        for (auto seg: u.encoded_segments())
        {
            std::cout << "segment: " << seg << "\n";
        }
        //]
    }

    {
        //[snippet_parsing_path_2
        string_view s = "https://www.boost.org/doc/libs";
        url_view    u = parse_uri(s).value();
        std::cout << u.encoded_segments().size() << " segments\n";
        for (auto seg: u.encoded_segments())
        {
            std::cout << "segment: " << seg << "\n";
        }
        //]
    }

    {
        //[snippet_parsing_path_3
        string_view s = "https://www.boost.org";
        url_view    u = parse_uri(s).value();
        std::cout << u << "\n"
            << "path:             " << u.encoded_path()     << "\n"
            << "encoded segments: " << u.encoded_segments() << "\n"
            << "segments:         " << u.segments()         << "\n";
        //]
    }

    {
        //[snippet_parsing_path_4
        string_view s = "https://www.boost.org//doc///libs";
        url_view    u = parse_uri(s).value();
        std::cout << u << "\n"
                  "path:             " << u.encoded_path()     << "\n"
                  "encoded segments: " << u.encoded_segments() << "\n"
                  "segments:         " << u.segments()         << "\n";
        std::cout << u.encoded_segments().size() << " segments\n";
        for (auto seg: u.encoded_segments())
        {
            std::cout << "segment: " << seg << "\n";
        }
        //]
    }

    {
        {
            //[snippet_parsing_path_5_a
            string_view s = "https://www.boost.org";
            url_view    u = parse_uri(s).value();
            std::cout << u << "\n"
                      << "path:     " << u.encoded_host()            << "\n"
                      << "path:     " << u.encoded_path()            << "\n"
                      << "segments: " << u.encoded_segments().size() << "\n";
            //]
        }
        {
            //[snippet_parsing_path_5_b
            string_view s = "https://www.boost.org/";
            url_view    u = parse_uri(s).value();
            std::cout << u << "\n"
                      << "host:     " << u.encoded_host()            << "\n"
                      << "path:     " << u.encoded_path()            << "\n"
                      << "segments: " << u.encoded_segments().size() << "\n";
            //]
        }
        {
            //[snippet_parsing_path_5_c
            string_view s = "https://www.boost.org//";
            url_view    u = parse_uri(s).value();
            std::cout << u << "\n"
                      << "host:     " << u.encoded_host()            << "\n"
                      << "path:     " << u.encoded_path()            << "\n"
                      << "segments: " << u.encoded_segments().size() << "\n";
            //]
        }
    }

    {
        //[snippet_parsing_path_6
        string_view s = "https://www.boost.org//doc/libs/";
        url_view    u = parse_uri(s).value();
        std::cout << u << "\n"
                  "authority: " << u.encoded_authority() << "\n"
                  "path:      " << u.encoded_path()      << "\n";
        std::cout << u.encoded_segments().size() << " segments\n";
        for (auto seg: u.encoded_segments())
        {
            std::cout << "segment: " << seg << "\n";
        }
        //]
    }

    {
        //[snippet_parsing_path_7
        string_view s = "https://doc/libs/";
        url_view    u = parse_uri(s).value();
        std::cout << u << "\n"
                  "authority: " << u.encoded_authority() << "\n"
                  "path:      " << u.encoded_path()      << "\n";
        std::cout << u.encoded_segments().size() << " segments\n";
        for (auto seg: u.encoded_segments())
        {
            std::cout << "segment: " << seg << "\n";
        }
        //]
    }

    {
        //[snippet_parsing_path_8
        string_view s = "https://www.boost.org/doc@folder/libs:boost";
        url_view    u = parse_uri(s).value();
        std::cout << u << "\n"
                  "authority: " << u.encoded_authority() << "\n"
                  "path:      " << u.encoded_path()      << "\n";
        std::cout << u.encoded_segments().size() << " segments\n";
        for (auto seg: u.encoded_segments())
        {
            std::cout << "segment: " << seg << "\n";
        }
        //]
    }

    {
        //[snippet_parsing_path_9
        string_view s = "/doc/libs";
        segments_encoded_view p = parse_path(s).value();
        std::cout << "path: " << p << "\n";
        std::cout << p.size() << " segments\n";
        for (auto seg: p)
        {
            std::cout << "segment: " << seg << "\n";
        }
        //]
    }
}

void
parsing_query()
{
    {
        //[snippet_parsing_query_1
        string_view s = "https://www.example.com/get-customer.php?id=409&name=Joe&individual";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "has query:     " << u.has_query()     << "\n"
                  "encoded query: " << u.encoded_query() << "\n"
                  "query:         " << u.query()         << "\n";
        std::cout << u.encoded_params().size() << " parameters\n";
        for (auto p: u.encoded_params())
        {
            if (p.has_value)
            {
                std::cout <<
                    "parameter: <" << p.key <<
                    ", " << p.value << ">\n";
            } else {
                std::cout << "parameter: " << p.key << "\n";
            }
        }
        //]
    }
    {
        //[snippet_parsing_query_2
        string_view s = "https://www.example.com/get-customer.php?key-1=value-1&key-2=&key-3&&=value-2";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "has query:     " << u.has_query()     << "\n"
                  "encoded query: " << u.encoded_query() << "\n"
                  "query:         " << u.query()         << "\n";
        std::cout << u.encoded_params().size() << " parameters\n";
        for (auto p: u.encoded_params())
        {
            if (p.has_value)
            {
                std::cout <<
                    "parameter: <" << p.key <<
                    ", " << p.value << ">\n";
            } else {
                std::cout << "parameter: " << p.key << "\n";
            }
        }
        //]
    }
    {
        //[snippet_parsing_query_3
        string_view s = "https://www.example.com/get-customer.php?email=joe@email.com&code=a:2@/!";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "has query:     " << u.has_query()     << "\n"
                  "encoded query: " << u.encoded_query() << "\n"
                  "query:         " << u.query()         << "\n";
        std::cout << u.encoded_params().size() << " parameters\n";
        for (auto p: u.encoded_params())
        {
            if (p.has_value)
            {
                std::cout <<
                    "parameter: <" << p.key <<
                    ", " << p.value << ">\n";
            } else {
                std::cout << "parameter: " << p.key << "\n";
            }
        }
        //]
    }
    {
        //[snippet_parsing_query_4
        string_view s = "https://www.example.com/get-customer.php?name=joe";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "encoded query: " << u.encoded_query() << "\n";
        //]
    }
    {
        //[snippet_parsing_query_5
        string_view s = "https://www.example.com/get-customer.php";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "has query:     " << u.has_query()     << "\n"
                  "encoded query: " << u.encoded_query() << "\n";
        //]
    }
    {
        //[snippet_parsing_query_6
        string_view s = "https://www.example.com/get-customer.php?name=John%20Doe";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "has query:     " << u.has_query()     << "\n"
                  "encoded query: " << u.encoded_query() << "\n"
                  "query:         " << u.query()         << "\n";
        //]
    }
    {
        //[snippet_parsing_query_7
        string_view s = "https://www.example.com/get-customer.php?name=John%26Doe";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "has query:     " << u.has_query()     << "\n"
                  "encoded query: " << u.encoded_query() << "\n"
                  "query:         " << u.query()         << "\n";
        //]
    }
}

void
parsing_fragment()
{
    {
        //[snippet_parsing_fragment_1
        string_view s = "https://www.example.com/index.html#section%202";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "has fragment:     " << u.has_fragment()     << "\n"
                  "encoded fragment: " << u.encoded_fragment() << "\n"
                  "fragment:         " << u.fragment()         << "\n";
        //]
    }
    {
        //[snippet_parsing_fragment_2_a
        string_view s = "https://www.example.com/index.html#";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "has fragment:     " << u.has_fragment()     << "\n"
                  "encoded fragment: " << u.encoded_fragment() << "\n"
                  "fragment:         " << u.fragment()         << "\n";
        //]
    }
    {
        //[snippet_parsing_fragment_2_b
        string_view s = "https://www.example.com/index.html";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "has fragment:     " << u.has_fragment()     << "\n"
                  "encoded fragment: " << u.encoded_fragment() << "\n"
                  "fragment:         " << u.fragment()         << "\n";
        //]
    }
    {
        //[snippet_parsing_fragment_3
        string_view s = "https://www.example.com/index.html#code%20:a@b?c/d";
        url_view u = parse_uri(s).value();
        std::cout << u << "\n"
                  "has fragment:     " << u.has_fragment()     << "\n"
                  "encoded fragment: " << u.encoded_fragment() << "\n"
                  "fragment:         " << u.fragment()         << "\n";
        //]
    }
}

void
using_modifying()
{
    {
        //[snippet_modifying_1
        string_view s = "https://www.example.com";
        url_view u = parse_uri(s).value();
        url v(u);
        //]

        //[snippet_modifying_2
        std::cout << v << "\n"
            "scheme:        " << v.scheme()            << "\n"
            "has authority: " << v.has_authority()     << "\n"
            "authority:     " << v.encoded_authority() << "\n"
            "path:          " << v.encoded_path()      << "\n";
        //]

        //[snippet_modifying_3
        v.set_scheme("http");
        std::cout << v << "\n";
        //]

        //[snippet_modifying_4
        try
        {
            v.set_scheme("http");
        }
        catch( std::exception const& )
        {
            // handle error
        }
        //]

        //[snippet_modifying_5
        v.set_host("www.my example.com");
        std::cout << v << "\n";
        //]


    }
}

namespace boost {
namespace urls {

class snippets_test
{
public:
    void
    run()
    {
        using_url_views();
        using_urls();
        parsing_urls();
        parsing_scheme();
        parsing_authority();
        parsing_path();
        parsing_query();
        parsing_fragment();
        using_modifying();

        BOOST_TEST_PASS();
    }
};

TEST_SUITE(snippets_test, "boost.url.snippets");

} // urls
} // boost
