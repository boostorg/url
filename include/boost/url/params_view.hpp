//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_VIEW_HPP
#define BOOST_URL_PARAMS_VIEW_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/const_string.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_view;
class params_encoded_view;
#endif

class params_view
    : private detail::parts_base
{
    friend class url_view;
    friend class params_encoded_view;

    string_view s_;
    std::size_t n_;
    const_string::allocator a_;

    template<class Allocator>
    params_view(
        string_view s,
        std::size_t n,
        Allocator const& a);

public:
    class iterator;

    class value_type
    {
    public:
        const_string key;
        const_string value;
        bool has_value;

        BOOST_URL_DECL
        ~value_type() noexcept;

        BOOST_URL_DECL
        value_type() noexcept;

        BOOST_URL_DECL
        value_type(
            value_type const& other) noexcept;

        BOOST_URL_DECL
        value_type&
        operator=(
            value_type const& other) noexcept;

    private:
        friend class params_view;
        friend class iterator;

        BOOST_URL_DECL
        value_type(
            char const* s,
            std::size_t nk,
            std::size_t nv,
            const_string::allocator a);
    };

    using reference = value_type;
    using const_reference = value_type;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    //--------------------------------------------
    //
    // Element Access
    //
    //--------------------------------------------

    BOOST_URL_DECL
    const_string
    at(string_view key) const;

    //--------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------

    BOOST_URL_DECL
    iterator
    begin() const noexcept;

    BOOST_URL_DECL
    iterator
    end() const noexcept;

    //--------------------------------------------
    //
    // Capacity
    //
    //--------------------------------------------

    inline
    bool
    empty() const noexcept;

    inline
    std::size_t
    size() const noexcept;

    //--------------------------------------------
    //
    // Lookup
    //
    //--------------------------------------------

    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    inline
    iterator
    find(string_view key) const noexcept;

    /** Search [from, end), from==end is valid
    */
    BOOST_URL_DECL
    iterator
    find(
        iterator from,
        string_view key) const noexcept;

    inline
    bool
    contains(string_view key) const noexcept;
};

} // urls
} // boost

// VFALCO This include is at the bottom of
// url_view.hpp because of a circular dependency
//#include <boost/url/impl/params_view.hpp>

#endif
