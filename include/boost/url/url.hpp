//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_URL_HPP
#define BOOST_URL_URL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/url_base.hpp>
#include <boost/assert.hpp>
#include <utility>

namespace boost {
namespace urls {

/** A modifiable container for a URL.

    Objects of this type hold URLs which may be
    inspected and modified. The derived class
    is responsible for providing storage.

    The underlying string stored in the container
    is always null-terminated.

    @par Exception Safety

    @li Functions marked `noexcept` provide the
    no-throw guarantee, otherwise:

    @li Functions which throw offer the strong
    exception safety guarantee.

    @par BNF
    @code
    URI-reference = URI / relative-ref

    URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

    relative-ref  = relative-part [ "?" query ] [ "#" fragment ]

    absolute-URI  = scheme ":" hier-part [ "?" query ]
    @endcode

    @par Specification
    @li <a href="https://tools.ietf.org/html/rfc3986"
        >Uniform Resource Identifier (URI): Generic Syntax (rfc3986)</a>

    @see
        @ref parse_absolute_uri,
        @ref parse_relative_ref,
        @ref parse_uri,
        @ref parse_uri_reference,
        @ref resolve.
*/
class BOOST_SYMBOL_VISIBLE url
    : public url_base
{
    friend std::hash<url>;

    using url_view_base::digest;

public:
    //--------------------------------------------
    //
    // Special Members
    //
    //--------------------------------------------

    /** Destructor

        Any params, segments, or iterators
        which reference this object are
        invalidated.
    */
    BOOST_URL_DECL
    ~url();

    /** Constructor

        Default constructed urls contain
        a zero-length string. This matches
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
    url() noexcept;

    /** Construct from a string

        This function constructs a URL from
        the string `s`, which must contain a
        valid URI or <em>relative-ref</em> or
        else an exception is thrown.

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
    url(string_view s);

    /** Constructor

        This function performs a move-construction
        from `u`. After the move. the state of `u`
        will be as-if default constructed.

        @par Exception Safety
        Throws nothing.

        @param u The url to construct from.
    */
    BOOST_URL_DECL
    url(url&& u) noexcept;

    /** Constructor

        This function constructs a copy of `u`.

        @param u The url to construct from.
    */
    url(url_view_base const& u)
    {
        copy(u);
    }

    /** Constructor

        This function constructs a copy of `u`.

        @param u The url to construct from.
    */
    url(url const& u)
        : url(static_cast<
            url_view_base const&>(u))
    {
    }

    /** Assignment

        This function performs a move-assignment
        from `u`. After the move. the state of `u`
        will be as-if default constructed.

        @par Exception Safety
        Throws nothing.

        @param u The url to assign from.
    */
    BOOST_URL_DECL
    url&
    operator=(url&& u) noexcept;

    /** Assignment

        This function assigns a copy of `u`
        to `*this`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param u The url to copy.
    */
    url&
    operator=(
        url_view_base const& u)
    {
        copy(u);
        return *this;
    }

    /** Assignment

        This function assigns a copy of `u`
        to `*this`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param u The url to copy.
    */
    url&
    operator=(url const& u)
    {
        return (*this)=static_cast<
            url_view_base const&>(u);
    }

private:
    char* allocate(std::size_t);
    void deallocate(char* s);

    BOOST_URL_DECL void clear_impl() noexcept override;
    BOOST_URL_DECL void reserve_impl(std::size_t) override;
};

//----------------------------------------------------------

/** Resolve a URL reference against a base URL

    This function attempts to resolve a URL
    reference `ref` against the base URL `base`
    which must satisfy the <em>absolute-URI</em>
    grammar.
    This process is defined in detail in
    rfc3986 (see below).
    The result of the resolution is placed
    into `dest`.
    If an error occurs, the contents of
    `dest` is unspecified.

    @par BNF
    @code
    absolute-URI  = scheme ":" hier-part [ "?" query ]
    @endcode

    @par Exception Safety
    Basic guarantee.
    Calls to allocate may throw.

    @param base The base URL to resolve against.

    @param ref The URL reference to resolve.

    @param dest The container where the result
    is written, upon success.

    @param ec Set to the error if any occurred.

    @par Specification
    <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-5"
        >5. Reference Resolution (rfc3986)</a>

    @see
        @ref url,
        @ref url_view.
*/
inline
void
resolve(
    url_view_base const& base,
    url_view_base const& ref,
    url_base& dest,
    error_code& ec)
{
    BOOST_ASSERT(&dest != &base);
    BOOST_ASSERT(&dest != &ref);
    dest.resolve_impl(base, ref, ec);
}

} // urls
} // boost

//------------------------------------------------

// std::hash specialization
#ifndef BOOST_URL_DOCS
namespace std {
template<>
struct hash< ::boost::urls::url >
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
    operator()(::boost::urls::url const& u) const noexcept
    {
        return u.digest(salt_);
    }

private:
    std::size_t salt_ = 0;
};
} // std
#endif

#include <boost/url/impl/params.hpp>
#include <boost/url/impl/params_encoded.hpp>
#include <boost/url/impl/segments.hpp>
#include <boost/url/impl/segments_encoded.hpp>

#endif
