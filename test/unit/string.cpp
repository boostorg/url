//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

// Test that header file is self-contained.
#include <boost/url/string.hpp>

#include <boost/static_assert.hpp>

namespace boost {
namespace urls {

#ifdef BOOST_URL_HAS_STRING_VIEW
BOOST_STATIC_ASSERT(is_stringlike<
    std::string_view>::value);
#endif

} // urls
} // boost
