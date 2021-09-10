//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_DETAIL_PCT_ENCODING_HPP
#define BOOST_URL_DETAIL_PCT_ENCODING_HPP

#include <boost/url/pct_encoding_types.hpp>
#include <boost/url/error.hpp>
#include <boost/url/string.hpp>
#include <boost/url/bnf/char_set.hpp>
#include <boost/assert.hpp>
#include <memory>

namespace boost {
namespace urls {
namespace detail {

/** Return true if plain equals a decoded percent-encoded string

    This function compares a plain key to a
    percent-encoded string. The comparison is
    made as if the key were percent-encoded.

    @param plain_key The key to use for comparison.

    @param encoded The percent-encoded string to
        compare to.
*/
BOOST_URL_DECL
bool
key_equal_encoded(
    string_view plain_key,
    pct_encoded_str encoded) noexcept;

//------------------------------------------------

// decode 2 hex chars
// pointed to by `it`
inline
char
decode_hex_octet(
    char const*& it,
    char const* const end,
    error_code& ec) noexcept
{
    using bnf::hexdig_value;
    if(end - it < 2)
    {
        // missing HEXDIG
        ec = error::missing_pct_hexdig;
        return 0;
    }
    auto const d0 =
        hexdig_value(it[0]);
    auto const d1 =
        hexdig_value(it[1]);
    if( d0 == -1 ||
        d1 == -1)
    {
        // expected HEXDIG
        ec = error::bad_pct_hexdig;
        return 0;
    }
    it += 2;
    return static_cast<char>(
        ((static_cast<
            unsigned char>(d0) << 4) +
        (static_cast<
            unsigned char>(d1))));
}

template<class CharSet>
std::size_t
pct_decode_size(
    char const*& it,
    char const* const end,
    error_code& ec,
    CharSet const& cs,
    pct_decode_opts const& opt) noexcept
{
    // can't have % in charset
    BOOST_ASSERT(! cs('%'));
    using bnf::hexdig_value;
    std::size_t n = 0;
    BOOST_ASSERT(! ec.failed());
    while(it != end)
    {
        if( opt.plus_to_space &&
            *it == '+')
        {
            // plus to space
            ++n;
            ++it;
            continue;
        }
        if( ! opt.allow_null &&
            *it == '\0')
        {
            // null in input
            ec = error::illegal_null;
            break;
        }
        if(cs(*it))
        {
            // unreserved
            ++n;
            ++it;
            continue;
        }
        if(*it == '%')
        {
            // escaped
            ++it;
            auto c = decode_hex_octet(
                it, end, ec);
            if(ec.failed())
                break;
            if( ! opt.allow_null &&
                c == '\0')
            {
                // escaped null
                ec = error::illegal_null;
                break;
            }
            if( opt.non_normal_is_error &&
                cs(c))
            {
                // escaped unreserved char
                ec = error::non_canonical;
                break;
            }
            ++n;
            continue;
        }
        if(opt.non_normal_is_error)
        {
            // reserved character in input
            ec = error::illegal_reserved_char;
            break;
        }
        ++n;
        ++it;
    }
    return n;
}

BOOST_URL_DECL
std::size_t
pct_decode_size_unchecked(
    string_view s) noexcept;

BOOST_URL_DECL
void
pct_decode_unchecked(
    char* first,
    char const* last,
    string_view s,
    pct_decode_opts const& opt) noexcept;

template<class Allocator>
string_type<Allocator>
pct_decode_unchecked(
    string_view s,
    std::size_t decoded_size,
    pct_decode_opts const& opt,
    Allocator const& a)
{
    string_type<Allocator> r(a);
    r.resize(decoded_size);
    pct_decode_unchecked(
        &r[0], &r[0] + r.size(),
            s, opt);
    return r;
}

template<class Allocator>
string_type<Allocator>
pct_decode_unchecked(
    string_view s,
    pct_decode_opts const& opt,
    Allocator const& a)
{
    return pct_decode_unchecked(s,
        pct_decode_size_unchecked(s),
            opt, a);
}

template<class CharSet>
bool
parse_pct_encoded(
    char const*& it,
    char const* const end,
    error_code& ec,
    CharSet const& cs,
    std::size_t& needed) noexcept
{
    using namespace bnf;
    std::size_t n = 0;
    char const* it0;
skip:
    it0 = it;
    it = find_if_not(
        it0, end, cs);
    n += it - it0;
    if(it == end)
        goto finish;
    if(*it != '%')
        goto finish;
    for(;;)
    {
        ++it;
        if(it == end)
        {
            // missing HEXDIG
            ec = error::missing_pct_hexdig;
            return false;
        }
        if(hexdig_value(*it) == -1)
        {
            // expected HEXDIG
            ec = error::bad_pct_hexdig;
            return false;
        }
        ++it;
        if(it == end)
        {
            // missing HEXDIG
            ec = error::missing_pct_hexdig;
            return false;
        }
        if(hexdig_value(*it) == -1)
        {
            // expected HEXDIG
            ec = error::bad_pct_hexdig;
            return false;
        }
        ++n;
        ++it;
        if(it == end)
            break;
        if(*it != '%')
            goto skip;
    }
finish:
    needed = n;
    ec = {};
    return true;
}

//------------------------------------------------

template<class CharSet>
char*
pct_encode(
    char* const first,
    char const* last,
    string_view s,
    pct_encode_opts const& opt,
    CharSet const& cs) noexcept
{
    // can't have % in charset
    BOOST_ASSERT(! cs('%'));
    static constexpr char hex[] =
        "0123456789abcdef";
    auto it = first;
    auto p = s.data();
    auto const end = p + s.size();
    if(! opt.space_to_plus)
    {
        while(p != end)
        {
            if(cs(*p))
            {
                BOOST_ASSERT(
                    it != last);
                *it++ = *p++;
                continue;
            }
            BOOST_ASSERT(
                last - it >= 3);
            *it++ = '%';
            auto const u = static_cast<
                unsigned char>(*p);
            *it++ = hex[u>>4];
            *it++ = hex[u&0xf];
            ++p;
        }
        return it;
    }
    // If you are converting space
    // to plus, then space should
    // be in the list of reserved
    // characters!
    BOOST_ASSERT(! cs(' '));
    while(p != end)
    {
        if(cs(*p))
        {
            BOOST_ASSERT(
                it != last);
            *it++ = *p++;
            continue;
        }
        if(*p == ' ')
        {
            BOOST_ASSERT(
                it != last);
            *it++ = '+';
            ++p;
            continue;
        }
        BOOST_ASSERT(
            last - it >= 3);
        *it++ = '%';
        auto const u = static_cast<
            unsigned char>(*p);
        *it++ = hex[u>>4];
        *it++ = hex[u&0xf];
        ++p;
    }
    return it;
}

} // detail
} // urls
} // boost

#endif
