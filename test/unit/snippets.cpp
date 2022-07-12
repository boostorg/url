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

#include <iostream>
#include <cctype>

//[snippet_headers_3
#include <boost/url.hpp>
namespace urls = boost::urls;
//]

void
using_url_views()
{
    //[snippet_parsing_1
    urls::string_view s = "https://user:pass@www.example.com:443/path/to/my%2dfile.txt?id=42&name=John%20Doe#page%20anchor";
    //]

    {
        //[snippet_parsing_2
        urls::result<urls::url_view> r = urls::parse_uri( s );
        //]
    }

    {
        //[snippet_parsing_3
        urls::url_view u = urls::parse_uri( s ).value();
        //]
    }

    urls::url_view u = urls::parse_uri( s ).value();

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
        urls::url_view u1 = urls::parse_uri( "http://www.example.com" ).value();
        std::cout << "fragment 1 : " << u1.encoded_fragment() << "\n\n";

        urls::url_view u2 = urls::parse_uri( "http://www.example.com/#" ).value();
        std::cout << "fragment 2 : " << u2.encoded_fragment() << "\n\n";
        //]
    }

    {
        //[snippet_accessing_3
        urls::url_view u1 = urls::parse_uri( "http://www.example.com" ).value();
        std::cout << "has fragment 1 : " << u1.has_fragment() << "\n";
        std::cout << "fragment 1 : " << u1.encoded_fragment() << "\n\n";

        urls::url_view u2 = urls::parse_uri( "http://www.example.com/#" ).value();
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
        urls::static_pool< 1024 > sp;
        std::cout <<
            "query    : " << u.query(sp.allocator())    << "\n"
            "fragment : " << u.fragment(sp.allocator()) << "\n";
        //]
    }

    {
        //[snippet_compound_elements_1
        urls::segments_encoded_view segs = u.encoded_segments();
        for( auto v : segs )
        {
            std::cout << v << "\n";
        }
        //]
    }

    {
        //[snippet_encoded_compound_elements_1
        urls::segments_view segs = u.segments();

        for( auto v : segs )
        {
            std::cout << v << "\n";
        }
        //]
    }

    {
        //[snippet_encoded_compound_elements_2
        urls::static_pool< 1024 > pool;

        urls::segments_view segs = u.segments( pool.allocator() );

        for( auto v : segs )
        {
            std::cout << v << "\n";
        }
        //]
    }

    {
        //[snippet_encoded_compound_elements_3
        urls::params_encoded_view params = u.encoded_params();

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
        urls::static_pool< 1024 > pool;

        urls::params_view params = u.params( pool.allocator() );

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
    urls::string_view s = "https://user:pass@www.example.com:443/path/to/my%2dfile.txt?id=42&name=John%20Doe#page%20anchor";

    //[snippet_modification_1
    urls::url u = urls::parse_uri( s ).value();
    //]

    //[snippet_modification_2
    u.set_scheme( "https" );
    //]

    //[snippet_modification_3
    u.set_scheme( urls::scheme::https ); // equivalent to u.set_scheme( "https" );
    //]

    //[snippet_modification_4
    u.set_host( urls::parse_ipv4_address( "192.168.0.1" ).value() )
        .set_port( 8080 )
        .remove_userinfo();
    //]

    //[snippet_modification_5
    urls::params p = u.params();
    p.emplace_at(p.find("name"), "name", "Vinnie Falco");
    std::cout << u << "\n";
    //]
}

