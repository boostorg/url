//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_SEGMENTS_REF_HPP
#define BOOST_URL_SEGMENTS_REF_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/segments_base.hpp>
#include <initializer_list>
#include <iterator>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_base;
class segments_view;
#endif

class segments_ref
    : public segments_base
{
    url_base* u_ = nullptr;

    friend class url_base;
    friend class segments_encoded_ref;

    segments_ref(
        url_base& u) noexcept;

public:
    //--------------------------------------------
    //
    // Special Members
    //
    //--------------------------------------------

    segments_ref(
        segments_ref const& other) = default;

    /** @{ */
    segments_ref&
    operator=(segments_ref const& other);

    segments_ref&
    operator=(segments_view const& other);
    /** @} */

    segments_ref&
    operator=(std::initializer_list<
        string_view> init);

    /** Conversion
    */
    operator
    segments_view() const noexcept;

    //--------------------------------------------

    void
    clear() noexcept;

    template<class FwdIt>
#ifdef BOOST_URL_DOCS
    void
#else
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            string_view>::value>::type
#endif
    assign(FwdIt first, FwdIt last);

    BOOST_URL_DECL
    iterator
    insert(
        iterator before,
        string_view s);

    iterator
    insert(
        iterator before,
        std::initializer_list<string_view> init);
    template<class FwdIt>
#ifdef BOOST_URL_DOCS
    iterator
#else
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            string_view>::value,
        iterator>::type
#endif
    insert(
        iterator before,
        FwdIt first,
        FwdIt last);

    //--------------------------------------------

    iterator
    replace(
        iterator pos,
        string_view s);

    iterator
    replace(
        iterator from,
        iterator to,
        std::initializer_list<
            string_view> init);

    template<class FwdIt>
#ifdef BOOST_URL_DOCS
    iterator
#else
    typename std::enable_if<
        std::is_convertible<typename
            std::iterator_traits<
                FwdIt>::reference,
            string_view>::value,
        iterator>::type
#endif
    replace(
        iterator from,
        iterator to,
        FwdIt first,
        FwdIt last);

    iterator
    erase(
        iterator pos) noexcept;

    BOOST_URL_DECL
    iterator
    erase(
        iterator first,
        iterator last) noexcept;

    void
    push_back(
        string_view s);

    void
    pop_back() noexcept;

private:
    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last,
        std::input_iterator_tag) = delete;

    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last,
        std::forward_iterator_tag);
};

} // urls
} // boost

// VFALCO This include is at the bottom of
// url.hpp because of a circular dependency
//
// #include <boost/url/impl/segments_ref.hpp>

#endif
