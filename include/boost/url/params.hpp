//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_PARAMS_HPP
#define BOOST_URL_PARAMS_HPP

#include <boost/url/config.hpp>
#include <boost/url/detail/char_type.hpp>
#include <utility>

namespace boost {
namespace url {

class view;

/** A read-only container referencing the URL query parameters.
*/
class params
{
    using id_type = unsigned;

    view const* v_ = nullptr;

public:
    class value_type;
    class iterator;
    using const_iterator = iterator;

    params() = default;
    params(params const&) = default;
    params& operator=(params const&) = default;

    explicit
    params(view const& v)
        : v_(&v)
    {
    }

    inline
    bool
    empty() const noexcept;

    inline
    std::size_t
    size() const noexcept;

    inline
    iterator
    begin() const noexcept;

    inline
    iterator
    end() const noexcept;

    BOOST_URL_DECL
    bool
    contains(string_view key) const noexcept;

    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    BOOST_URL_DECL
    iterator
    find(string_view key) const noexcept;

    inline
    value_type
    operator[](std::size_t i) const;

    BOOST_URL_DECL
    std::string
    operator[](string_view key) const;

    BOOST_URL_DECL
    value_type
    at(std::size_t i) const;

    template<class Allocator =
        std::allocator<char>>
    BOOST_URL_DECL
    string_type<Allocator>
    at( string_view key,
        Allocator const& a = {}) const;
};

//----------------------------------------------------------

class params::value_type
{
    view const* v_;
    id_type i_;

    friend class params;

    value_type(
        view const* v,
        id_type i)
        : v_(v)
        , i_(i)
    {
    }

public:
    value_type() = delete;
    value_type& operator=(value_type const&) = delete;

    value_type(value_type const&) = default;

    BOOST_URL_DECL
    string_view
    encoded_key() const noexcept;

    BOOST_URL_DECL
    string_view
    encoded_value() const noexcept;

    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    key(Allocator const& a = {}) const;

    template<
        class Allocator =
            std::allocator<char>>
    string_type<Allocator>
    value(Allocator const& a = {}) const;

    value_type const*
    operator->() const noexcept
    {
        return this;
    }

    operator
    std::pair<
        std::string const,
        std::string>() const
    {
        return {key(), value()};
    }
};

} // url
} // boost

#include <boost/url/impl/params.hpp>
#ifdef BOOST_URL_HEADER_ONLY
#include <boost/url/impl/params.ipp>
#endif

#endif
