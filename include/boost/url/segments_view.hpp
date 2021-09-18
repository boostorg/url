//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_VIEW_HPP
#define BOOST_URL_SEGMENTS_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/string.hpp>
#include <boost/url/detail/pct_encoding.hpp>
#include <boost/url/rfc/pct_encoded_bnf.hpp>
#include <iosfwd>
#include <utility>

namespace boost {
namespace urls {

/** A BidirectionalRange view of read-only path segments
*/
class segments_view
{
    string_view s_;
    std::size_t n_;

    friend class url;
    friend class url_view;

    segments_view(
        string_view s,
        std::size_t n)
        : s_(s)
        , n_(n)
    {
    }

public:
    class value_type;
    class iterator;

    segments_view(
        segments_view const&) = default;
    segments_view& operator=(
        segments_view const&) = default;

    BOOST_URL_DECL
    segments_view() noexcept;

    /** Return true if the range contains no elements
    */
    bool
    empty() const noexcept
    {
        return n_ == 0;
    }

    /** Return the number of elements in the range
    */
    std::size_t
    size() const noexcept
    {
        return n_;
    }

    /** Return the complete encoded path
    */
    string_view
    encoded_path() const noexcept
    {
        return s_;
    }

    /** Return an iterator to the beginning of the range
    */
    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    /** Return an iterator to the end of the range
    */
    BOOST_URL_DECL
    iterator
    end() const noexcept;

    BOOST_URL_DECL friend segments_view
        parse_path_abempty(string_view s,
            error_code& ec) noexcept;
    BOOST_URL_DECL friend segments_view
        parse_path_abempty(string_view s);
    BOOST_URL_DECL friend segments_view
        parse_path_absolute(string_view s,
            error_code& ec) noexcept;
    BOOST_URL_DECL friend segments_view
        parse_path_absolute(string_view s);
    BOOST_URL_DECL friend segments_view
        parse_path_noscheme(string_view s,
            error_code& ec) noexcept;
    BOOST_URL_DECL friend segments_view
        parse_path_noscheme(string_view s);
    BOOST_URL_DECL friend segments_view
        parse_path_rootless(string_view s,
            error_code& ec) noexcept;
    BOOST_URL_DECL friend segments_view
        parse_path_rootless(string_view s);
};

//----------------------------------------------------------

/** The value type for segment views
*/
class segments_view::value_type
{
    pct_encoded_str s_;

    friend class iterator;
    friend class segments_view;

public:
    value_type() = default;
    value_type& operator=(
        value_type const&) = default;
    value_type(
        value_type const&) = default;

    /** Return the segment

        This function returns the key as
        a percent-encoded string.

        @see key
    */
    string_view
    encoded_segment() const noexcept
    {
        return s_.str;
    }

    /** Return the segment

        This function returns the segment as a
        string with percent-decoding applied.

        @par Exception Safety

        Strong guarantee.
        Calls to allocate may throw.

        @param a An optional allocator the returned
        string will use. If this parameter is omitted,
        the default allocator is used, and the return
        type of the function becomes `std::string`.

        @return A `std::basic_string` using the
        specified allocator.
    */
    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    segment(Allocator const& a = {}) const
    {
        return detail::pct_decode_unchecked(
            s_.str, s_.decoded_size, {}, a);
    }

    value_type const*
    operator->() const noexcept
    {
        return this;
    }

