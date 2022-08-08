//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_URL_VIEW_HPP
#define BOOST_URL_URL_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/url_view_base.hpp>
#include <utility>

namespace boost {
namespace urls {

/** A non-owning reference to a valid URL 

    Objects of this type represent valid URL
    strings constructed from a parsed, external
    character buffer whose storage is managed
    by the caller. That is, it acts like a
    @ref string_view in terms of ownership.
    The caller is responsible for ensuring
    that the lifetime of the underlying
    character buffer extends until it is no
    longer referenced.

    @par Example 1
    Construction from a string parses the input
    as a <em>URI-reference</em> and throws an
    exception on error. Upon success, the
    constructed object points to the passed
    character buffer; ownership is not
    transferred.
    @code
    url_view u( "https://www.example.com/index.htm?text=none#a1" );
    @endcode

    @par Example 2
    Parsing functions like @ref parse_uri_reference
    return a @ref result containing either a valid
    @ref url_view upon succcess, otherwise they
    contain an error. The error can be converted to
    an exception by the caller if desired:
    @code
    result< url_view > rv = parse_uri_reference( "https://www.example.com/index.htm?text=none#a1" );
    @endcode

    @par BNF
    @code
    URI-reference = URI / relative-ref

    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
    @endcode

    @par Specification
    @li <a href="https://tools.ietf.org/html/rfc3986"
        >Uniform Resource Identifier (URI): Generic Syntax (rfc3986)</a>

    @see
        @ref parse_absolute_uri,
        @ref parse_origin_form,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref parse_uri_reference.
*/
class BOOST_URL_DECL url_view
    : public url_view_base
{
    friend std::hash<url_view>;
    friend class url_view_base;

#ifndef BOOST_URL_DOCS
    // VFALCO docca emits this erroneously
    friend struct detail::url_impl;
#endif

    using url_view_base::digest;

    explicit
    url_view(
        detail::url_impl const& impl) noexcept
        : url_view_base(impl)
    {
    }

public:
    //--------------------------------------------
    //
    // Special Members
    //
    //--------------------------------------------

    /** Destructor

        Any params, segments, or iterators
        which reference this object are
        invalidated. The ownership and lifetime
        of the underlying character buffer
        remains unchanged.
    */
    ~url_view();

    /** Constructor

        Default constructed views refer to
        a string with zero length, which
        always remains valid. This matches
        the grammar for a relative-ref with
        an empty path and no query or
        fragment.

        @par BNF
        @code
        relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
        @endcode

        @par Exception Safety
        Throws nothing.

        @par Specification
        <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.2"
            >4.2. Relative Reference (rfc3986)</a>
    */
    url_view() noexcept;

    /** Construct from a string.

        This function constructs a URL from
        the string `s`, which must contain a
        valid URI or <em>relative-ref</em> or
        else an exception is thrown. Upon
        successful construction, the view
        refers to the characters in the
        buffer pointed to by `s`.
        Ownership is not transferred; The
        caller is responsible for ensuring
        that the lifetime of the buffer
        extends until the view is destroyed.

        @par BNF
        @code
        URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

        relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
        @endcode

        @throw std::invalid_argument parse error.

        @param s The string to parse.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.1"
            >4.1. URI Reference</a>
    */
    explicit
    url_view(string_view s);

    /** Constructor
    */
    url_view(
        url_view const& other) noexcept
        : url_view(static_cast<
            url_view_base const&>(other))
    {
    }

    /** Constructor
    */
    url_view(
        url_view_base const&) noexcept;

    /** Assignment
    */
    url_view&
    operator=(
        url_view const& other) noexcept
    {
        *this = static_cast<
            url_view_base const&>(other);
        return *this;
    }

    /** Assignment
    */
    url_view&
    operator=(url_view_base const&) noexcept;
};

//------------------------------------------------

/** Return a reference to a parsed URL string

    This function parses a string according
    to the grammar below and returns a view
    referencing the passed string upon success,
    else returns an error.
    Ownership of the string is not transferred;
    the caller is responsible for ensuring that
    the lifetime of the character buffer extends
    until the view is no longer being accessed.

    @par Example
    @code
    result< url_view > rv = parse_absolute_uri( "http://example.com/index.htm?id=1" );
    @endcode

    @par BNF
    @code
    absolute-URI    = scheme ":" hier-part [ "?" query ]

    hier-part       = "//" authority path-abempty
                    / path-absolute
                    / path-rootless
                    / path-empty
    @endcode

    @throw std::length_error `s.size() > url_view::max_size`

    @return A @ref result containing a value or an error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.3"
        >4.3. Absolute URI (rfc3986)</a>

    @see
        @ref parse_origin_form,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref parse_uri_reference,
        @ref url_view.
*/
BOOST_URL_DECL
result<url_view>
parse_absolute_uri(
    string_view s);

//------------------------------------------------

/** Return a reference to a parsed URL string

    This function parses a string according
    to the grammar below and returns a view
    referencing the passed string upon success,
    else returns an error.
    Ownership of the string is not transferred;
    the caller is responsible for ensuring that
    the lifetime of the character buffer extends
    until the view is no longer being accessed.

    @par Example
    @code
    result< url_view > = parse_origin_form( "/index.htm?layout=mobile" );
    @endcode

    @par BNF
    @code
    origin-form    = absolute-path [ "?" query ]

    absolute-path = 1*( "/" segment )
    @endcode

    @throw std::length_error `s.size() > url_view::max_size`

    @return A @ref result containing a value or an error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc7230#section-5.3.1"
        >5.3.1.  origin-form (rfc7230)</a>

    @see
        @ref parse_absolute_uri,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref parse_uri_reference,
        @ref url_view.
*/
BOOST_URL_DECL
result<url_view>
parse_origin_form(
    string_view s);

//------------------------------------------------

/** Return a reference to a parsed URL string

    This function parses a string according
    to the grammar below and returns a view
    referencing the passed string upon success,
    else returns an error.
    Ownership of the string is not transferred;
    the caller is responsible for ensuring that
    the lifetime of the character buffer extends
    until the view is no longer being accessed.

    @par Example
    @code
    result< url_view > = parse_relative_ref( "images/dot.gif?v=hide#a" );
    @endcode

    @par BNF
    @code
    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]

    relative-part = "//" authority path-abempty
                  / path-absolute
                  / path-noscheme
                  / path-abempty
                  / path-empty
    @endcode

    @throw std::length_error `s.size() > url_view::max_size`

    @return A @ref result containing a value or an error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.2
        >4.2. Relative Reference (rfc3986)</a>
    @li <a href="https://www.rfc-editor.org/errata/eid5428"
        >Errata ID: 5428 (rfc3986)</a>

    @see
        @ref parse_absolute_uri,
        @ref parse_origin_form,
        @ref parse_uri,
        @ref parse_uri_reference,
        @ref url_view.
*/
BOOST_URL_DECL
result<url_view>
parse_relative_ref(
    string_view s);

//------------------------------------------------

/** Return a reference to a parsed URL string

    This function parses a string according
    to the grammar below and returns a view
    referencing the passed string upon success,
    else returns an error.
    Ownership of the string is not transferred;
    the caller is responsible for ensuring that
    the lifetime of the character buffer extends
    until the view is no longer being accessed.

    @par Example
    @code
    result< url_view > = parse_uri( "https://www.example.com/index.htm?id=guest#s1" );
    @endcode

    @par BNF
    @code
    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

    hier-part     = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty
    @endcode

    @throw std::length_error `s.size() > url_view::max_size`

    @return A @ref result containing a value or an error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3"
        >3. Syntax Components (rfc3986)</a>

    @see
        @ref parse_absolute_uri,
        @ref parse_origin_form,
        @ref parse_relative_ref,
        @ref parse_uri_reference,
        @ref url_view.
*/
BOOST_URL_DECL
result<url_view>
parse_uri(
    string_view s);

//------------------------------------------------

/** Return a reference to a parsed URL string

    This function parses a string according
    to the grammar below and returns a view
    referencing the passed string upon success,
    else returns an error.
    Ownership of the string is not transferred;
    the caller is responsible for ensuring that
    the lifetime of the character buffer extends
    until the view is no longer being accessed.

    @par Example
    @code
    result< url_view > = parse_uri_reference( "ws://echo.example.com/?name=boost#demo" );
    @endcode

    @par BNF
    @code
    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

    hier-part     = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty

    URI-reference = URI / relative-ref

    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]

    relative-part = "//" authority path-abempty
                  / path-absolute
                  / path-noscheme
                  / path-abempty
                  / path-empty
    @endcode

    @throw std::length_error `s.size() > url_view::max_size`

    @return A @ref result containing a value or an error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.1"
        >4.1. URI Reference (rfc3986)</a>
    @li <a href="https://www.rfc-editor.org/errata/eid5428"
        >Errata ID: 5428 (rfc3986)</a>

    @see
        @ref parse_absolute_uri,
        @ref parse_origin_form,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref url_view.
*/
BOOST_URL_DECL
result<url_view>
parse_uri_reference(
    string_view s);

} // urls
} // boost

//------------------------------------------------

// std::hash specialization
#ifndef BOOST_URL_DOCS
namespace std {
template<>
struct hash< ::boost::urls::url_view >
{
    hash() = default;
    hash(hash const&) = default;
    hash& operator=(hash const&) = default;

    explicit
    hash(std::size_t salt) noexcept
        : salt_(salt)
    {
    }

    std::size_t
    operator()(::boost::urls::url_view const& u) const noexcept
    {
        return u.digest(salt_);
    }

private:
    std::size_t salt_ = 0;
};
} // std
#endif

#endif
