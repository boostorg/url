//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_CONST_ENCODED_VIEW_HPP
#define BOOST_URL_PARAMS_CONST_ENCODED_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/params_encoded_base.hpp>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view_base;
#endif

/** A forward range of read-only encoded query parameters.

    Objects of this type represent an iterable
    range of query parameters, where each parameter
    is represented by percent-encoded key and value
    strings.

    Dereferenced iterators return a structure of
    string views into the underlying character
    buffer.

    Ownership of the underlying characters is
    not transferred; the character buffer used
    to construct the container must remain
    valid for as long as the container exists.

    The container models a multi-map. That is,
    duplicate keys are possible.

    A view of encoded parameters in a URL's query
    can be obtained by calling
        @ref url_view::encoded_params.
    Alternatively, to obtain encoded parameters
    from a query stored in a string call the
    parsing function (see below).

    @par Example

    A query parameter string is parsed into
    encoded params_view, then each parameter is
    printed to standard output:

    @code
    url_view u( "?cust=John&id=42&last_invoice=1001" );

    for( auto e : u.encoded_params() )
        std::cout << "key = " << e.key << ", value = " << e.value << std::endl;
    @endcode

    @par Complexity
    Iterator increment runs in linear time on
    the size of the parameter.
    All other operations run in constant time.
    No operations allocate memory.
*/
class params_const_encoded_view
    : public params_encoded_base
{
    friend class url_view_base;
    friend class params_encoded_view;

    params_const_encoded_view(
        url_view_base const& u) noexcept
        : params_encoded_base(u)
    {
    }

public:
    /** Constructor

        After the copy both views will point to
        the same character buffer.

        Ownership is not transferred; the caller
        is responsible for ensuring the lifetime
        of the character buffer extends until
        it is no longer referenced.

        @par Complexity
            Constant

        @par Exception Safety
            Throws nothing

    */
    params_const_encoded_view(
        params_const_encoded_view const&) = default;

    /** Assignment

        After the assignment, both views will point to
        the same underlying object.

        Ownership is not transferred; the caller
        is responsible for ensuring the lifetime
        of the character buffer extends until
        it is no longer referenced.

        @par Complexity
            Constant

        @par Exception Safety
            Throws nothing

    */
    params_const_encoded_view&
    operator=(params_const_encoded_view const&) & = default;
};

} // urls
} // boost

#endif