void
parsing_urls()
{
    //[snippet_parsing_url_1
    urls::result< urls::url_view > r = urls::parse_uri( "https://www.example.com/path/to/file.txt" );

    if( r.has_value() )                         // parsing was successful
    {
        urls::url_view u = r.value();                 // extract the urls::url_view

        std::cout << u;                         // format the URL to cout
    }
    else
    {
        std::cout << r.error().message();       // parsing failure; print error
    }
    //]

    //[snippet_parsing_url_2
    // This will hold our copy
    std::shared_ptr<urls::url_view const> sp;
    {
        std::string s = "/path/to/file.txt";

        // result::value() will throw an exception if an error occurs
        urls::url_view u = urls::parse_relative_ref( s ).value();

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
        urls::url v;
        {
            std::string s = "/path/to/file.txt";

            // result::value() will throw an exception if an error occurs
            v = urls::parse_relative_ref(s).value();

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
        urls::string_view s = "mailto:name@email.com";
        urls::url_view u = urls::parse_uri( s ).value();
        std::cout << u.scheme() << "\n";
        //]
    }
    {
        urls::string_view s = "mailto:name@email.com";
        //[snippet_parsing_scheme_2
        urls::url_view u = urls::parse_uri( s ).value();
        if (u.has_scheme())
        {
            std::cout << u.scheme() << "\n";
        }
        //]
    }
    {
        //[snippet_parsing_scheme_3
        urls::string_view s = "file://host/path/to/file";
        urls::url_view u = urls::parse_uri( s ).value();
        if (u.scheme_id() == urls::scheme::file)
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
        urls::string_view s = "https:///path/to_resource";
        urls::url_view u = urls::parse_uri( s ).value();
        std::cout << u << "\n"
            "scheme:        " << u.scheme()            << "\n"
            "has authority: " << u.has_authority()     << "\n"
            "authority:     " << u.encoded_authority() << "\n"
            "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_2
        urls::string_view s = "https://www.boost.org";
        urls::url_view u = urls::parse_uri( s ).value();
        std::cout << "scheme:        " << u.scheme()            << "\n"
                     "has authority: " << u.has_authority()     << "\n"
                     "authority:     " << u.encoded_authority() << "\n"
                     "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_3
        urls::string_view s = "https://www.boost.org/users/download/";
        urls::url_view u = urls::parse_uri( s ).value();
        std::cout << u << "\n"
            "scheme:        " << u.scheme()   << "\n"
            "has authority: " << u.has_authority()     << "\n"
            "authority:     " << u.encoded_authority() << "\n"
            "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_4
        urls::string_view s = "https://www.boost.org/";
        urls::url_view u = urls::parse_uri( s ).value();
        std::cout << "scheme:        " << u.scheme()            << "\n"
                     "has authority: " << u.has_authority()     << "\n"
                     "authority:     " << u.encoded_authority() << "\n"
                     "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_5
        urls::string_view s = "mailto:John.Doe@example.com";
        urls::url_view u = urls::parse_uri( s ).value();
        std::cout << "scheme:        " << u.scheme()            << "\n"
                     "has authority: " << u.has_authority()     << "\n"
                     "authority:     " << u.encoded_authority() << "\n"
                     "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_6
        urls::string_view s = "mailto://John.Doe@example.com";
        urls::url_view u = urls::parse_uri( s ).value();
        std::cout << u << "\n"
            "scheme:        " << u.scheme()   << "\n"
            "has authority: " << u.has_authority()     << "\n"
            "authority:     " << u.encoded_authority() << "\n"
            "path:          " << u.encoded_path()      << "\n";
        //]
    }
    {
        //[snippet_parsing_authority_7
        urls::string_view s = "https://john.doe@www.example.com:123/forum/questions/";
        urls::url_view u = urls::parse_uri( s ).value();
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
        urls::string_view s = "https://john.doe@www.example.com:123/forum/questions/";
        urls::url_view u = urls::parse_uri( s ).value();
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
        urls::string_view s = "https://john.doe@192.168.2.1:123/forum/questions/";
        urls::url_view u = urls::parse_uri( s ).value();
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
        urls::string_view s = "https://www.boost.org/users/download/";
        urls::url_view u = urls::parse_uri( s ).value();
        switch (u.host_type())
        {
        case urls::host_type::name:
            // resolve name
        case urls::host_type::ipv4:
        case urls::host_type::ipv6:
        case urls::host_type::ipvfuture:
            // connect to ip
            break;
        case urls::host_type::none:
            // handle empty host URL
            break;
        }
        //]
    }
    {
        //[snippet_parsing_authority_11
        urls::string_view s = "https://john.doe:123456@www.somehost.com/forum/questions/";
        urls::url_view u = urls::parse_uri( s ).value();
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
        urls::string_view s = "www.example.com:80";
        urls::authority_view a = urls::parse_authority( s ).value();
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
        urls::string_view s = "user:pass@www.example.com:443";
        urls::authority_view a = urls::parse_authority( s ).value();
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
        urls::string_view s = "https://www.boost.org/doc/libs/";
        urls::url_view    u = urls::parse_uri(s).value();
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
        urls::string_view s = "https://www.boost.org/doc/libs";
        urls::url_view    u = urls::parse_uri(s).value();
        std::cout << u.encoded_segments().size() << " segments\n";
        for (auto seg: u.encoded_segments())
        {
            std::cout << "segment: " << seg << "\n";
        }
        //]
    }

    {
        //[snippet_parsing_path_3
        urls::string_view s = "https://www.boost.org";
        urls::url_view    u = urls::parse_uri(s).value();
        std::cout << u << "\n"
            << "path:             " << u.encoded_path()     << "\n"
            << "encoded segments: " << u.encoded_segments() << "\n"
            << "segments:         " << u.segments()         << "\n";
        //]
    }

    {
        //[snippet_parsing_path_4
        urls::string_view s = "https://www.boost.org//doc///libs";
        urls::url_view    u = urls::parse_uri(s).value();
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
            urls::string_view s = "https://www.boost.org";
            urls::url_view    u = urls::parse_uri(s).value();
            std::cout << u << "\n"
                      << "path:     " << u.encoded_host()            << "\n"
                      << "path:     " << u.encoded_path()            << "\n"
                      << "segments: " << u.encoded_segments().size() << "\n";
            //]
        }
        {
            //[snippet_parsing_path_5_b
            urls::string_view s = "https://www.boost.org/";
            urls::url_view    u = urls::parse_uri(s).value();
            std::cout << u << "\n"
                      << "host:     " << u.encoded_host()            << "\n"
                      << "path:     " << u.encoded_path()            << "\n"
                      << "segments: " << u.encoded_segments().size() << "\n";
            //]
        }
        {
            //[snippet_parsing_path_5_c
            urls::string_view s = "https://www.boost.org//";
            urls::url_view    u = urls::parse_uri(s).value();
            std::cout << u << "\n"
                      << "host:     " << u.encoded_host()            << "\n"
                      << "path:     " << u.encoded_path()            << "\n"
                      << "segments: " << u.encoded_segments().size() << "\n";
            //]
        }
    }

    {
        //[snippet_parsing_path_6
        urls::string_view s = "https://www.boost.org//doc/libs/";
        urls::url_view    u = urls::parse_uri(s).value();
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
        urls::string_view s = "https://doc/libs/";
        urls::url_view    u = urls::parse_uri(s).value();
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
        urls::string_view s = "https://www.boost.org/doc@folder/libs:boost";
        urls::url_view    u = urls::parse_uri(s).value();
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
        urls::string_view s = "/doc/libs";
        urls::segments_encoded_view p = urls::parse_path(s).value();
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
        urls::string_view s = "https://www.example.com/get-customer.php?id=409&name=Joe&individual";
        urls::url_view u = urls::parse_uri(s).value();
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
        urls::string_view s = "https://www.example.com/get-customer.php?key-1=value-1&key-2=&key-3&&=value-2";
        urls::url_view u = urls::parse_uri(s).value();
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
        urls::string_view s = "https://www.example.com/get-customer.php?email=joe@email.com&code=a:2@/!";
        urls::url_view u = urls::parse_uri(s).value();
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
        urls::string_view s = "https://www.example.com/get-customer.php?name=joe";
        urls::url_view u = urls::parse_uri(s).value();
        std::cout << u << "\n"
                  "encoded query: " << u.encoded_query() << "\n";
        //]
    }
    {
        //[snippet_parsing_query_5
        urls::string_view s = "https://www.example.com/get-customer.php";
        urls::url_view u = urls::parse_uri(s).value();
        std::cout << u << "\n"
                  "has query:     " << u.has_query()     << "\n"
                  "encoded query: " << u.encoded_query() << "\n";
        //]
    }
    {
        //[snippet_parsing_query_6
        urls::string_view s = "https://www.example.com/get-customer.php?name=John%20Doe";
        urls::url_view u = urls::parse_uri(s).value();
        std::cout << u << "\n"
                  "has query:     " << u.has_query()     << "\n"
                  "encoded query: " << u.encoded_query() << "\n"
                  "query:         " << u.query()         << "\n";
        //]
    }
    {
        //[snippet_parsing_query_7
        urls::string_view s = "https://www.example.com/get-customer.php?name=John%26Doe";
        urls::url_view u = urls::parse_uri(s).value();
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
        urls::string_view s = "https://www.example.com/index.html#section%202";
        urls::url_view u = urls::parse_uri(s).value();
        std::cout << u << "\n"
                  "has fragment:     " << u.has_fragment()     << "\n"
                  "encoded fragment: " << u.encoded_fragment() << "\n"
                  "fragment:         " << u.fragment()         << "\n";
        //]
    }
    {
        //[snippet_parsing_fragment_2_a
        urls::string_view s = "https://www.example.com/index.html#";
        urls::url_view u = urls::parse_uri(s).value();
        std::cout << u << "\n"
                  "has fragment:     " << u.has_fragment()     << "\n"
                  "encoded fragment: " << u.encoded_fragment() << "\n"
                  "fragment:         " << u.fragment()         << "\n";
        //]
    }
    {
        //[snippet_parsing_fragment_2_b
        urls::string_view s = "https://www.example.com/index.html";
        urls::url_view u = urls::parse_uri(s).value();
        std::cout << u << "\n"
                  "has fragment:     " << u.has_fragment()     << "\n"
                  "encoded fragment: " << u.encoded_fragment() << "\n"
                  "fragment:         " << u.fragment()         << "\n";
        //]
    }
    {
        //[snippet_parsing_fragment_3
        urls::string_view s = "https://www.example.com/index.html#code%20:a@b?c/d";
        urls::url_view u = urls::parse_uri(s).value();
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
        urls::string_view s = "https://www.example.com";
        urls::url_view u = urls::parse_uri(s).value();
        urls::url v(u);
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
        v.set_host("www.my example.com");
        std::cout << v << "\n";
        //]


    }
}

