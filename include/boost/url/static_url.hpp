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

// VFALCO This class is for reducing
// the number of template instantiations,
// and keep definitions in the library
#ifndef BOOST_URL_DOCS
class BOOST_SYMBOL_VISIBLE
    static_url_base
    : public url_base
{
protected:
    ~static_url_base() = default;
    BOOST_URL_DECL static_url_base(
        char* buf, std::size_t cap) noexcept;
    BOOST_URL_DECL static_url_base(
        char* buf, std::size_t cap, string_view s);
    BOOST_URL_DECL void clear_impl() noexcept override;
    BOOST_URL_DECL void reserve_impl(std::size_t) override;

    void
    copy(url_view_base const& u)
    {
        this->url_base::copy(u);
    }
};
#endif

//------------------------------------------------

/** A URL with fixed-capacity storage

    This container acts like @ref url,
    except that dynamic allocations are
    never performed. Instead, the capacity
    for the data comes from inline storage.

    @tparam Capacity The maximum capacity
    in bytes. A URL requires bytes equal to
    at least the number of characters plus
    one for the terminating NULL, plus an
    additional number of bytes proportional
    to the larger of the count of path segments
    and the count of query params. Due to
    alignment requirements, the usable
    capacity may be slightly less than
    this number.

    @see
        @ref url,
        @ref url_view.
*/
template<std::size_t Capacity>
class static_url
#ifdef BOOST_URL_DOCS
    : public url_base
#else
    : public static_url_base
#endif
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
