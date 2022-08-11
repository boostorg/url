//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_IMPL_URL_VIEW_IPP
#define BOOST_URL_IMPL_URL_VIEW_IPP

#include <boost/url/url_view.hpp>
#include <boost/url/detail/except.hpp>
#include <boost/url/rfc/absolute_uri_rule.hpp>
#include <boost/url/rfc/relative_ref_rule.hpp>
#include <boost/url/rfc/uri_rule.hpp>
#include <boost/url/rfc/uri_reference_rule.hpp>
#include <boost/url/rfc/origin_form_rule.hpp>
#include <boost/url/grammar/parse.hpp>
#include <boost/assert/source_location.hpp>

namespace boost {
namespace urls {

namespace detail {

url_view
url_impl::
construct() const noexcept
{
    return url_view(*this);
}

} // detail

//------------------------------------------------

url_view::
~url_view()
{
}

url_view::
url_view() noexcept = default;

url_view::
url_view(string_view s)
    : url_view(parse_uri_reference(s).value())
{
}

url_view::
url_view(
    url_view_base const& u) noexcept
    : url_view_base(u.u_)
{
}

url_view&
url_view::
operator=(
    url_view_base const& u) noexcept
{
    u_ = u.u_;
    return *this;
}

//------------------------------------------------
//
// Parsing
//
//------------------------------------------------

result<url_view>
parse_absolute_uri(
    string_view s)
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "too large");
    return grammar::parse(
        s, absolute_uri_rule);
}

result<url_view>
parse_origin_form(
    string_view s)
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "too large");
    return grammar::parse(
        s, origin_form_rule);
}

result<url_view>
parse_relative_ref(
    string_view s)
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "too large");

    return grammar::parse(
        s, relative_ref_rule);
}
result<url_view>
parse_uri(
    string_view s)
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "too large");
    return grammar::parse(s, uri_rule);
}

result<url_view>
parse_uri_reference(
    string_view s)
{
    if(s.size() > url_view::max_size())
        detail::throw_length_error(
            "too large");

    return grammar::parse(
        s, uri_reference_rule);
}

} // urls
} // boost

#endif
