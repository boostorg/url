//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_VIEW_HPP
#define BOOST_URL_SEGMENTS_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/segments_base.hpp>
#include <cstddef>
#include <iosfwd>

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

    segments_view ps = u.segments();

    assert( ps.buffer().data() == u.string().data() );
    @endcode

    The strings produced when iterators are
    dereferenced belong to the iterator and
    become invalidated when that particular
    iterator is incremented, decremented,
    or destroyed.
    Any percent-escapes in returned strings
    are decoded first.

    @par Iterator Invalidation
    Changes to the underlying character buffer
    can invalidate iterators which reference it.

    @see
        @ref segments_encoded_view,
        @ref segments_encoded_ref,
        @ref segments_ref.
*/
class segments_view
    : public segments_base
{
    friend class url_view_base;
    friend class segments_encoded_view;
    friend class segments_ref;

    segments_view(
        detail::path_ref const& ref) noexcept
        : segments_base(ref)
    {
    }

public:
    /** Constructor

        After construction, both views will
        reference the same underlying character
        buffer.

        Ownership is not transferred; the caller
        is responsible for ensuring the lifetime
        of the buffer extends until it is no
        longer referenced.

        @par Postconditions
        @code
        this->buffer().data() == other.buffer().data()
        @endcode

        @par Complexity
        Constant

        @par Exception Safety
        Throws nothing
    */
    segments_view(
        segments_view const& other) = default;

    /** Assignment (deleted)
    */
    segments_view&
    operator=(segments_view const& other) = delete;
};

} // urls
} // boost

#include <boost/url/impl/segments_view.hpp>

#endif
