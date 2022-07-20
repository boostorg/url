//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_HPP
#define BOOST_URL_PARAMS_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/query_param.hpp>
#include <boost/url/pct_encoded_view.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <initializer_list>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url;
class params_encoded;
#endif

/** A random-access view of percent-decoded query parameters in a url.

   This view represents the range of percent-decoded
   query parameters in a url.

   Its iterator dereferences to read-only decoded
   query parameters in the underlying url.

 */
class params
    : private detail::parts_base
{
    friend class url;
    friend class params_encoded;

    url* u_ = nullptr;

    params(url& u) noexcept
        : u_(&u)
    {}

public:
    /** A read-only forward iterator to a decoded query parameter.

        This is a read-only forward iterator to
        the decoded query parameters.

     */
#ifdef BOOST_URL_DOCS
    using iterator = __see_below__;
#else
    class iterator;
#endif

    /// @copydoc iterator
    using const_iterator = iterator;

    /** A type which can represent a parameter as a value

        This type allows for making a copy of
        a parameter where ownership is retained
        in the copy.

    */
    using value_type = query_param;

    /** A type which can represent a parameter as a const reference

        This type does not make a copy of a parameter
        and ownership is retained by the container.
    */
    using reference = query_param_view;

    /// @copydoc reference
    using const_reference = reference;

    /** An unsigned integer type to represent sizes.
    */
    using size_type = std::size_t;

    /** A signed integer type to represent differences
    */
    using difference_type = std::ptrdiff_t;

    /** Constructor

        Copy constructor

    */
    params(params const&) = default;

    /** View assignment

        After the assignment, both views will point to
        the same underlying object.

    */
    params&
    operator=(params const&) & = default;

    /** Assignment

        The query parameters are replaced by the
        contents of the initializer list.

        The key and value strings must not
        reference the underlying URL buffer, or
        else the behavior is undefined.

        @param init Initializer list with query parameters

     */
    params&
    operator=(std::initializer_list<query_param_view> init);

    /** Assignment

        The query parameters are replaced by the
        contents of the initializer list.

        The key and value strings must not
        reference the underlying URL buffer, or
        else the behavior is undefined.

        @param init Initializer list with query parameters

     */
    void
    assign(std::initializer_list<
        query_param_view> init);

    /** Assignment

        The query parameters are replaced by the
        contents of the range.

        The key and value strings must not
        reference the underlying URL buffer, or
        else the behavior is undefined.

        @param first An iterator to the beginning of the query parameter range

        @param last An iterator to the end of the query parameter range

     */
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last);

private:
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last,
        std::forward_iterator_tag);

public:
#ifndef BOOST_URL_DOCS
    // Doxygen cannot render ` = delete`
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last,
        std::input_iterator_tag) = delete;
#endif

    //--------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------

    /** Returns an iterator to the beginning

        Returns an iterator to the beginning of
        the container

        @return Iterator to beginning of the container

     */
    const_iterator
    begin() const noexcept;

    /** Returns an iterator to the end

        @return Iterator to the end of the container

     */
    iterator
    end() const noexcept;

    //--------------------------------------------
    //
    // Capacity
    //
    //--------------------------------------------

    /** Checks whether the container is empty

        @return True if container is empty

     */
    bool
    empty() const noexcept;

    /** Return the number of elements

        @return Number of elements in the container

     */
    std::size_t
    size() const noexcept;

    /** Clears the contents of the container

        This function clears the contents of the
        container as if calling
        `erase(begin(), end())`.

     */
    void
    clear() noexcept;

    /** Insert element

        This function insert an element at the
        specified container position.

        @return Iterator pointing to the new inserted element

        @param before Position at which the element should be inserted

        @param v Element to be inserted

      */
    iterator
    insert(
        iterator before,
        query_param_view const& v);

    /** Insert elements

        This function inserts a list of elements
        at the specified container position

        The key and value strings must not
        reference the underlying URL buffer, or
        else the behavior is undefined.

        @return Iterator pointing to the first inserted element

        @param before Position at which the elements should be inserted

        @param init Elements to be inserted

     */
    iterator
    insert(
        iterator before,
        std::initializer_list<
            query_param_view> init);

    /** Insert elements

        This function inserts a range of elements
        at the specified container position.

        The key and value strings must not
        reference the underlying URL buffer, or
        else the behavior is undefined.

        @return Iterator pointing to the first inserted element

        @param before Position at which the elements should be inserted

        @param first An iterator to the beginning of the query parameter range

        @param last An iterator to the end of the query parameter range

     */
    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last);

private:
    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last,
        std::forward_iterator_tag);

public:
#ifndef BOOST_URL_DOCS
    // Doxygen cannot render ` = delete`
    template<class FwdIt>
    iterator
    insert(
            iterator before,
            FwdIt first,
            FwdIt last,
            std::input_iterator_tag) = delete;
