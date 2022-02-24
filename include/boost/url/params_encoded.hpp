//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
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
class url;
#endif

/** A random-access handle to percent-encoded query parameters in a url.

    Unlike @ref params, this class dereferences to percent-encoded values,
    which avoids the necessity allocating instances of @ref const_string for the
    conversion.

 */
class params_encoded
    : private detail::parts_base
{
    friend class url;

    /** A pointer to the underlying url that contains these query parameters
     */
    url* u_ = nullptr;

    /** Construct query params from a parsed url

        @param u The url with the query params
     */
    inline
    explicit
    params_encoded(
        url& u) noexcept;

public:
    /** A random-access iterator referencing parameters in a url query.

        Dereferencing this iterator returns the underlying encoded
        query params.

     */
    class iterator;

    /** A type which can represent a parameter as a value
      */
    using value_type = query_param_view;

    /** A type which can represent a parameter as a value also used as reference
     */
    using reference = query_param_view;

    /** A type which can represent a parameter as a value also used as const reference
     */
    using const_reference = query_param_view;

    /** An unsigned integer type to represent sizes
     */
    using size_type = std::size_t;

    /** A signed integer type to represent differences
     */
    using difference_type = std::ptrdiff_t;

    /** <!-- @name Members -->
     *  <!-- @{ -->
     */

    /** Assignment

        After the assignment, both views will point to
        the same underlying object.
    */
    params_encoded&
    operator=(params_encoded const&) & = default;

    /** Assignment from initializer list

        Assign params from a list of param value_type.

        Each value_type is a query <key, value> pair.

        @return Pointer to this params object

        @param init Initializer list with query parameters

      */
    inline
    params_encoded&
    operator=(std::initializer_list<
        value_type> init);

    /** Assignment from initializer list

        Assign params from a list of param value_type.

        Each instance of @ref value_type is a query <key, value> pair.

        @return Pointer to this params object

        @param init Initializer list with query parameters

    */
    inline
    void
    assign(std::initializer_list<
        value_type> init);

    /** Assignment from iterators

        Assign params from a range of params value_type.

        Each value_type is a query <key, value> pair.

        @tparam FwdIt Forward Iterator Type

        @return Pointer to this params object

        @param first Iterator to first query parameter

        @param last Iterator to one past the last query parameter

      */
    template<class FwdIt>
    void
    assign(FwdIt first, FwdIt last);

private:
    /** Assignment from forward iterators
      */
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

        @tparam Allocator An Allocator type

        @return A percent-decoded container of query parameters

        @param alloc Allocator to create read-only strings for decoded strings

    */
    template<class Allocator =
        std::allocator<char>>
    params
    decoded(Allocator const& alloc = {}) const;

    /** <!-- @} --> */

    /** <!-- @name Element Access -->
     *  <!-- @{ -->
     */

    /** Access element at given position with bounds checking

        @return Query param reference

        @param pos Position

      */
    inline
    value_type
    at(std::size_t pos) const;

    /** Access element with given key with bounds checking

        @return Query param mapped value

        @param key Element key

      */
    BOOST_URL_DECL
    string_view
    at(string_view key) const;

    /** Access element at given position without bounds checking

        @return Query params reference

        @param pos Element position

      */
    BOOST_URL_DECL
    value_type
    operator[](
        std::size_t pos) const;

    /** Access first query param in the container

        @return Query params reference

      */
    inline
    value_type
    front() const;

    /** Access last query param in the container

        @return Query params reference
      */
    inline
    value_type
    back() const;

    /** <!-- @} --> */

    /** <!-- @name Iterators -->
        <!-- @{ -->
     */

    /** Returns an iterator to the beginning of container

        @return Iterator to first element
      */
    inline
    iterator
    begin() const noexcept;

    /** Returns an iterator to one past the last element

        @return Iterator to one past the last element

      */
    inline
    iterator
    end() const noexcept;

    /** <!-- @} --> */

    /** <!-- @name Capacity -->
        <!-- @{ -->
     */

    /** Checks whether the container is empty

        @return True if container is empty
      */
    inline
    bool
    empty() const noexcept;

    /** Returns the number of query parameters in the url

        @return The number of query parameters in the url

     */
    inline
    std::size_t
    size() const noexcept;

    /** <!-- @} --> */

    /** <!-- @name Modifiers -->
        <!-- @{ -->
     */

    /** Clears the contents of the container

        Clears the contents of the container as if calling
        `erase(begin(), end())`.

      */
    inline
    void
    clear() noexcept;

    //--------------------------------------------

    /** Insert element at specified container position

        @note Behavior is undefined if the element
        belongs to the container

        @return Iterator pointing to the inserted element

        @param before Position at which the element should be inserted

        @param v Element to be inserted

      */
    inline
    iterator
    insert(
        iterator before,
        value_type const& v);

    /** Insert list of elements at specified container position

        @return Iterator pointing to the first inserted element

        @param before Position at which the elements should be inserted

        @param init Elements to be inserted

      */
    inline
    iterator
    insert(
        iterator before,
        std::initializer_list<
            value_type> init);

    /** Insert range of elements at specified container position

        @note Behavior is undefined if any elements of the range
        belong to the container

        @tparam FwdIt Iterator type

        @return Iterator pointing to the first inserted element

        @param before Position at which the elements should be inserted

        @param first Iterator pointing to first element to be inserted

        @param last Iterator pointing to one past the last element to be inserted

      */
    template<class FwdIt>
    iterator
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
    /** Insert input range of elements at container position
     */
    template<class FwdIt>
    iterator
    insert(
        iterator before,
        FwdIt first,
        FwdIt last,
        std::input_iterator_tag) = delete;
#endif

    //--------------------------------------------

    /** Replace an element at specified container position

        @note Behavior is undefined if the element
        belongs to the container

        @return Iterator to position where element was inserted

        @param pos Position whose element should be replaced

        @param value New value for the position

     */
    inline
    iterator
    replace(
        iterator pos,
        value_type const& value);

    /** Replace a range of elements at a range of container positions

        @note Behavior is undefined if any elements of the range
        belong to the container

        @tparam FwdIt Iterator type

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

    /** Replace a a list of elements at a range of container positions

        @note Behavior is undefined if any elements of the initializer_list
        belong to the container

        @return Iterator to position where the first inserted element

        @param from Iterator to first element to be replaced

        @param to Iterator to one past last element to be replaced

        @param init List of elements to replace the range

      */
    inline
    iterator
    replace(
        iterator from,
        iterator to,
        std::initializer_list<
            value_type> init);

    /** Remove a value from the container

        @return Iterator to position where the element was removed

        @param pos Iterator to element which should be removed

    */
    BOOST_URL_DECL
    iterator
    remove_value(
        iterator pos);

    /** Replace a value at container position

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
    inline
    iterator
    emplace_at(
        iterator pos,
        string_view key,
        string_view value);

    /** Constructs a key-only value at container position

        This function constructs a value at the container position.
        The new value has a given query key and no query mapped value.

        @return Iterator to position where the element was constructed

        @param pos Position where the new element should be constructed

        @param key The key of the new element

      */
    inline
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
    inline
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
    inline
    iterator
    emplace_before(
        iterator before,
        string_view key);

    //--------------------------------------------

    /** Erases element from container at position

        @return Iterator to position where the element was erased

        @param pos Position whose element should be erased

      */
    inline
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

    /** Erases elements associated with a given key from container

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
    inline
    iterator
    emplace_back(
        string_view key);

    /** Construct element at the last position of the container

        @return Position where the element was constructed

        @param key Element key

        @param value Element value

      */
    inline
    iterator
    emplace_back(
        string_view key,
        string_view value);

    /** Insert value at the last position of the container

        @param value Value to be inserted
      */
    inline
    void
    push_back(
        value_type const& value);

    /** Remove element at the last position of the container
      */
    inline
    void
    pop_back() noexcept;

    /** <!-- @} --> */

    /** <!-- @name Lookup -->
     *  <!-- @{ -->
     */

    /** Count number of elements with a given key

        @return Number of elements with a given key

        @param key Element key

      */
    BOOST_URL_DECL
    std::size_t
    count(string_view key) const noexcept;

    /** Find element with a given key

        @return Iterator pointing to element with a given key

        @param key Element key

      */
    inline
    iterator
    find(string_view key) const noexcept;

    /** Find element with a given key after specified position

        This function searches the range [from, end).

        from==end is valid.

        @return Iterator pointing to element with the given key

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
    inline
    bool
    contains(string_view key) const noexcept;

    /** <!-- @} --> */
};

} // urls
} // boost

// VFALCO This include is at the bottom of
// url.hpp because of a circular dependency
//#include <boost/url/impl/params_encoded.hpp>

#endif
