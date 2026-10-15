//
// Copyright (c) 2022 alandefreitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
//

#include <boost/url.hpp>
#include <cstring>

int main() {
    boost::urls::ipv4_address ip_addr({127, 0, 0, 1});
    if (ip_addr.to_bytes().size() != 4) {
        return 1;
    }

    // The error categories are objects in the
    // library, so the consumer links to them
    boost::system::error_code ec =
        boost::urls::error::bad_pct_hexdig;
    if (std::strcmp(ec.category().name(), "boost.url") != 0) {
        return 1;
    }
    if (ec != boost::urls::grammar::condition::fatal) {
        return 1;
    }
    ec = boost::urls::grammar::error::mismatch;
    if (std::strcmp(ec.category().name(), "boost.url.grammar") != 0) {
        return 1;
    }
    if (boost::urls::parse_uri("not a uri").has_value()) {
        return 1;
    }
    return 0;
}
