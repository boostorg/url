//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CppAlliance/url
//

#include <boost/url/url.hpp>
#include <iostream>

int
main()
{
    namespace urls = boost::urls;

    // Parse a URL
    urls::url u(
        "https://www.boost.org/users?username=Ana%20Trujillo&city=M%C3%A9xico%20D%2EF%2E");

    // Get params
    urls::params params = u.params();

    // Construct sql query from params
    std::string sql = "SELECT * FROM Users WHERE Username = '";
    if (params.contains("username"))
        (*params.find("username")).value.append_to(sql);
    sql += "' AND City = '";
    if (params.contains("city"))
        (*params.find("city")).value.append_to(sql);
    sql += "';";

    std::cout << sql << "\n";

    return EXIT_SUCCESS;
}