void
grammar_parse()
{
    {
        //[snippet_parse_1
        urls::string_view s = "http:after_scheme";
        urls::scheme_rule r;
        const char* it = s.begin();
        urls::error_code ec;
        if (urls::grammar::parse(it, s.end(), ec, r))
        {
            std::cout << "scheme: " << r.scheme << '\n';
            std::cout << "suffix: " << it << '\n';
        }
        //]
    }

    {
        //[snippet_parse_2
        urls::string_view s = "?key=value#anchor";
        urls::query_part_rule r1;
        urls::fragment_part_rule r2;
        const char* it = s.begin();
        urls::error_code ec;
        if (urls::grammar::parse(it, s.end(), ec, r1))
        {
            if (urls::grammar::parse(it, s.end(), ec, r2))
            {
                std::cout << "query: " << r1.query_part << '\n';
                std::cout << "fragment: " << r2.fragment.str << '\n';
            }
        }
        //]
    }

    {
        //[snippet_parse_3
        urls::string_view s = "?key=value#anchor";
        urls::query_part_rule r1;
        urls::fragment_part_rule r2;
        const char* it = s.begin();
        urls::error_code ec;
        if (urls::grammar::parse(it, s.end(), ec, r1, r2))
        {
            std::cout << "query: " << r1.query_part << '\n';
            std::cout << "fragment: " << r2.fragment.str << '\n';
        }
        //]
    }

    {
        //[snippet_parse_4
        urls::string_view s = "http://www.boost.org";
        urls::uri_rule r;
        urls::error_code ec;
        if (urls::grammar::parse_string(s, ec, r))
        {
            std::cout << "scheme: " << r.scheme_part.scheme << '\n';
            std::cout << "host: " << r.hier_part.authority.host.host_part << '\n';
        }
        //]
    }
}

