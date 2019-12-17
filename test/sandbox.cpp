//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#include "test_suite.hpp"

namespace boost {
namespace url {

class sandbox_test
{
public:
    void
    doSandbox()
    {
        //auto u = parse("https://www.boost.org/index.html");
        /*

        value v("https://www.boost.org/index.html");

        - Return the decoded string
        - Return the encoded string as std::string
        - Return the encoded string without dynamic alloc
        - Set the value using encoded string
        - Set the value using plain string
        - Set the value using safe string (encoded == plain)

        // Return the hostname as a plain string
        string_view
        hostname() const noexcept;

        // Return the encoded hostname as a string
        template<class Alloc = std::allocator<char>>
        std::basic_string<char, Alloc>
        encoded_hostname( Alloc const& = Alloc{} );

        // Set the hostname to the encoded string
        v.hostname(encoded( "boost%2Eorg" ));

        // Set the hostname to the plain string
        v.hostname(plain( "boost%2Eorg" ));

        // Set the hostname to the plain string without validating
        v.hostname(unchecked( "boost%2Eorg" ));

        // Set the hostname to the string, detecting its encoding
        v.hostname( "boost%2Eorg" );
        */
    }

    void
    run()
    {
        doSandbox();
    }
};

TEST_SUITE(sandbox_test, "boost.url.sandbox");

} // url
} // boost
