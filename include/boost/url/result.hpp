//
// Copyright (c) 2022 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RESULT_HPP
#define BOOST_URL_RESULT_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error_code.hpp>
#include <boost/system/result.hpp>

namespace boost {
namespace urls {

/** The type of result returned by library functions

    This is an alias template used as the return type
    for functions that can either return a container,
    or fail with an error code. This is a brief
    synopsis of the type:

    @par Declaration
    @code
    template< class T >
    class result
    {
    public:
        // Return true if the result contains an error
        constexpr bool has_error() const noexcept;

        // These return true if the result contains a value
        constexpr bool has_value() const noexcept;
        constexpr explicit operator bool() const noexcept;

        // Return the value or throw an exception if has_value()==false
        constexpr T& value();
        constexpr T& operator*();
        constexpr T const& value() const;
        constexpr T const& operator*() const;

        // Return the error, which is default constructed if has_error()==false
        constexpr error_code error() const noexcept;

        ...more
    @endcode

    @par Usage
    Given the function @ref parse_uri with this signature:
    @code
    result< url_view > parse_uri( string_view s ) noexcept;
    @endcode

    The following statement captures the value in a
    variable upon success, otherwise throws:
    @code
    url_view u = parse_uri( "http://example.com/path/to/file.txt" ).value();
    @endcode

    This statement captures the result in a local
    variable and inspects the error condition:
    @code
    result< url_view > r = parse_uri( "http://example.com/path/to/file.txt" );

    if( r.has_error() )
        std::cout << r.error();
    else
        std::cout << r.value();
    @endcode

    @note For a full synopsis of the type, please see
    the corresponding documentation in Boost.System.

    @tparam T The type of value held by the result.
*/
template<class T>
using result = boost::system::result<T, error_code>;

} // urls
} // boost

#endif