//[snippet_customization_1
struct lowercase_rule
{
    urls::string_view str;

    friend
    void
    tag_invoke(
        urls::grammar::parse_tag const&,
        char const*& it,
        char const* const end,
        urls::error_code& ec,
        lowercase_rule& t) noexcept
    {
        ec = {};
        char const* begin = it;
        while (it != end && std::islower(*it))
        {
            ++it;
        }
        t.str = urls::string_view(begin, it);
    }
};
//]

void
grammar_customization()
{
    {
        //[snippet_customization_2
        urls::string_view s = "http:somelowercase";
        urls::scheme_rule r1;
        lowercase_rule r2;
        urls::error_code ec;
        if (urls::grammar::parse_string(s, ec, r1, ':', r2))
        {
            std::cout << "scheme: " << r1.scheme << '\n';
            std::cout << "lower:  " << r2.str << '\n';
        }
        //]
    }
}

//[snippet_charset_1
struct digit_chars_t
{
    constexpr
    bool
    operator()( char c ) const noexcept
    {
        return c >= '0' && c <= '9';
    }
};
//]

//[snippet_charset_4
struct CharSet
{
    bool operator()( char c ) const noexcept;

    char const* find_if ( char const* first, char const* last ) const noexcept;
    char const* find_if_not ( char const* first, char const* last ) const noexcept;
};
//]