#endif

     /** Replace element

        This function replaces an element at
        the specified container position.

        @return Iterator to position where element was inserted

        @param pos Position whose element should be replaced

        @param value New value for the position

     */
    iterator
    replace(
        iterator pos,
        query_param_view const& value);

     /** Replace elements

        This function replaces a range of elements
        at a range of container positions

        The key and value strings must not
        reference the underlying URL buffer, or
        else the behavior is undefined.

        @return Iterator to position where the first inserted element

        @param from Iterator to beginning of the range to be replaced

        @param to Iterator to end of the range to be replaced

        @param first An iterator to the beginning of range to be inserted

        @param last An iterator to the end of the range to be inserted

     */
    template<class FwdIt>
    iterator
    replace(
        iterator from,
        iterator to,
        FwdIt first,
        FwdIt last);

     /** Replace elements

        This function replaces a list of elements
        at a range of container positions

        The key and value strings must not
        reference the underlying URL buffer, or
        else the behavior is undefined.

        @return Iterator to position where the first inserted element

        @param from Iterator to beginning of the range to be replaced

        @param to Iterator to end of the range to be replaced

        @param init List of elements to replace the range

     */
    iterator
    replace(
        iterator from,
        iterator to,
        std::initializer_list<
            query_param_view> init);

     /** Remove a query parameter from the container

        @return Iterator to position where the element was removed

        @param pos Iterator to element which should be removed

     */
    BOOST_URL_DECL
    iterator
    remove_value(
        iterator pos);

     /** Replace element

        This function replaces a query parameter
        at the specified container position

        @return Iterator to position where the element was replaced

        @param pos Position whose element should be replaced

        @param value New value for the element

     */
    BOOST_URL_DECL
    iterator
    replace_value(
        iterator pos,
        string_view value);

    /** Constructs a value at container position

        @return Iterator to position where the element was constructed

        @param pos Position where the new element should be constructed

        @param key Key of the new element

        @param value Value of the new element

     */
    iterator
    emplace_at(
        iterator pos,
        string_view key,
        string_view value);

    /** Constructs a key-only value at container position

        This function constructs a value at the
        specified container position.

        The new value has a specified query key and no query mapped value.

        @return Iterator to position where the element was constructed

        @param pos Position where the new element should be constructed

        @param key The key of the new element

      */
    iterator
    emplace_at(
        iterator pos,
        string_view key);

    /** Constructs a key-only query parameter at container position

        @return Iterator to position where the element was constructed

        @param before Position where the new element should be constructed

        @param key Key of the query param

        @param value Value of the query param

      */
    iterator
    emplace_before(
        iterator before,
        string_view key,
        string_view value);

    /** Constructs a value at container position

        @return Iterator to position where the element was constructed

        @param before Position where the new element should be constructed

        @param key Key of the new query param

      */
    iterator
    emplace_before(
        iterator before,
        string_view key);

    /** Erases element from container at specified position

        @return Iterator to position where the element was erased

        @param pos Position whose element should be erased

      */
    iterator
    erase(iterator pos);

    /** Erases range of elements from container

        @return Iterator to position where the first element was erased

        @param first An iterator to the beginning of the query parameter range

        @param last An iterator to the end of the query parameter range

      */
    BOOST_URL_DECL
    iterator
    erase(
        iterator first,
        iterator last);

    /** Erases elements associated with a specified key from container

        @return Number of elements erased from the container

        @param key Key of the element to be erased

      */
    BOOST_URL_DECL
    std::size_t
    erase(string_view key) noexcept;

    /** Construct key-only element at the end of the container

        @return Position where the element was constructed

        @param key String-like element key

      */
    iterator
    emplace_back(
        string_view key);

    /** Construct element at the end of the container

        @return Position where the element was constructed

        @param key Element key

        @param value Element value

      */
    iterator
    emplace_back(
        string_view key,
        string_view value);

    /** Insert value at the end of the container

        @param value Value to be inserted

      */
    void
    push_back(
        query_param_view const& value);

    //--------------------------------------------
    //
    // Lookup
    //
    //--------------------------------------------

    /** Count number of elements with a specified key

        @return Number of elements with a specified key

        @param key Element key

      */
    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    /** Find element with a specified key

        @return Iterator pointing to element with a specified key

        @param key Element key

      */
    iterator
    find(string_view key) const noexcept;

    /** Find element with a specified key after position

        This function searches the range [`from`, `end`).

        `from==end` is valid.

        @return Iterator pointing to element with the specified key

        @param from First position to consider in search

        @param key Element key

      */
    BOOST_URL_DECL
    iterator
    find(
        iterator from,
        string_view key) const noexcept;

    /** Check if container contains an element with the specified key

        @return True if container contains an element with the specified key

        @param key Element key

      */
    bool
    contains(string_view key) const noexcept;

};

} // urls
} // boost

// VFALCO This include is at the bottom of
// url.hpp because of a circular dependency
//#include <boost/url/impl/params.hpp>

#endif
