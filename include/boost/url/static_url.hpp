//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_STATIC_URL_HPP
#define BOOST_URL_STATIC_URL_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/url_base.hpp>
#include <boost/align/align_up.hpp>
#include <boost/static_assert.hpp>
#include <cstddef>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
template<std::size_t Capacity>
class static_url;
#endif

// VFALCO This class is for reducing
// the number of template instantiations,
// and keep definitions in the library

/** Common implementation for all static URLs

    This base class is used by the library
    to provide common functionality for
    static URLs. Users should not use this
    class directly. Instead, construct an
    instance of one of the containers
    or call a parsing function.

    @par Containers
        @li @ref url
        @li @ref url_view
        @li @ref static_url

    @par Parsing Functions
        @li @ref parse_absolute_uri
        @li @ref parse_origin_form
        @li @ref parse_relative_ref
        @li @ref parse_uri
        @li @ref parse_uri_reference
*/
class BOOST_SYMBOL_VISIBLE
    static_url_base
    : public url_base
{
    template<std::size_t>
    friend class static_url;

    ~static_url_base() = default;
    BOOST_URL_DECL static_url_base(
        char* buf, std::size_t cap) noexcept;
    BOOST_URL_DECL static_url_base(
        char* buf, std::size_t cap, string_view s);
    BOOST_URL_DECL void clear_impl() noexcept override;
    BOOST_URL_DECL void reserve_impl(std::size_t, op_t&) override;
    BOOST_URL_DECL void cleanup(op_t&) override;

    void
    copy(url_view_base const& u)
    {
        this->url_base::copy(u);
    }

    /** Swap the contents.

        Exchanges the contents of this static_url with another
        static_url. The contents of the urls are swapped in
        linear time with no possibility of exceptions.
        All views, iterators and references are invalidated.

        @par Complexity

        Linear in @ref size() + `other.size()`

        @par Exception Safety

        Throws nothing.

        @param other The static_url to swap with

        If `this == &other`, this function call has no effect.
    */
    BOOST_URL_DECL
    void
    swap(static_url_base& other) noexcept;


};

//------------------------------------------------

/** A URL with fixed-capacity storage

    This container acts like @ref url,
    except that dynamic allocations are
    never performed. Instead, the capacity
    for the data comes from inline storage.

    @par Example
    @code
    static_url< 4000 > u( "https://www.example.com" );
    @endcode

    @tparam Capacity The maximum capacity
    in characters, not including the
    null terminator.

    @see
        @ref url,
        @ref url_view.
*/
template<std::size_t Capacity>
class static_url
    : public static_url_base
{
    char buf_[Capacity + 1];

    friend std::hash<static_url>;
    using url_view_base::digest;

public:
    /** Destructor
    */
    ~static_url() = default;

    /** Constructor
    */
    static_url() noexcept
        : static_url_base(
            buf_, sizeof(buf_))
    {
    }

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

        @throw std::exception parse error.

        @param s The string to parse.

        @par Specification
        @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-4.1"
            >4.1. URI Reference</a>
    */
    explicit
    static_url(string_view s)
        : static_url_base(
            buf_, sizeof(buf_), s)
    {
    }

    /** Constructor

        This constructs a copy of `u`.

        @par Exception Safety
        Strong guarantee.

        @throw std::length_error insufficient space
    */
    static_url(
        static_url const& u)
        : static_url()
    {
        copy(u);
    }

    /** Constructor

        This constructs a copy of `u`.

        @par Exception Safety
        Strong guarantee.

        @throw std::length_error insufficient space
    */
    static_url(
        url_view_base const& u)
        : static_url()
    {
        copy(u);
    }

    /** Assignment

        @par Exception Safety
        Strong guarantee.

        @throw std::length_error insufficient space
    */
    static_url&
    operator=(
        static_url const& u)
    {
        copy(u);
        return *this;
    }

    /** Assignment

        @par Exception Safety
        Strong guarantee.

        @throw std::length_error insufficient space
    */
    static_url&
    operator=(
        url_view_base const& u)
    {
        copy(u);
        return *this;
    }

    /** Exchange the given static_urls.

        Exchanges the contents of the static_url `lhs` with another
        static_url `rhs`. The contents of the urls are swapped in
        linear time with no possibility of exceptions.
        All views, iterators and references are invalidated.

        @par Effects
        @code
        lhs.swap( rhs );
        @endcode

        @par Complexity

        Linear in `lhs.size() + rhs.size()`.

        @par Exception Safety

        Throws nothing

        @param lhs The static_url to exchange.

        @param rhs The static_url to exchange.

        If `&lhs == &rhs`, this function call has no effect.

        @see @ref static_url::swap
    */
    friend
    void
    swap(static_url& lhs, static_url& rhs) noexcept
    {
        lhs.swap(rhs);
    }
};

} // urls
} // boost

//------------------------------------------------

// std::hash specialization
#ifndef BOOST_URL_DOCS
namespace std {
template<std::size_t N>
struct hash< ::boost::urls::static_url<N> >
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
    operator()(::boost::urls::static_url<N> const& u) const noexcept
    {
        return u.digest(salt_);
    }

private:
    std::size_t salt_ = 0;
};
} // std
#endif

#endif