    operator
    std::string() const
    {
        return segment();
    }
};

//----------------------------------------------------------

/** Format the encoded path to an output stream
*/
BOOST_URL_DECL
std::ostream&
operator<<(
    std::ostream& os,
    segments_view const& pv);

//----------------------------------------------------------

/** Return a path view from a parsed string, using path-abempty bnf

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise sets the error and returns an empty range.

    @par BNF
    @code
    path-abempty  = *( "/" segment )
    @endcode

    @par Exception Safety
    No-throw guarantee.

    @param s The string to parse
    @param ec Set to the error, if any occurred

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path,
        @ref parse_path_absolute,
        @ref parse_path_noscheme,
        @ref parse_path_rootless.
*/
BOOST_URL_DECL
segments_view
parse_path_abempty(
    string_view s,
    error_code& ec) noexcept;

/** Return a path view from a parsed string, using path-abempty bnf

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise throws an exception.

    @par BNF
    @code
    path-abempty  = *( "/" segment )
    @endcode

    @throw system_error Thrown on error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path,
        @ref parse_path_absolute,
        @ref parse_path_noscheme,
        @ref parse_path_rootless.
*/
BOOST_URL_DECL
segments_view
parse_path_abempty(
    string_view s);

/** Return a path view from a parsed string, using path-abempty bnf

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise sets the error and returns an empty range.

    @par BNF
    @code
    path-abempty  = *( "/" segment )
    @endcode

    @par Exception Safety
    No-throw guarantee.

    @param s The string to parse
    @param ec Set to the error, if any occurred

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path,
        @ref parse_path_absolute,
        @ref parse_path_noscheme,
        @ref parse_path_rootless.
*/
inline
segments_view
parse_path(
    string_view s,
    error_code& ec) noexcept
{
    return parse_path_abempty(s, ec);
}

/** Return a path view from a parsed string, using path-abempty bnf

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise throws an exception.

    @par BNF
    @code
    path-abempty  = *( "/" segment )
    @endcode

    @throw system_error Thrown on error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path_abempty,
        @ref parse_path_absolute,
        @ref parse_path_noscheme,
        @ref parse_path_rootless.
*/
inline
segments_view
parse_path(
    string_view s)
{
    return parse_path_abempty(s);
}

/** Return a path view from a parsed string, using path-absolute bnf

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise sets the error and returns an empty range.

    @par BNF
    @code
    path-absolute = "/" [ segment-nz *( "/" segment ) ]
    @endcode

    @par Exception Safety
    No-throw guarantee.

    @param s The string to parse
    @param ec Set to the error, if any occurred

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path,
        @ref parse_path_abempty,
        @ref parse_path_noscheme,
        @ref parse_path_rootless.
*/
BOOST_URL_DECL
segments_view
parse_path_absolute(
    string_view s,
    error_code& ec) noexcept;

/** Return a path view from a parsed string, using path-absolute bnf

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise throws an exception.

    @par BNF
    @code
    path-absolute = "/" [ segment-nz *( "/" segment ) ]
    @endcode

    @throw system_error Thrown on error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path,
        @ref parse_path_abempty,
        @ref parse_path_noscheme,
        @ref parse_path_rootless.
*/
BOOST_URL_DECL
segments_view
parse_path_absolute(
    string_view s);

/** Return a path view from a parsed string, using path-noscheme bnf

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise sets the error and returns an empty range.

    @par BNF
    @code
    path-noscheme = segment-nz-nc *( "/" segment )

    segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
                    ; non-zero-length segment without any colon ":"
    @endcode

    @par Exception Safety
    No-throw guarantee.

    @param s The string to parse
    @param ec Set to the error, if any occurred

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path,
        @ref parse_path_abempty,
        @ref parse_path_absolute,
        @ref parse_path_rootless.
*/
BOOST_URL_DECL
segments_view
parse_path_noscheme(
    string_view s,
    error_code& ec) noexcept;

/** Return a path view from a parsed string, using path-noscheme bnf

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise throws an exception.

    @par BNF
    @code
    path-noscheme = segment-nz-nc *( "/" segment )

    segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
                    ; non-zero-length segment without any colon ":"
    @endcode

    @throw system_error Thrown on error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path,
        @ref parse_path_abempty,
        @ref parse_path_absolute,
        @ref parse_path_rootless.
*/
BOOST_URL_DECL
segments_view
parse_path_noscheme(
    string_view s);

/** Return a path view from a parsed string, using path_rootless bnf

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise sets the error and returns an empty range.

    @par BNF
    @code
    path-rootless = segment-nz *( "/" segment )

    segment-nz    = 1*pchar
    @endcode

    @par Exception Safety
    No-throw guarantee.

    @param s The string to parse
    @param ec Set to the error, if any occurred

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path,
        @ref parse_path_abempty,
        @ref parse_path_absolute,
        @ref parse_path_noscheme.
*/
BOOST_URL_DECL
segments_view
parse_path_rootless(
    string_view s,
    error_code& ec) noexcept;

/** Return a path view from a parsed string, using path_rootless bnf

    This function parses the string and returns the
    corresponding path object if the string is valid,
    otherwise sets the error and returns an empty range.

    @par BNF
    @code
    path-rootless = segment-nz *( "/" segment )

    segment-nz    = 1*pchar
    @endcode

    @throw system_error Thrown on error

    @param s The string to parse

    @par Specification
    @li <a href="https://datatracker.ietf.org/doc/html/rfc3986#section-3.3">
        3.3. Path (rfc3986)</a>

    @see
        @ref parse_path,
        @ref parse_path_abempty,
        @ref parse_path_absolute,
        @ref parse_path_noscheme.
*/
BOOST_URL_DECL
segments_view
parse_path_rootless(
    string_view s);

} // urls
} // boost

#include <boost/url/impl/segments_view.hpp>

#endif
