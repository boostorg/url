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

    params_const_encoded_view p = u.encoded_params();
    @endcode

    Strings produced when elements are returned
    have type @ref param_pct_view and represent
    encoded strings. Strings passed to member
    functions may contain percent escapes, and
    throw exceptions on invalid inputs.

    @par Iterator Invalidation
    Changes to the underlying character buffer
    can invalidate iterators which reference it.
*/
class params_const_encoded_view
    : public params_encoded_base
{
    friend class url_view_base;
    friend class params_encoded_view;

    params_const_encoded_view(
        detail::url_impl const& ui) noexcept
        : params_encoded_base(ui)
    {
    }

public:
    /** Constructor

        After construction both views will
        reference the same character buffer.

        Ownership is not transferred; the caller
        is responsible for ensuring the lifetime
        of the buffer extends until it is no
        longer referenced.

        @par Postconditions
        @code
        this->buffer().data() == other.buffer().data()
        @endcode

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing
    */
    params_const_encoded_view(
        params_const_encoded_view const& other) = default;

    /** Assignment (deleted)
    */
    params_const_encoded_view&
    operator=(
        params_const_encoded_view const&) = delete;
};

} // urls
} // boost

// This is in <boost/url/url_view_base.hpp>
//
// #include <boost/url/impl/params_const_encoded_view.hpp>

#endif
