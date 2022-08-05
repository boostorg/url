//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/url_view_base.hpp>

#include <boost/url/url_view.hpp>

#include "test_suite.hpp"

namespace boost {
namespace urls {

struct url_view_base_test
{
    void
    run()
    {
        //
        // javadocs
        //

        // size()
        {
            url_view u( "file:///Program%20Files" );
            assert( u.size() == 23 );
        }

        // empty()
        {
            url_view u;
            assert( u.empty() );
        }

        // persist()
        {
            std::shared_ptr< url_view const > sp;
            {
                std::string s( "http://example.com" );
                url_view u( s );                        // u references characters in s

                assert( u.data() == s.data() );         // same buffer

                sp = u.persist();

                assert( sp->data() != s.data() );       // different buffer
                assert( sp->string() == s);             // same contents

                // s is destroyed and thus u
                // becomes invalid, but sp remains valid.
            }
        }

        //----------------------------------------
        //
        // Scheme
        //
        //----------------------------------------

        // has_scheme()
        {
            url_view u( "http://www.example.com" );

            assert( u.has_scheme() );
        }

        // scheme()
        {
            url_view u( "http://www.example.com" );
        
            assert( u.scheme() == "http" );
        }

        // scheme_id()
        {
            url_view u( "wss://www.example.com/crypto.cgi" );

            assert( u.scheme_id() == scheme::wss );
        }

        //----------------------------------------
        //
        // Authority
        //
        //----------------------------------------

        // has_authority()
        {
            url_view u( "http://www.example.com/index.htm" );

            assert( u.has_authority() );
        }

        // encoded_authority()
        {
            url_view u( "file://Network%20Drive/My%2DFiles" );

            assert( u.encoded_authority() == "Network%20Drive" );
        }

        // authority()
        {
            url_view u( "https://www.example.com:8080/index.htm" );

            authority_view a = u.authority();
        }

        // has_userinfo()
        {
            url_view u( "http://jane%2Ddoe:pass@example.com" );

            assert( u.has_userinfo() );
        }

        // encoded_userinfo()
        {
            url_view u( "http://jane%2Ddoe:pass@example.com" );

            assert( u.encoded_userinfo() == "jane%2Ddoe:pass" );
        }

        // userinfo()
        {
            url_view u( "http://jane%2Ddoe:pass@example.com" );

            assert( u.userinfo() == "jane-doe:pass" );
        }

        // encoded_user()
        {
            url_view u( "http://jane%2Ddoe:pass@example.com" );

            assert( u.encoded_user() == "jane%2Ddoe" );
        }

        // user()
        {
            url_view u( "http://jane%2Ddoe:pass@example.com" );

            assert( u.user() == "jane-doe" );
        }

        // has_password()
        {
            url_view u( "http://jane%2Ddoe:pass@example.com" );

            assert( u.has_password() );
        }

        // encoded_password
        {
            url_view u( "http://jane%2Ddoe:pass@example.com" );

            assert( u.encoded_password() == "pass" );
        }

        // password
        {
            url_view u( "http://jane%2Ddoe:pass@example.com" );

            assert( u.password() == "pass" );
        }

        // host_type
        {
            url_view u( "https://192.168.0.1/local.htm" );

            assert( u.host_type() == host_type::ipv4 );
        }

        // encoded_host()
        {
            url_view u( "https://www%2droot.example.com/" );

            assert( u.encoded_host() == "www%2droot.example.com" );
        }

        // encoded_hostname()
        {
            url_view u( "wss://[2001:0db8::0370:7334]/index.htm" );

            assert( u.encoded_hostname() == "2001:0db8::0370:7334" );
        }

        // hostname()
        {
            url_view u( "https://www%2droot.example.com/" );

            assert( u.hostname() == "www-root.example.com" );
        }

        // host
        {
            url_view u( "https://www%2droot.example.com/" );

            assert( u.host() == "www-root.example.com" );
        }

        // ipv4_address()
        {
            url_view u( "http://127.0.0.1/index.htm?user=win95" );

            ipv4_address ip = u.ipv4_address();
        }

        // ipv6_address
        {
            url_view u( "ftp://[::1]" );

            ipv6_address ip = u.ipv6_address();

            assert( ip.is_loopback() );
        }

        // ipvfuture()
        {
            url_view u( "http://[v1fe.d:9]" );

            assert( u.ipvfuture() == "v1fe.d:9" );
        }

        // has_port()
        {
            url_view u( "wss://www.example.com:443" );

            assert( u.has_port() );
        }

        // port()
        {
            url_view u( "http://localhost.com:8080" );

            assert( u.port() == "8080" );
        }

        // port_number()
        {
            url_view u( "http://localhost.com:8080" );

            assert( u.port_number() == 8080 );
        }

        // encoded_host_and_port()
        {
            url_view u( "http://www.example.com:8080/index.htm" );

            assert( u.encoded_host_and_port() == "www.example.com:8080" );
        }

        //----------------------------------------
        //
        // Path
        //
        //----------------------------------------

        // is_path_absolute()
        {
            url_view u( "/path/to/file.txt" );

            assert( u.is_path_absolute() );
        }

        // encoded_path()
        {
            url_view u( "file:///Program%20Files/Games/config.ini" );

            assert( u.encoded_path() == "/Program%20Files/Games/config.ini" );
        }

        // path()
        {
            url_view u( "file:///Program%20Files/Games/config.ini" );

            assert( u.path() == "/Program Files/Games/config.ini" );
        }

        //----------------------------------------
        //
        // Query
        //
        //----------------------------------------

        // has_query()
        {
            url_view u( "/sql?id=42&col=name&page-size=20" );

            assert( u.has_query() );
        }

        // encoded_query()
        {
            url_view u( "/sql?id=42&name=jane%2Ddoe&page+size=20" );

            assert( u.encoded_query() == "id=42&name=jane%2Ddoe&page+size=20" );
        }

        // query
        {
            url_view u( "/sql?id=42&name=jane%2Ddoe&page+size=20" );

            assert( u.query() == "id=42&name=jane-doe&page size=20" );
        }

        //----------------------------------------
        //
        // Fragment
        //
        //----------------------------------------

        // has_fragment()
        {
            url_view u( "http://www.example.com/index.htm#a%2D1" );

            assert( u.has_fragment() );
        }

        // encoded_fragment()
        {
            url_view u( "http://www.example.com/index.htm#a%2D1" );

            assert( u.encoded_fragment() == "a%2D1" );
        }

        // fragment()
        {
            url_view u( "http://www.example.com/index.htm#a%2D1" );

            assert( u.fragment() == "a-1" );
        }
    }
};

TEST_SUITE(
    url_view_base_test,
    "boost.url.url_view_base");

} // urls
} // boost
