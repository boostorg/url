//
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_PCT_ENCODED_VIEW_IPP
#define BOOST_URL_IMPL_PCT_ENCODED_VIEW_IPP

#include <boost/url/pct_encoded_view.hpp>
#include <ostream>

namespace boost {
namespace urls {

auto
pct_encoded_view::iterator::
operator*() const noexcept ->
    reference
{
    if (plus_to_space_ &&
        *pos_ == '+')
        return ' ';
    if (*pos_ != '%')
        return *pos_;
    char d0;
    grammar::hexdig_value(pos_[1], d0);
    char d1;
    grammar::hexdig_value(pos_[2], d1);
    return static_cast<char>(
        ((static_cast<
              unsigned char>(d0) << 4) +
         (static_cast<
             unsigned char>(d1))));
}

pct_encoded_view::
pct_encoded_view(
    string_view str,
    pct_decode_opts opt) noexcept
    : p_(str.data())
    , n_(str.size())
    , plus_to_space_(opt.plus_to_space)
{
    error_code ec;
    opt.non_normal_is_error = false;
    dn_ = validate_pct_encoding(str, ec, opt);
    if (ec.failed())
        detail::throw_invalid_argument(
            BOOST_CURRENT_LOCATION);
}

auto
pct_encoded_view::
copy(
    char* dest,
    size_type count,
    size_type pos) const ->
    size_type
{
    if( pos > size() )
        detail::throw_invalid_argument(
            BOOST_CURRENT_LOCATION);
    std::size_t rlen = (std::min)(count, size() - pos);
    auto first = std::next(begin(), pos);
    auto last = std::next(first, rlen);
    while (first != last)
        *dest++ = *first++;
    return rlen;
}

namespace detail
{
template <class T>
int
decoded_strcmp(pct_encoded_view s0, T s1)
{
    auto const n0 = s0.size();
    auto const n1 = s1.size();
    auto n = (std::min)(n0, n1);
    auto it0 = s0.begin();
    auto it1 = s1.begin();
    while (n--)
    {
        const char c0 = *it0++;
        const char c1 = *it1++;
        if (c0 == c1)
            continue;
        return 1 - 2 * (static_cast<unsigned char>(c0)
                      < static_cast<unsigned char>(c1));
    }
    return 1 - (n0 == n1) - 2 * (n0 < n1);
}
}

int
pct_encoded_view::
compare(string_view other) const noexcept
{
    return detail::decoded_strcmp(*this, other);
}

int
pct_encoded_view::
compare(pct_encoded_view other) const noexcept
{
    return detail::decoded_strcmp(*this, other);
}

void
pct_encoded_view::
write(std::ostream& os) const
{
    auto it = begin();
    auto const end_ = end();
    while(it != end_)
        os.put(*it++);
}

} // urls
} // boost

#endif
