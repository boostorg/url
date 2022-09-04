//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_CONST_VIEW_HPP
#define BOOST_URL_PARAMS_CONST_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/params_base.hpp>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view_base;
#endif

/** A view representing query parameters in a URL

    Objects of this type are used to interpret
    the query parameters as a bidirectional view
    of key/value pairs.

    The view does not retain ownership of the
    elements and instead references the original
    character buffer. The caller is responsible
    for ensuring that the lifetime of the buffer
    extends until it is no longer referenced.

    @par Example
    @code
    url_view u( "?first=John&last=Doe" );

    params_const_view p = u.params();
    @endcode

    The strings produced when iterators are
    dereferenced belong to the iterator and
    become invalidated when that particular
    iterator is incremented, decremented,
    or destroyed.
    Any percent-escapes in returned strings
    are decoded first.
    Strings passed to member functions do
    not contain percent-escapes; the percent
    character ('%') is treated as a literal
    percent.

    @par Example
    @code
    url_view u( "?first=John&last=Doe" );

    params_const_view p = u.params();
    @endcode

    @par Iterator Invalidation
    Changes to the underlying url's query
    can invalidate iterators which reference
    the url.
*/
class params_const_view
    : public params_base
{
    friend class params_view;
    friend class url_view_base;

    params_const_view(
        detail::url_impl const& impl) noexcept
        : params_base(impl)
    {
    }

public:
    /** Constructor

        After the copy both views will point to
        the same character buffer.

        Ownership is not transferred; the caller
        is responsible for ensuring the lifetime
        of the buffer extends until it is no
        longer referenced.

        @par Complexity
        Constant

        @par Exception Safety
        Throws nothing
    */
    params_const_view(
        params_const_view const&) = default;
};

} // urls
} // boost

#endif
