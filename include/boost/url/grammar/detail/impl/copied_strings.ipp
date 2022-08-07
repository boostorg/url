//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/http_proto
//

#ifndef BOOST_URL_GRAMMAR_DETAIL_IMPL_COPIED_STRINGS_IPP
#define BOOST_URL_GRAMMAR_DETAIL_IMPL_COPIED_STRINGS_IPP

#include <boost/url/grammar/detail/copied_strings.hpp>
#include <functional>

namespace boost {
namespace urls {
namespace grammar {
namespace detail {

bool
copied_strings_base::
is_overlapping(
    string_view s) const noexcept
{
    auto const b1 = s_.data();
    auto const e1 = b1 + s_.size();
    auto const b2 = s.data();
    auto const e2 = b2 + s.size();
    auto const less_equal =
        std::less_equal<char const*>();
    if(less_equal(e1, b2))
        return false;
    if(less_equal(e2, b1))
        return false;
    return true;
}

copied_strings_base::
~copied_strings_base()
{
    while(dynamic_list_)
    {
        auto p = dynamic_list_;
        dynamic_list_ =
            dynamic_list_->next;
        delete[] p;
    }
}

copied_strings_base::
copied_strings_base(
    string_view s,
    char* local_buf,
    std::size_t local_size) noexcept
    : s_(s)
    , local_buf_(local_buf)
    , local_remain_(local_size)
{
}

string_view
copied_strings_base::
maybe_copy(
    string_view s)
{
    if(! is_overlapping(s))
        return s;
    if(local_remain_ >= s.size())
    {
        std::memcpy(local_buf_,
            s.data(), s.size());
        s = string_view(
            local_buf_, s.size());
        local_buf_ += s.size();
        local_remain_ -= s.size();
        return s;
    }
    auto const n =
        sizeof(dynamic_buf);
    auto p = new dynamic_buf[1 +
        sizeof(n) * ((s.size() +
            sizeof(n) - 1) /
                sizeof(n))];
    std::memcpy(p + 1,
        s.data(), s.size());
    s = string_view(reinterpret_cast<
        char const*>(p + 1), s.size());
    p->next = dynamic_list_;
    dynamic_list_ = p;
    return s;
}

} // detail
} // grammar
} // urls
} // boost

#endif
