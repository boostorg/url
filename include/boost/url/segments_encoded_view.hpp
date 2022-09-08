//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_ENCODED_VIEW_HPP
#define BOOST_URL_SEGMENTS_ENCODED_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/segments_encoded_base.hpp>
#include <boost/url/segments_view.hpp>
#include <boost/url/string_view.hpp>
#include <iosfwd>
#include <utility>

namespace boost {
namespace urls {

/** A view representing path segments in a URL

    Objects of this type are used to interpret
    the path as a bidirectional view of segment
    strings.

    The view does not retain ownership of the
    elements and instead references the original
    character buffer. The caller is responsible
    for ensuring that the lifetime of the buffer
    extends until it is no longer referenced.

    @par Example
    @code
    url_view u( "/path/to/file.txt" );

    segments_encoded_view ps = u.encoded_segments();

    assert( ps.buffer().data() == u.string().data() );
    @endcode

    Strings produced when elements are returned
    have type @ref param_pct_view and represent
    encoded strings. Strings passed to member
    functions may contain percent escapes, and
    throw exceptions on invalid inputs.

    @par Iterator Invalidation
    Changes to the underlying character buffer
    can invalidate iterators which reference it.

    @see
        @ref segments_view,
        @ref segments_encoded_ref,
        @ref segments_ref.
*/
class segments_encoded_view
    : public segments_encoded_base
{
    friend class url_view_base;
    friend class segments_encoded_ref;

    segments_encoded_view(
        detail::path_ref const& ref) noexcept;

public:
    /** Constructor

        After construction, both views will
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
    segments_encoded_view(
        segments_encoded_view const&) noexcept = default;

    /** Assignment (deleted)
    */
    segments_encoded_view&
    operator=(
        segments_encoded_view const&) = delete;

    /** Conversion

        This conversion returns a new view which
        references the same underlying character
        buffer, and whose iterators and members
        return ordinary strings with decoding
        applied to any percent escapes.

        Ownership is not transferred; the caller
        is responsible for ensuring the lifetime
        of the buffer extends until it is no
        longer referenced.

        @par Example
        @code
        segments_view ps = parse_path( "/path/to/file.txt" ).value();
        @endcode

        @par Postconditions
        @code
        segments_view( *this ).buffer().data() == this->buffer().data()
        @endcode

        @par Complexity
        Constant

        @par Exception Safety
        Throws nothing
    */
    BOOST_URL_DECL
    operator
    segments_view() const noexcept;

    //--------------------------------------------

    BOOST_URL_DECL
    friend
        result<segments_encoded_view>
        parse_path(string_view s) noexcept;
};

} // urls
} // boost

#endif
