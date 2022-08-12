//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
// Copyright (c) 2022 Alan de Freitas (alandefreitas@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_PARAMS_ENCODED_HPP
#define BOOST_URL_PARAMS_ENCODED_HPP

#include <boost/url/detail/config.hpp>
#include <boost/url/params.hpp>
#include <boost/url/query_param.hpp>
#include <boost/url/string_view.hpp>
#include <boost/url/detail/parts_base.hpp>
#include <initializer_list>
#include <iterator>
#include <type_traits>

namespace boost {
namespace urls {

#ifndef BOOST_URL_DOCS
class url_base;
#endif

/** A random-access view of percent-encoded query parameters.

    The elements of this class dereference
    directly to the percent-encoded values as
    string views.

    This avoids the necessity allocating decoded
    strings.

 */
class params_encoded
    : private detail::parts_base
{
    url_base* u_ = nullptr;

    friend class url_base;

    explicit
    params_encoded(
        url_base& u) noexcept;

public:
    /** A read-only forward iterator to an encoded query parameter.

        This is a read-only forward iterator to
        the encoded query parameters.

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
    using reference = query_param_encoded_view;

    /// @copydoc reference
    using const_reference = reference;

    /** An unsigned integer type to represent sizes.
    */
    using size_type = std::size_t;

    /** A signed integer type to represent differences
     */
    using difference_type = std::ptrdiff_t;

    /** Constructor

        After the copy, both views will point to
        the same underlying object.

        Ownership is not transferred; the caller
        is responsible for ensuring the lifetime
        of the character buffer extends until
        it is no longer referenced.

        @par Complexity
        Constant

        @par Exception Safety
        Throws nothing

    */
    params_encoded(params_encoded const&) = default;

    /** Assignment

        After the assignment, both views will point to
        the same underlying object.

        Ownership is not transferred; the caller
        is responsible for ensuring the lifetime
        of the character buffer extends until
        it is no longer referenced.

        @par Complexity
            Constant

        @par Exception Safety
            Throws nothing

    */
    params_encoded&
    operator=(params_encoded const&) & = default;

    /** Assignment from initializer list

        The query parameters are replaced by the
        contents of the initializer list.

        The behavior is undefined if the elements
        belong to the container.

        @par Complexity
        Linear in the size of `init`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @return Pointer to this params object

        @param init Initializer list with query parameters

      */
    params_encoded&
    operator=(std::initializer_list<
        reference> init);

    /** Assignment from initializer list

        The query parameters are replaced by the
        contents of the initializer list.

        The behavior is undefined if the elements
        belong to the container.

        @par Complexity
        Linear in the size of `init`.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param init Initializer list with query parameters

    */
    void
    assign(std::initializer_list<
        reference> init);

    /** Assignment from iterators

        The query parameters are replaced by the
        contents of the range.

        The behavior is undefined if the elements
        belong to the container.

        @par Complexity
        Linear in the size of the input range.

        @par Exception Safety
        Strong guarantee.
        Calls to allocate may throw.

        @param first Iterator to first query parameter

        @param last Iterator to one past the last query parameter

      */
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last);

private:
    // Assignment from forward iterators
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last,
        std::forward_iterator_tag);

public:
#ifndef BOOST_URL_DOCS
    /** Assignment from input iterators
     */
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last,
           std::input_iterator_tag) = delete;
#endif

    /** Return a view of this container as percent-decoded query parameters

        @return A percent-decoded container of query parameters

    */
    BOOST_URL_DECL
    params
    decoded() const;

    //--------------------------------------------
    //
    // Element Access
    //
    //--------------------------------------------

    /** Return indexed element with bounds checking

        @return Query param reference

        @param pos Position

      */
    reference
    at(std::size_t pos) const;

    /** Return first element matching key with bounds checking

        @return Query param mapped value

        @param key Element key

      */
    BOOST_URL_DECL
    string_view
    at(string_view key) const;

    /** Return indexed element

        @return Query params reference

        @param pos Element position

      */
    BOOST_URL_DECL
    reference
    operator[](
        std::size_t pos) const;

    /** Return first element

        @return Query params reference

      */
    reference
    front() const;

    /** Return last element

        @return Query params reference
      */
    reference
    back() const;

    //--------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------

    /** Returns an iterator to the beginning of container

        @return Iterator to first element
      */
    iterator
    begin() const noexcept;

    /** Returns an iterator to one past the last element

        @return Iterator to one past the last element

      */
    iterator
    end() const noexcept;

    //--------------------------------------------
    //
    // Capacity
    //
    //--------------------------------------------

    /// Return true if the container is empty
    bool
    empty() const noexcept;

    /// Returns the number of query parameters in the url
    std::size_t
    size() const noexcept;

    /** Clears the contents of the container

        This function clears the contents of the
        container as if calling
        `erase(begin(), end())`.

      */
    void
    clear() noexcept;

    //--------------------------------------------

    /** Insert element in container

        This function inserts an element at the
        specified container position.

        The behavior is undefined if the element
        belongs to the container.

        @return Iterator pointing to the inserted element

        @param before Position at which the element should be inserted

        @param v Element to be inserted

      */
    iterator
    insert(
        iterator before,
        reference const& v);

    /** Insert elements in container

        This function inserts a list of elements
        at the specified container position.

        The behavior is undefined if the elements
        belong to the container.

        @return Iterator pointing to the first inserted element

        @param before Position at which the elements should be inserted

        @param init Elements to be inserted

      */
    iterator
    insert(
        iterator before,
        std::initializer_list<
            reference> init);

    /** Insert elements in container

        This function inserts a range of elements
        at the specified container position.

        The behavior is undefined if any elements of
        the range belong to the container.

        @return Iterator pointing to the first inserted element

        @param before Position at which the elements should be inserted

        @param first Iterator pointing to first element to be inserted

        @param last Iterator pointing to one past the last element to be inserted

      */
    template<class FwdIt>
