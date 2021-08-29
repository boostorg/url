//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_BASIC_URL_HPP
#define BOOST_URL_BASIC_URL_HPP

#include <boost/url/config.hpp>
#include <boost/url/url_base.hpp>
#include <boost/url/detail/storage.hpp>
#include <utility>

namespace boost {
namespace urls {

/** A container for storing a URL.
*/
template<class Allocator>
class basic_url
    : private detail::storage_member<Allocator>
    , public url_base
{
  using storage_t = detail::storage_member<Allocator>;
public:
    basic_url() noexcept
        : detail::storage_member<
            Allocator>(Allocator{})
        , url_base(static_cast<
            detail::storage*>(&this->st_))
    {
    }

    explicit
    basic_url(
        string_view s,
        Allocator const& a = {})
        : storage_t(a)
        , url_base(&this->st_, s)
    {
    }

    explicit
    basic_url(
        Allocator const& a) noexcept
        : storage_t(a)
        , url_base(&this->st_)
    {
    }

    basic_url(basic_url&& other) noexcept
      : storage_t(std::move(other)),
      url_base(&this->st_)
    {
      other.operator=(basic_url());
    }

    basic_url(
      basic_url const& other)
        : storage_t(other)
        , url_base(
          static_cast<detail::storage*>(&this->st_)
            , other.encoded_url())
    {
    }

    basic_url&
    operator=(
      basic_url &&other)
    {
      storage_t::operator=(std::move(other));
      url_base::operator=(url_base(&this->st_));

      other.storage_t::operator=(storage_t(Allocator{}));
      other.url_base::operator=(url_base(&other.st_));
      return *this;
    }

    basic_url&
    operator=(
        basic_url const&other)
    {
      storage_t::operator=(other);
      url_base::operator=(url_base(&this->st_));
      return *this;
    }

};

} // urls
} // boost

#endif
