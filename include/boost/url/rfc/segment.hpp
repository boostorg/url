//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_SEGMENT_HPP
#define BOOST_URL_RFC_SEGMENT_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/rfc/pct_encoding.hpp>
#include <boost/url/bnf/repeat.hpp>

namespace boost {
namespace urls {
namespace rfc {

/** BNF for segment

    @par BNF
    @code
    segment       = *pchar

    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
    pct-encoded   = "%" HEXDIG HEXDIG
    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
class segment
{
public:
    class value_type
    {
        friend class segment;

        string_view s_;

    public:
        string_view
        encoded_str() const noexcept
        {
            return s_;
        }

        template<
            class Allocator =
                std::allocator<char>>
        string_type<Allocator>
        str(
            Allocator const& a = {}) const
        {
            return pct_decode_unchecked(
                s_, a);
        }
    };

    value_type const&
    operator*() const noexcept
    {
        return v_;
    }

    value_type const*
    operator->() const noexcept
    {
        return &v_;
    }

    BOOST_URL_DECL
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec);

private:
    value_type v_;
};

/** BNF for segment which is not empty

    @par BNF
    @code
    segment-nz    = 1*pchar

    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
    pct-encoded   = "%" HEXDIG HEXDIG
    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
class segment_nz
{
public:
    class value_type
    {
        friend class segment_nz;

        string_view s_;

    public:
        string_view
        encoded_str() const noexcept
        {
            return s_;
        }

        template<
            class Allocator =
                std::allocator<char>>
        string_type<Allocator>
        str(
            Allocator const& a = {}) const
        {
            return pct_decode_unchecked(
                s_, a);
        }
    };

    value_type const&
    operator*() const noexcept
    {
        return v_;
    }

    value_type const*
    operator->() const noexcept
    {
        return &v_;
    }

    BOOST_URL_DECL
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec);

private:
    value_type v_;
};

/** BNF for segment which is not empty and has no colon

    @par BNF
    @code
    segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
                  ; non-zero-length segment without any colon ":"

    pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
    pct-encoded   = "%" HEXDIG HEXDIG
    sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
                  / "*" / "+" / "," / ";" / "="
    @endcode

    @see
        https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
*/
class segment_nz_nc
{
public:
    class value_type
    {
        friend class segment_nz_nc;

        string_view s_;

    public:
        string_view
        encoded_str() const noexcept
        {
            return s_;
        }

        template<
            class Allocator =
                std::allocator<char>>
        string_type<Allocator>
        str(
            Allocator const& a = {}) const
        {
            return pct_decode_unchecked(
                s_, a);
        }
    };

    value_type const&
    operator*() const noexcept
    {
        return v_;
    }

    value_type const*
    operator->() const noexcept
    {
        return &v_;
    }

    BOOST_URL_DECL
    char const*
    parse(
        char const* const start,
        char const* const end,
        error_code& ec);

private:
    value_type v_;
};

} // rfc
} // urls
} // boost

#endif