#ifndef BOOST_URL_DOCS
    typename std::enable_if<
        !std::is_convertible<
            FwdIt, string_view
            >::value,
        iterator>::type
#else
    iterator
#endif
    insert(
        iterator before,
        FwdIt first,
        FwdIt last);

private:
    /** Insert forward range of elements at container position
     */
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

    //--------------------------------------------

    /** Replace container element

        This function replaces an element at the
        specified container position.

        The behavior is undefined if the new element
        belongs to the existing container.

        @return Iterator to position where element was inserted

        @param pos Position whose element should be replaced

        @param value New value for the position

     */
    iterator
    replace(
        iterator pos,
        reference const& value);

    /** Replace container elements

        This function replaces a range of elements
        at a range of container positions.

        The behavior is undefined if any elements of
        the range belong to the container.

        @return Iterator to position where the first inserted element

        @param from Iterator to first element to be replaced

        @param to Iterator to one past last element to be replaced

        @param first Iterator to first element to be inserted

        @param last Iterator to one past last element to be inserted

      */
    template<class FwdIt>
    iterator
    replace(
        iterator from,
        iterator to,
        FwdIt first,
        FwdIt last);

    /** Replace container elements

        This function replaces a list of elements
        at a range of container positions.

        The behavior is undefined if any elements of
        the initializer_list belong to the
        container.

        @return Iterator to position where the first inserted element

        @param from Iterator to first element to be replaced

        @param to Iterator to one past last element to be replaced

        @param init List of elements to replace the range

      */
    iterator
    replace(
        iterator from,
        iterator to,
        std::initializer_list<
            reference> init);

    /** Remove a value from the container

        @return Iterator to position where the element was removed

        @param pos Iterator to element which should be removed

    */
    BOOST_URL_DECL
    iterator
    remove_value(
        iterator pos);

    /** Replace element value

        This function replaces a value at the
        specified container position while
        maintaining the original key.

        @return Iterator to position where the element was replaced

        @param pos Position whose element should be replaced

        @param value New value for the element

     */
    BOOST_URL_DECL
    iterator
    replace_value(
        iterator pos,
        string_view value);

    //--------------------------------------------

    /** Constructs a value at container position

        @return Iterator to position where the element was constructed

        @param pos Position where the new element should be constructed

        @param key Key of the new element

        @param value Value of the new element

      */
    iterator
    replace(
        iterator pos,
        string_view key,
        string_view value);

    /** Constructs a key-only value at container position

        This function constructs a value at the
        specified container position.

        The new value has a specified query key
        and no query mapped value.

        @return Iterator to position where the element was constructed

        @param pos Position where the new element should be constructed

        @param key The key of the new element

      */
    iterator
    replace(
        iterator pos,
        string_view key);

    /** Constructs a key-only query parameter at container position

        @return Iterator to position where the element was constructed

        @param before Position where the new element should be constructed

        @param key Key of the query param

        @param value Value of the query param

      */
    iterator
    insert(
        iterator before, // used to be emplace_before
        string_view key,
        string_view value);

    /** Constructs a value at container position

        @return Iterator to position where the element was constructed

        @param before Position where the new element should be constructed

        @param key Key of the new query param

      */
    iterator
    insert(
        iterator before, // used to be emplace_before
        string_view key);

    //--------------------------------------------

    /** Erases element from container at position

        @return Iterator to position where the element was erased

        @param pos Position whose element should be erased

      */
    iterator
    erase(iterator pos);

    /** Erases range of elements from container

        @return Iterator to position where the first element was erased

        @param first Iterator pointing to the first element to be erased

        @param last Iterator pointing to one past the last element to be erased

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

    //--------------------------------------------

    /** Construct element at the last position of the container

        @return Position where the element was constructed

        @param key Element key

      */
    iterator
    append(
        string_view key);

    /** Construct element at the last position of the container

        @return Position where the element was constructed

        @param key Element key

        @param value Element value

      */
    iterator
    append(
        string_view key,
        string_view value);

    /** Insert value at the last position of the container

        @param value Value to be inserted
      */
    void
    push_back(
        reference const& value);

    /** Remove element at the last position of the container
      */
    void
    pop_back() noexcept;

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

    /** Find element with a specified key after specified position

        This function searches the range `[from, end)`.

        The range where `from==end` is also valid.

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
//#include <boost/url/impl/params_encoded.hpp>

#endif