//[snippet_mutable_string_exemplar
struct MutableString
{
    template< class InputIt >
    void assign( InputIt first, InputIt last );

    template< class InputIt >
    void append( InputIt first, InputIt last );
};
//]


void
grammar_charset()
{
    {
        //[snippet_charset_2
        urls::query_chars_t cs;
        assert(cs('a'));
        assert(cs('='));
        assert(!cs('#'));
        //]
        (void)cs;
    }
    {
        //[snippet_charset_3
        urls::string_view s = "key=the%20value";
        urls::pct_encoded_rule<urls::query_chars_t> r;
        urls::error_code ec;
        if (urls::grammar::parse_string(s, ec, r))
        {
            std::cout << "query:        " << r.s.str << '\n';
            std::cout << "decoded size: " << r.s.decoded_size << '\n';
        }
        //]
    }
}

void
modifying_path()
{
    {
        //[snippet_modifying_path_1
        urls::url_view u = urls::parse_uri("https://www.boost.org").value();
        //]
        BOOST_TEST_NOT(u.is_path_absolute());
        BOOST_TEST_EQ(u.encoded_segments().size(), 0u);
    }

    {
        //[snippet_modifying_path_2
        urls::url_view u = urls::parse_uri("https://www.boost.org/").value();
        //]
        BOOST_TEST(u.is_path_absolute());
        BOOST_TEST_EQ(u.encoded_segments().size(), 0u);
    }

    {
        //[snippet_modifying_path_3
        urls::url u = urls::parse_uri("https://www.boost.org/./a/../b").value();
        u.normalize();
        //]
        BOOST_TEST(u.is_path_absolute());
        BOOST_TEST_EQ(u.string(), "https://www.boost.org/b");
        BOOST_TEST_EQ(u.encoded_segments().size(), 1u);
    }

    {
        //[snippet_modifying_path_4
        // scheme and a relative path
        urls::url_view u = urls::parse_uri("https:path/to/file.txt").value();
        //]
        BOOST_TEST_EQ(u.scheme(), "https");
        BOOST_TEST_NOT(u.has_authority());
        BOOST_TEST_NOT(u.is_path_absolute());
        BOOST_TEST_EQ(u.encoded_segments().size(), 3u);
    }

    {
        //[snippet_modifying_path_5
        // scheme and an absolute path
        urls::url_view u = urls::parse_uri("https:/path/to/file.txt").value();
        //]
        BOOST_TEST_EQ(u.scheme(), "https");
        BOOST_TEST_NOT(u.has_authority());
        BOOST_TEST(u.is_path_absolute());
        BOOST_TEST_EQ(u.encoded_segments().size(), 3u);
    }

    {
        //[snippet_modifying_path_6
        // "//path" will be considered the authority component
        urls::url_view u = urls::parse_uri("https://path/to/file.txt").value();
        //]
        BOOST_TEST_EQ(u.scheme(), "https");
        BOOST_TEST(u.has_authority());
        BOOST_TEST(u.is_path_absolute());
        BOOST_TEST_EQ(u.encoded_segments().size(), 2u);
    }

    {
        //[snippet_modifying_path_7
        // only a relative path
        urls::url_view u = urls::parse_uri_reference("path-to/file.txt").value();
        //]
        BOOST_TEST_NOT(u.has_scheme());
        BOOST_TEST_NOT(u.has_authority());
        BOOST_TEST_NOT(u.is_path_absolute());
        BOOST_TEST_EQ(u.encoded_segments().size(), 2u);
    }

    {
        //[snippet_modifying_path_8
        // "path:" will be considered the scheme component
        // instead of a substring of the first segment
        urls::url_view u = urls::parse_uri_reference("path:to/file.txt").value();
        //]
        BOOST_TEST(u.has_scheme());
        BOOST_TEST_NOT(u.has_authority());
        BOOST_TEST_NOT(u.is_path_absolute());
        BOOST_TEST_EQ(u.encoded_segments().size(), 2u);
    }

    {
        //[snippet_modifying_path_9
        // "path" should not become the authority component
        urls::url u = urls::parse_uri("https:path/to/file.txt").value();
        u.set_encoded_path("//path/to/file.txt");
        //]
        BOOST_TEST_EQ(u.scheme(), "https");
        BOOST_TEST_NOT(u.has_authority());
        BOOST_TEST(u.is_path_absolute());
        BOOST_TEST_EQ(u.encoded_segments().size(), 4u);
    }

    {
        //[snippet_modifying_path_10
        // "path:to" should not make the scheme become "path:"
        urls::url u = urls::parse_uri_reference("path-to/file.txt").value();
        u.set_encoded_path("path:to/file.txt");
        //]
        BOOST_TEST_NOT(u.has_scheme());
        BOOST_TEST_NOT(u.has_authority());
        BOOST_TEST_NOT(u.is_path_absolute());
        BOOST_TEST_EQ(u.encoded_segments().size(), 2u);
    }

    {
        //[snippet_modifying_path_11
        // should not insert as "pathto/file.txt"
        urls::url u = urls::parse_uri_reference("to/file.txt").value();
        urls::segments segs = u.segments();
        segs.insert(segs.begin(), "path");
        //]
        BOOST_TEST_NOT(u.has_scheme());
        BOOST_TEST_NOT(u.has_authority());
        BOOST_TEST_NOT(u.is_path_absolute());
        BOOST_TEST_EQ(u.encoded_segments().size(), 3u);
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
        grammar_parse();
        grammar_customization();
        grammar_charset();
        modifying_path();

        BOOST_TEST_PASS();
    }
};

TEST_SUITE(snippets_test, "boost.url.snippets");

} // urls
} // boost
