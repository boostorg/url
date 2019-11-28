//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_DETAIL_BUFFER_HPP
#define BOOST_URL_DETAIL_BUFFER_HPP

#include <boost/url/detail/char_type.hpp>
#include <cstring>
#include <string>
#include <utility>

namespace boost {
namespace url {
namespace detail {

struct any_buffer
{
    virtual
    ~any_buffer() = default;

    virtual
    void
    append(char c) = 0;
    
    virtual
    void
    append(
        char const* data,
        std::size_t size) = 0;

    virtual
    void
    encode(
        string_view s,
        detail::pct_encoding e) = 0;
};

//----------------------------------------------------------

class counting_buffer : public any_buffer
{
    std::size_t n_ = 0;

public:
    using return_type = std::size_t;

    void
    append(char) override
    {
        ++n_;
    }

    void
    append(
        char const*,
        std::size_t size) override
    {
        if(size >
            std::size_t(-1) - n_)
            BOOST_THROW_EXCEPTION(
                std::length_error(
                    "buffer overflow"));
        n_ += size;
    }

    void
    encode(
        string_view s,
        detail::pct_encoding e) override
    {
        auto const size =
            e.encoded_size(s);
        if(size >
            std::size_t(-1) - n_)
            BOOST_THROW_EXCEPTION(
                std::length_error(
                    "buffer overflow"));
        n_ += size;
    }

    return_type
    result()
    {
        return n_;
    }
};

//----------------------------------------------------------

template<class Allocator>
class string_buffer : public any_buffer
{
public:
    using result_type =
        std::basic_string<char,
            std::char_traits<char>,
                Allocator>;

    explicit
    string_buffer(
        Allocator const& a)
        : s_(a)
    {
    }

    void
    prepare(std::size_t size)
    {
        s_.resize(size);
        dest_ = &s_[0];
    }

    void
    append(char c) override
    {
        *dest_++ = c;
    }

    void
    append(
        char const* data,
        std::size_t size) override
    {
        std::memcpy(
            dest_, data, size);
        dest_ += size;
    }

    void
    encode(
        string_view s,
        detail::pct_encoding e) override
    {
        dest_ += e.encode(dest_, s);
    }

    result_type
    result()
    {
        BOOST_ASSERT(s_.empty() ||
            dest_ == &s_.back() + 1);
        return std::move(s_);
    }

private:
    std::basic_string<char,
        std::char_traits<char>,
        Allocator> s_;
    char* dest_;
};

} // detail
} // url
} // boost

#endif
