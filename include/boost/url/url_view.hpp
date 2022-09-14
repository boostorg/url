//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/url
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
class BOOST_SYMBOL_VISIBLE url_view
    : public url_view_base
{
    friend std::hash<url_view>;
    friend class url_view_base;
    friend class params_base;
    friend class params_encoded_base;

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

        Any param views, segment views, or
        iterators which reference this object are
        invalidated. The ownership and lifetime
        of the underlying character buffer
        remains unchanged.
    */
    BOOST_URL_DECL
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
    BOOST_URL_DECL
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
    BOOST_URL_DECL
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
    BOOST_URL_DECL
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
    BOOST_URL_DECL
    url_view&
    operator=(url_view_base const&) noexcept;

    //--------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------

    /** Return the maximum number of characters possible

        This represents the largest number of
        characters that are possible in a url,
        not including any null terminator.

        @par Complexity
        Constant.

        @par Exception Safety
        Throws nothing.
    */
    static
    constexpr
    std::size_t
    max_size() noexcept
    {
        return BOOST_URL_MAX_SIZE;
    }
};

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
